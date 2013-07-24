# This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""Tests for ReduceCppTofCalib"""
# pylint: disable = C0103

import os
import json
import unittest
from Configuration import Configuration
import MAUS

class ReduceCppTofCalibTestCase(unittest.TestCase):# pylint: disable = R0904
    """Tests for ReduceCppTofCalib"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """init a ReduceCppTofCalib object"""
        cls.reducer = MAUS.ReduceCppTofCalib()
        cls.c = Configuration()

    def test_empty(self):
        """Check against configuration is empty"""
        result = self.reducer.birth("")
        self.assertFalse(result)
        result = self.reducer.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_init(self):
        """Check that birth works properly"""
        success = self.reducer.birth(self. c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        """Check that against data stream is empty"""
        test1 = ('%s/src/reduce/ReduceCppTofCalib/noDataTest.txt' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.reducer.process(data)
        spill = json.loads(result)
        no_slab_hits = True
        if 'slab_hits' in spill:
            no_slab_hits = False
        self.assertTrue(no_slab_hits)

    def test_process(self):
        """Check ReduceCppTofCalib process function"""
        test2 = ('%s/src/reduce/ReduceCppTofCalib/processTest.txt' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with some crazy events.
        self.reducer.process(data)

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that death works ok"""
        success = cls.reducer.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        cls.reducer = None

if __name__ == '__main__':
    unittest.main()