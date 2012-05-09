"""
Go controls the running of MAUS dataflows.
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
from framework.input_transform import InputTransformExecutor
from framework.merge_output import MergeOutputExecutor
from framework.multi_process import MultiProcessExecutor
from framework.single_thread import PipelineSingleThreadDataflowExecutor

class Go: # pylint: disable=R0921, R0903
    """
    @class Go

    Go drives the execution of a MAUS dataflow. The types of
    components in a MAUS dataflow are: 

    - input: where the data comes from,
    - transformer: transform a spill to a new state (stateless),
    - reduce: process spill based on previous spills (has state),
    - output: where the data goes.

    There are different execution models e.g.

    - pipeline_single_thread - execute the complete dataflow for each
      spill in a single-threaded pipeline.
    - multi_process - submit spills to remove worker nodes for
      transformer application then merge and output the transformed
      spills.
    """

    def __init__(self, inputer, transformer, merger, outputer, config_file=None, command_line_args = True): # pylint: disable=R0913,C0301
        """
        Initialise the configuration dictionary and delegate execution
        to the relevant dataflow executor specified in the
        configuration. 
        
        @param self Object reference.
        @param inputer Input task.
        @param transformer Transformer task.
        @param merger Merger task.
        @param outputer Output task.
        @param config_file Configuration file.
        @param command_line_args If set to True, use command line
        arguments to handle configuration and throw a SystemExit
        exception if invalid arguments are passed. (Note some third
        party components, e.g. nosetests, have their own command
        line arguments that are incompatible with MAUS's).
        @throws Exception if MAUS_ROOT_DIR is not set or there are
        any other execution problems.
        """
        # Check MAUS_ROOT_DIR.
        Go.check_maus_root_dir()

        # Load the MAUS JSON configuration overriding it with the
        # contents of the given configuration file and command
        # line arguments.
        configuration  = Configuration()
        json_config_doc = configuration.getConfigJSON(
            config_file, command_line_args)

        # Parse the configuration JSON.
        json_config_dictionary = json.loads(json_config_doc)
        # How should we 'drive' the components?
        type_of_dataflow = json_config_dictionary['type_of_dataflow']
        # Grab version
        version = json_config_dictionary["maus_version"]

        # Print MAUS run info... userful for when the user wants to
        # know what they ran after the fact.  The PID can be used if
        # you want to kill MAUS at the command line: kill -s 9
        # PID_NUMBER.
        print("Welcome to MAUS:")
        print(("\tProcess ID (PID): %d" % os.getpid()))
        print(("\tProgram Arguments: %s" % str(sys.argv)))
        print ("\tVersion: %s" % version)
        if json_config_dictionary["verbose_level"] == 0:
            print "Configuration: ", \
                json.dumps(json_config_dictionary, indent=2)

        # Set up the dataflow executor.
        if type_of_dataflow == 'pipeline_single_thread':
            executor = PipelineSingleThreadDataflowExecutor(
                inputer, transformer, merger, outputer, json_config_doc)
        elif type_of_dataflow == 'multi_process':
            executor = MultiProcessExecutor(
                inputer, transformer, merger, outputer, json_config_doc)
        elif type_of_dataflow == 'multi_process_input_transform':
            executor = InputTransformExecutor( \
                inputer, transformer, json_config_doc)
        elif type_of_dataflow == 'multi_process_merge_output':
            executor = MergeOutputExecutor( \
                merger, outputer, json_config_doc)
        elif type_of_dataflow == 'many_local_threads':
            raise NotImplementedError()
        else:
            raise LookupError("bad type_of_dataflow: %s" % type_of_dataflow)

        # Execute the dataflow.
        print("INITIATING EXECUTION")
        executor.execute()
        print("DONE")

    @staticmethod
    def check_maus_root_dir():
        """
        Check that the MAUS_ROOT_DIR environment variable has been set
        and is equal to the current directory. 
 
        @throws Exception if MAUS_ROOT_DIR is not set.
        """
        # Determine what the 'env.sh' has set the user's environment
        # to. Exit if not set.
        #  otherwise with an exception.
        maus_root_dir = os.environ.get('MAUS_ROOT_DIR')
        if maus_root_dir == "":
            raise Exception("MAUS_ROOT_DIR environmental variable not set")

        # Warn the user that they could be using the wrong version of
        # MAUS. os.getcwd() is the current directory from which the
        # script is being executed. This warns the user to many common
        # errors. 
        if maus_root_dir not in os.getcwd():
            print("MAUS_ROOT_DIR = %s" % (maus_root_dir))
            print("Current working directory = %s\n" % (os.getcwd()))
    
    @staticmethod
    def get_possible_dataflows():
        """
        Enumerate list of possible types of dataflow.

        @return description strings indexed by dataflow type.
        """
        possible_types_of_dataflow = {}
        possible_types_of_dataflow['pipeline_single_thread'] = \
            PipelineSingleThreadDataflowExecutor.get_dataflow_description()
        possible_types_of_dataflow['multi_process'] = \
            MultiProcessExecutor.get_dataflow_description() 
        possible_types_of_dataflow['multi_process_input_transform'] = \
            InputTransformExecutor.get_dataflow_description()
        possible_types_of_dataflow['multi_process_merge_output'] = \
            MergeOutputExecutor.get_dataflow_description() 
        return possible_types_of_dataflow
