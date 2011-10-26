"""
M. Littlefield
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
import unittest
from geometry.ConfigReader import Configreader

class test_config_reader(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    This class tests the module ConfigReader.py
    which reads the configuration defaults and makes
    the info available for the geometry files. This test
    ensures it is doing what it is expected to.
    """
    def setUp(self): #pylint: disable = C0103
        """
        method set_up This method creates a Configreader
        object ready to test
        """
        self.testcase = Configreader()
        
    def test_readconfig(self):
        """
        These tests work by reading the defaults from
        the ConfigurationDeafaults(Geometry) and compares them
        to what is expected in this file.
        """
        self.testcase.readconfig()
        gdml_dir = os.environ['MAUS_ROOT_DIR'] + \
        '/src/common_py/geometry/testCases/testGeometry'
        self.assertEqual(self.testcase.gdmldir, gdml_dir, \
        'GDML Directory not found, testConfigReader::test_readconfig')
        test_note = 'This is a standard note It uses the standard test case'
        self.assertEqual(self.testcase.geometrynotes, test_note, \
              'Note not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.zipfile, 0, \
        'Zip file arg not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.deleteoriginals, 0, \
        'Delete originals arg not found, testConfigReader::test_readconfig')
        test_download = os.environ['MAUS_ROOT_DIR'] + \
                    '/src/common_py/geometry/Download/'
        self.assertEqual(self.testcase.downloaddir, test_download, \
        'Download directory not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.starttime, '2011-09-08 09:00:00', \
                'Start time not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.stoptime, None, \
        'Start time not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.runnum, 1, \
        'Start time not found, testConfigReader::test_readconfig')

if __name__ == '__main__':
    unittest.main()
