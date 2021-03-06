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

"""
InputPyJSON inputs data as a json document into the MAUS framework
"""

import json
import gzip

import ErrorHandler

class InputPyJSON:
    """Reads JSON documents from files/sockets

    This class inputs from a python file object that
    is passed as the only argument 'arg_file' to the
    constructor.  This allows the class to input
    from any Python file object:
    - an uncompressed ASCII file where each line
    represents an spill document.  One can use
    the command 'python -mjson.tool' to view the
    spills in a more human-readable fashion. For
    example 'cat filename | python -mjson.tool';
    <b>arg_file=open('filename', 'w')</b>
    - a gzip-compressed file that can be
    decompressed either with InputPyJSON within
    MAUS or by the Linux tools gunzip/gzip;
    <b>arg_file=GzipFile('filename', 'w')</b>;
    see http://docs.python.org/library/gzip.html
    - a socket;
    http://docs.python.org/library/socket.html
    - etc...
    .

    """

    def __init__(self, arg_file=None, arg_number_of_events=-1):
        """Setup InputPyJSON

        \param arg_file python file object.
        <b>Default: open('mausput','r')</b>
        \param arg_number_of_events number of events to
        read in.  If -1, read forever or until
        the end of the file.
        <b>Default: -1</b>
        """
        self._file = arg_file
        self._number_of_events = arg_number_of_events
        self._current_event = 0

    def birth(self, config_document = "{}"):
        """
        birth() opens the json file based on datacards

        @param json_config json string holding a dict of json parameters
        - input_json_file_name datacard controls file name to open
        - input_json_file_type datacard controls file type (either 'text' or 
          'gzip'
        @returns True on success, False on failure

        If self._file is opens self._file file.
        """
        try:
            if self._file == None:
                config = json.loads(config_document)
                fname = config["input_json_file_name"]
                if config["input_json_file_type"] == "gzip":
                    self._file = gzip.GzipFile(fname, 'r')
                elif config["input_json_file_type"] == "text":
                    self._file = open(fname, 'r')
                else:
                    raise IOError('Did not recognise input_json_file_type '+\
                                  str(config["input_json_file_type"]))
            return True
        except Exception: #pylint: disable=W0703
            ErrorHandler.HandleException({}, self)
            return False

    def emitter(self):
        """Emit JSON documents

        This is a python generator that yields
        JSON documents until the end of the file
        or until the maximum number of events is hit.
        """

        # this returns an empty string if
        # there is nothing to read, hence
        # while loop ahead
        next_value = self._file.readline()

        while next_value != '':
            # remove \n and whitespace
            next_value = next_value.rstrip()

            # see if EOF is reached
            if next_value == '':
                return

            # check that it's a valid JSON document
            json.loads(next_value)

            # have we gone past the number of events we wanted?
            if self._number_of_events >= 0 and\
                    self._current_event >= self._number_of_events:
                return

            # yield the current event (google 'python generators' if confused)
            yield next_value
            self._current_event += 1
            next_value = self._file.readline()


    def death(self):
        """
        death() closes input file
        """
        self._file.close()
        return True

