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
test_for_mapcpptrackerdigits
"""
# pylint: disable=C0103
# pylint: disable=R0904

import json
import unittest
import os
from Configuration import Configuration

from MAUS import MapCppTrackerDigits
import maus_cpp.globals
import maus_cpp.converter

# Disable: Too many public methods
# pylint: disable-msg=R0904
# Disable: Invalid name
# pylint: disable-msg=C0103
# Disable: Class method should have 'cls' as first argument
# pylint: disable-msg=C0202

class MapCppTrackerDigitsTestCase(unittest.TestCase):
    """ The MapCppTrackerDigits test."""

    cfg = json.loads(Configuration().getConfigJSON())
    cfg['reconstruction_geometry_filename'] = 'Stage6.dat'

    @classmethod
    def setUpClass(self):
        """ Class Initializer.
            The set up is called before each test function is called.
        """
        self.mapper = MapCppTrackerDigits()
        # Test whether the configuration files were loaded correctly at birth
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        self.mapper.birth(json.dumps(self.cfg))

    def testDeath(self):
        """ Test to make sure death occurs """
        self.mapper.death()

    # def testBadData(self):
    #    """Check can handle nonsense json input data"""
    #    result = self.mapper.process("blah")
    #    spill_out = json.loads(result)
    #    self.assertTrue('errors' in spill_out)
    #    self.assertTrue("bad_json_document" in spill_out['errors'])

    def testProcess(self):
        """ Test of the process function """
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        assert root_dir != None
        assert os.path.isdir(root_dir)
        _filename = \
        '%s/src/map/MapCppTrackerDigits/lab7_unpacked' % root_dir
        assert os.path.isfile(_filename)
        _file = open(_filename, 'r')
        # File is open.
        # Spill 1 is corrupted.
        spill_1 = _file.readline().rstrip()
        output_1 = self.mapper.process(spill_1)
        self.assertFalse(
                      "recon_events" in maus_cpp.converter.json_repr(output_1))
        # Spill 2 is sane.
        print "*********************** WARNING - TRACKER UNPACKING WILL FAIL "+\
              "DUE TO INCOMPATIBLE DATA STRUCTURE (rogers) ******************"
        spill_2 = _file.readline().rstrip()
        output_2 = self.mapper.process(spill_2)
        self.assertTrue("errors" in maus_cpp.converter.json_repr(output_2))
        # spill 3 is end of event
        spill_3 = _file.readline().rstrip()
        output_3 = self.mapper.process(spill_3)
        self.assertTrue("errors" in maus_cpp.converter.json_repr(output_3))
        # Close file.
        _file.close()

    @classmethod
    def tear_down_class(self):
        """___"""
        self.mapper.death()
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
