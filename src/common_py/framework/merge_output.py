"""
Multi-process dataflows module.
"""

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

from datetime import datetime
import json
import signal
import sys

from docstore.DocumentStore import DocumentStoreException
from framework.utilities import DataflowUtilities
from framework.utilities import DocumentStoreUtilities
from framework.workers import WorkerBirthFailedException
from framework.workers import WorkerDeathFailedException

import maus_cpp.run_action_manager

class MergeOutputExecutor: # pylint: disable=R0903, R0902
    """
    @class MergeOutputExecutor
    Execute merge-output MAUS dataflows reading spills from a document
    store.

    This is the algorithm used to merge spills and pass the results to
    an output stream: 
    @verbatim
    run_number = None
    end_of_run = None
    is_birthed = FALSE
    last_time = 01/01/1970
    WHILE TRUE
      READ spills added since last time from document store
      IF spill IS "end_of_run" 
        end_of_run = spill
      IF spill_run_number != run_number
        IF is_birthed
          IF end_of_run == None
              end_of_run = 
                  {"daq_event_type":"end_of_run",  "run_number":run_number}
          Send end_of_run to merger
          DEATH merger and outputter
        BIRTH merger and outputter
        run_number = spill_run_number
        end_of_run = None
        is_birthed = TRUE
      MERGE and OUTPUT spill
    Send END_OF_RUN block to merger
    DEATH merger and outputter
    @endverbatim
    This assumes that all spills for run N-1 have time stamps less
    than that of spills for run N.

    is_birthed is used to ensure that there is no BIRTH-DEATH-BIRTH
    redundancy on receipt of the first spill. 

    If a document store is not given to the constructor then this
    class expects a document store class to be specified in the JSON
    configuration e.g.  
    @verbatim
    doc_store_class = "MongoDBDocumentStore.MongoDBDocumentStore"
    @endverbatim
    The document store class itself may have additional configuration
    requirements.  

    If a collection name is given to the constructor then this is
    used. Otherwise there must be a doc_collection_name key in the
    configuration.
    """

    def __init__(self, merger, outputer, config_doc, doc_store = None, collection_name = None): # pylint: disable=R0913,C0301
        """
        Save references to arguments and parse the JSON configuration
        document, then connect to the document store. 
        @param self Object reference.
        @param merger Merger task.
        @param outputer Output task.
        @param config_doc JSON configuration document.
        @param doc_store Document store.
        @param collection_name Collection name in document store.
        @throws ValueError if collection_name is None and there is no
        "doc_collection_name" entry in config_doc.
        @throws KeyError If any required configuration parameters are
        missing.
        @throws ValueError If any configuration values are invalid.
        @throws TypeError If any dynamic class does not implement
        a required interface.
        @throws DocumentStoreException if there is a problem
        connecting to the document store.
        """
        self.last_time = datetime(1970, 01, 01)
        self.job_footer = None
        self.merger = merger
        self.outputer = outputer
        self.config_doc = config_doc
        self.config = json.loads(self.config_doc)
        # Current run number (from spills).
        self.run_number = None
        # Last end_of_run spill received.
        self.end_of_run_spill = None
        # Counts of spills processed.
        self.spill_process_count = 0
        self.write_headers = self.config["header_and_footer_mode"] == "append"
        # Connect to doc store.
        if (doc_store == None):
            self.doc_store = DocumentStoreUtilities.setup_doc_store(\
                self.config)
        else:
            self.doc_store = doc_store
        # Get collection name.
        if (collection_name == None):
            if (not self.config.has_key("doc_collection_name") or
               (self.config["doc_collection_name"] in [None, ""])):
                raise ValueError("collection is not specified")
            else:
                self.collection = self.config["doc_collection_name"]
        else:   
            self.collection = collection_name

    def start_of_job(self, job_header):
        """
        Birth the outputer and save job header
        """
        print("-------- MERGE OUTPUT --------")
        # Register CTRL-C handler.
        signal.signal(signal.SIGINT, self.handle_ctrl_c) 
        print("BIRTH outputer %s" % self.outputer.__class__)
        birth = self.outputer.birth(self.config_doc)
        if not(birth == True or birth == None): # new-style birth() returns None
            raise WorkerBirthFailedException(self.outputer.__class__)
        if self.write_headers:
            self.outputer.save(json.dumps(job_header))

 
    def start_of_run(self):
        """
        Prepare for a new run by updating the local run number then
        birthing the merger and outputer. 
        @param self Object reference.
        @param run_number New run number.
        @throws WorkerBirthFailedException if birth returns False.
        @throws Exception if there is a problem when birth is called.
        """
        self.end_of_run_spill = None
        print "---------- START RUN %d ----------" % self.run_number
        print("BIRTH merger %s" % self.merger.__class__)
        birth = self.merger.birth(self.config_doc)
        if not(birth == True or birth == None): # new-style birth() returns None
            raise WorkerBirthFailedException(self.merger.__class__)
        run_header = maus_cpp.run_action_manager.start_of_run(self.run_number)
        if self.write_headers:
            self.outputer.save(run_header)

    def end_of_run(self):
        """
        End a run by sending an end_of_run spill through the merger
        and outputer then death the merger and outputer. The end_of_run
        spill is the last one that was encountered before a change
        in run was detected. If there was no such end_of_run then
        a dummy is created.
        @param self Object reference.
        @throws WorkerDeathFailedException if death returns False.
        @throws Exception if there is a problem when passing the
        end_of_run through or when death is called.
        """
        print("Finishing current run...sending end_of_run to merger")
        if (self.end_of_run_spill == None):
            print "  Missing an end_of_run spill..."
            print "  ...creating one to flush the mergers!"
            self.end_of_run_spill = {
                "scalars":{},
                "emr_spill_data":{},
                "spill_number":-1,
                "run_number":int(self.run_number),
                "daq_event_type":"end_of_run",
                "recon_events":[],
                "mc_events":[],
                "maus_event_type":"Spill",
            }
        end_of_run_spill_str = json.dumps(self.end_of_run_spill)
        self.merger.process(end_of_run_spill_str)
        print("DEATH merger %s" % self.merger.__class__)
        death = self.merger.death()
        if not(death == True or death == None): # new-style death() returns None
            raise WorkerDeathFailedException(self.merger.__class__)
        run_footer = maus_cpp.run_action_manager.end_of_run(self.run_number)
        if self.write_headers:
            self.outputer.save(run_footer)
        print "---------- END RUN %d ----------" % self.run_number

    def end_of_job(self, job_footer):
        """
        Output footer and then death the outputer
        """
        if self.write_headers:
            self.outputer.save(json.dumps(job_footer))
        print("DEATH outputer %s" % self.outputer.__class__)
        death = self.outputer.death()
        if not(death == True or death == None): # new-style death() returns None
            raise WorkerDeathFailedException(self.outputer.__class__)

    def execute(self, job_header, job_footer, will_run_until_ctrl_c=True):
        """
        Execute the dataflow. Retrieve spills from document store and
        submit to merge and output workers. A date query is run on
        the document store so that only the spills added since the
        previous query are processed on each iteration.
        @param self Object reference.
        @param job_header JobHeader maus_event to be written at start_of_job
        @param job_footer JobFooter maus_event to be written at end_of_job
        @throws RabbitMQException if RabbitMQ cannot be contacted.
        @throws DocumentStoreException if there is a problem
        using the document store.
        @throws WorkerBirthFailedException if birth returns False.
        @throws WorkerDeathFailedException if death returns False.
        @throws Exception if there is a problem when merging or
        outputting a spill, birthing or deathing the merger or
        outputer or merging an end_of_run spill.
        """
        self.job_footer = job_footer
        self.start_of_job(job_header)
        run_again = True # always run once
        while run_again:
            run_again = will_run_until_ctrl_c
            try:
                docs = self.doc_store.get_since(self.collection, self.last_time)
            except Exception as exc:
                sys.excepthook(*sys.exc_info())
                raise DocumentStoreException(exc)
            # Iterate using while, not for, since docs is an
            # iterator which streams data from database and we
            # want to detect database errors.
            while True:
                try:
                    doc = docs.next()
                except StopIteration:
                    # No more data so exit inner loop.
                    break
                except Exception as exc:
                    raise DocumentStoreException(exc)
                doc_id = doc["_id"]
                doc_time = doc["date"]
                spill = doc["doc"]
                print "Read spill %s (dated %s)" % (doc_id, doc_time)
                if (doc_time > self.last_time):
                    self.last_time = doc_time
                # Check for change in run.
                spill_doc = json.loads(spill)
                spill_run_number = DataflowUtilities.get_run_number(spill_doc)
                if (DataflowUtilities.is_end_of_run(spill_doc)):
                    self.end_of_run_spill = spill_doc
                if (spill_run_number != self.run_number):
                    print "Change of run detected"
                    if (self.run_number != None):
                        # Death workers only if birthed.
                        self.end_of_run()
                    self.run_number = spill_run_number
                    self.start_of_run()
                # Handle current spill.
                print "Executing Merge for spill %s\n" % doc_id,
                merged_spill = self.merger.process(spill)
                print "Executing Output for spill %s\n" % doc_id,
                if not self.outputer.save(str(merged_spill)):
                    print "Failed to execute Output"
                self.spill_process_count += 1
                print "Spills processed: %d" % self.spill_process_count
        # Finish the final run.
        print "No more data and set to stop - ending run"
        self.end_of_run()
        print "Ending job"
        self.end_of_job(self.job_footer)

    def handle_ctrl_c(self, signum, frame): # pylint: disable=W0613
        """
        Handler to break out of execute loop when CTRL-C is
        pressed. The merger and outputer are deathed before
        sys.exit(0) is invoked
        @param self Object reference.
        @param signum Signal number.
        @param frame. Frame.
        """
        self.end_of_run()
        self.end_of_job(self.job_footer)
        sys.exit(0)

    @staticmethod
    def get_dataflow_description():
        """
        Get dataflow description.
        @return description.
        """
        description = "Run MAUS merge-output in multi-processing mode. This\n"  
        description += "requires MongoDB to be installed. See\n" 
        description += "the wiki links on how to do this at\n"
        description += \
            "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSDevs\n"
        return description
