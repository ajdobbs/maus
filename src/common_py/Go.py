"""
Go controls the running of executables - map reduce functionality etc.
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

import os
import json
import sys

# MAUS
from Configuration import Configuration

def get_possible_dataflows():
    """
    Enumerate list of possible types of dataflow
    """
    possible_types_of_dataflow = {}

    description  = "Run in a pipeline programming fashion with only a\n"
    description += "single thread.  See Wikipedia on 'pipeline\n"
    description += "programming' for more information."
    possible_types_of_dataflow['pipeline_single_thread'] = description

    description  = "Run MICE how it is run in the control room.  This\n"
    description += "requires CouchDB and celery to be installed. See\n"
    description += "the wiki links on how to do this at\n"
    description += \
        "http://micewww.pp.rl.ac.uk/projects/maus/wiki/MAUSCeleryConfiguration\n" # pylint:disable=C0301
    possible_types_of_dataflow['multi_process'] = description

    description  = "Distribute as much work as possible on the local\n"
    description += "machine using Python's multiprocessing library."
    possible_types_of_dataflow['many_local_threads'] = description

    return possible_types_of_dataflow

def buffer_input(the_emitter, number_spills):
    """
    Buffer the input stream by only reading the first
    1024 spills into memory.  Returns an array of spills.
    """
    my_buffer = []

    for i in range(number_spills):  # pylint: disable=W0612
        try:
            value = next(the_emitter)
            my_buffer.append(value.encode('ascii'))
        except StopIteration:
            return my_buffer

    return my_buffer

class Go:  #  pylint: disable=R0921
    """
    @class Go
    The Go class will driving other MAUS components.  The types of components
    are:
      - input: where does the data come from?
      - transformer: transform a spill to a new state (stateless)
      - reduce: process spill based on previous spills (has state)
      - output: where the data goes
    and there are different dataflow models:
      - pipeline_single_thread
    """

    def __init__(self, arg_input, arg_transformer, arg_merger,  # pylint: disable=R0913,C0301
                 arg_outputer, arg_config_file=None, command_line_args = True):
        """
        Initialise the configuration dictionary, input, mapper, merger and
        outputer
        @param arg_input
        @param arg_mapper
        @param arg_merger
        @param arg_outputer
        @param arg_config_file Configuration file
        @param command_line_args If set to true, use command line arguments to
               handle configuration and throw a SystemExit exception if
               invalid arguments are passed. (Note some third party items, e.g.
               nosetests, have their own command line arguments that are
               incompatible with MAUS's)
        """
        #  Determing what the 'env.sh' has set the user's environment to.  Bail
        #  otherwise with an exception.
        maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
        if maus_root_dir == "":
            raise Exception("MAUS_ROOT_DIR environmental variable not set")

        #  Warn the user that they could be using the wrong version of MAUS.
        #  os.getcwd() is the current directory from which the script is being
        #  executed.  This warns the user to many common errors.
        if maus_root_dir not in os.getcwd():
            print("\nWARNING: YOU ARE RUNNING MAUS OUTSIDE ITS MAUS_ROOT_DIR")
            print("WARNING:\tMAUS_ROOT_DIR = %s" % (maus_root_dir))
            print("WARNING:\tCURRENT DIRECTORY = %s\n" % (os.getcwd()))

        #  Keep copies of the arguments
        self.input = arg_input
        self.transformer = arg_transformer
        self.merger = arg_merger
        self.outputer = arg_outputer

        #  Get a copy of the configuration JSON document and keep it local
        conf  = Configuration()
        c_doc = conf.getConfigJSON(arg_config_file, command_line_args)
        self.json_config_document = c_doc

        #  Parse the configuration JSON
        json_config_dictionary = json.loads(self.json_config_document)

        #  How should we 'drive' the components?
        type_of_dataflow = json_config_dictionary['type_of_dataflow']

        #  Grab version
        version = json_config_dictionary["maus_version"]

        #  Print some MAUS info... userful for when the user wants to know
        #  what they ran after the fact.  The PID can be used if you want to
        #  kill MAUS at the command line: kill -s 9 PID_NUMBER
        print("Welcome to MAUS:")
        print(("\tProcess ID (PID): %d" % os.getpid()))
        print(("\tProgram Arguments: %s" % str(sys.argv)))
        print ("\tVersion: %s" % version)
        if json_config_dictionary["verbose_level"] == 0:
            print "Configuration: ", \
                json.dumps(json_config_dictionary, indent=2)

        #
        #  Enumerate list of possible types of dataflow
        #
        if type_of_dataflow == 'pipeline_single_thread':
            self.pipeline_single_thread()
        elif type_of_dataflow == 'multi_process':
            self.multi_process()
        elif type_of_dataflow == 'many_local_threads':
            self.many_local_threads()  #  not implemented
        else:
            raise LookupError("bad type_of_dataflow: %s" % type_of_dataflow)


    def pipeline_single_thread(self):
        """
        MAUS pipeline dataflow
        
        Drive the MAUS components in a pipeline where each spill is passed
        through transform, merge, and output, then next spill is progressed.
        """


        print("INPUT: Reading some input")
        assert(self.input.birth(self.json_config_document) == True)
        emitter = self.input.emitter()
        map_buffer = buffer_input(emitter, 1)

        print("TRANSFORM: Setting up transformer (this can take a while...)")
        assert(self.transformer.birth(self.json_config_document) == True)

        print("MERGE: Setting up merger")
        assert(self.merger.birth(self.json_config_document) == True)

        print("OUTPUT: Setting up outputer")
        assert(self.outputer.birth(self.json_config_document) == True)

        print("PIPELINE mode: TRANSFORM, MERGE, OUTPUT, then next spill.")

        #  This helps us time how long the setup that sometimes happens in the
        # first spill takes
        print("HINT: MAUS will process 1 spill only at first...")

        i = 0
        while len(map_buffer) != 0:
            for spill in map_buffer:
                spill = self.transformer.process(spill)
                spill = self.merger.process(spill)
                self.outputer.save(spill)

            i += len(map_buffer)
            map_buffer = buffer_input(emitter, 128)

            #  Not python 3 compatible print() due to backward compatability
            print "TRANSFORM/MERGE/OUTPUT: ",
            print "Processed %d spills so far," % i,
            print("%d spills in buffer." % (len(map_buffer)))

        print("TRANSFORM: Shutting down transformer")
        assert(self.transformer.death() == True)

        print("MERGE: Shutting down merger")
        assert(self.merger.death() == True)

        print("OUTPUT: Shutting down outputer")
        assert(self.outputer.death() == True)

    def setup_data_store_proxy(self):
        """
        Set up data store for multi-process execution. The data store
        is configured via the following parameters in the JSON
        configuration:

        -data_store_class - data store class name. If omitted, then
         the default of "InMemoryDataStore" is used.
        -data_store_module - module in which data store class is. If
         omitted then this is set to be equal to the data_store_class.

        It is assumed that the data store class can be loaded by
        execution of "import data_store_class from data_store_module".
        The "connect" method of the data store is then invoked to 
        initialise the connection.

        @param self Object reference.
        @return data store object.
        @throws ImportError. If the module name do not exist.
        @throws AttributeError. If the class is not in the module.
        @throws KeyError. If there is no data_store_class in the JSON
        configuration.
        """
        json_config_dictionary = json.loads(self.json_config_document)

        # Get class and bust up into module path and class name.
        data_store_class = json_config_dictionary["data_store_class"]
        path = data_store_class.split(".")
        data_store_class = path.pop()
        data_store_module = ".".join(path)

        # Dynamically import the module.
        module_object = __import__(data_store_module)
        # Get class object.
        class_object = getattr(module_object, data_store_class)
        # Create instance of class object.
        data_store = class_object()
        # Connect to the data store.
        data_store.connect(json_config_dictionary)

        return data_store

    def multi_process(self):
        """
        MAUS control room style multi-processing dataflow
        
        Drive the MAUS components in a distributed task queue where
        each tranform is done in a concurrent processing step using
        Celery, the results then being driven through a merge and
        output.
        """
        # Create and connect to the data store proxy.
        data_store = self.setup_data_store_proxy()

        from celery.task.control import inspect
        # Check for active workers.
        print("Checking for active workers")
        inspection = inspect()
        active_workers = inspection.active()
        if (active_workers == None):
            raise Exception("No active Celery workers!")

        print("INPUT: Reading some input")
        assert(self.input.birth(self.json_config_document) == True)
        emitter = self.input.emitter()
        map_buffer = buffer_input(emitter, 1)

        print("TRANSFORM: spawning transform jobs for each spill.")
        i = 0
        transform_results = {}
        while len(map_buffer) != 0:
            for spill in map_buffer:
#                from maustasks import MausSimulationTask
                from maustasks import MausDoNothingTask
                result = \
                    MausDoNothingTask.delay(spill) # pylint:disable=E1101
                # Index results by spill_id so can present
                # results to merge-output in same order.
                transform_results[i] = result
                i += 1
            map_buffer = buffer_input(emitter, 128)
            print " Processed %d spills so far," % i,
            print(" %d spills left in buffer." % (len(map_buffer)))

        print("TRANSFORM: waiting for transform jobs to complete.")
        # Wait for workers to finish - just keep looping until each
        # either succeeds or fails.
        spills = {}
        while len(transform_results) != 0:
            for spill_id in transform_results.keys():
                result = transform_results[spill_id]
                if result.successful():
                    del transform_results[spill_id]
                    print " Spill %d task %s OK " % (spill_id, result.task_id)
                    # Index results by spill_id so can present
                    # results to merge-output in same order.
                    spill = result.result
                    spills[spill_id] = spill
                    data_store.put(str(spill_id), spill)
                elif result.failed():
                    del transform_results[spill_id]
                    print " Spill %d task %s FAILED " \
                        % (spill_id, result.task_id)
                    print "Exception in worker: %s " % result.result
                    print "Traceback in worker: %s " % result.traceback
                else:
                    continue
        print("TRANSFORM: transform jobs completed.")

        print("MERGE: Setting up merger")
        assert(self.merger.birth(self.json_config_document) == True)

        print("OUTPUT: Setting up outputer")
        assert(self.outputer.birth(self.json_config_document) == True)

        print("MULTI PROCESS mode: MERGE, OUTPUT, then next spill.")

        i = 0
        for spill_id in data_store.ids():
            print spill_id
            spill = data_store.get(spill_id)
            print "  Executing Merge->Output for spill %s\n" % spill_id,
            spill = self.merger.process(spill)
            self.outputer.save(spill)
            data_store.delete(spill_id)
            i += 1
            print("  %d spills left in buffer." % (len(data_store)))

        print("MERGE: Shutting down merger")
        assert(self.merger.death() == True)

        print("OUTPUT: Shutting down outputer")
        assert(self.outputer.death() == True)

        print "Spills in data_store..."
        for spill_id in data_store.ids():
            print spill_id
        print "...!"

    def many_local_threads(self):
        """
        Not implemented
        """
        raise NotImplementedError()
