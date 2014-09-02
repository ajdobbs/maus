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

"""Tests for MapCppEmrMCDigitization"""

import os
import json
import unittest
from Configuration import Configuration
import MAUS

class TestMapCppEmrMCDigitization(unittest.TestCase): #pylint: disable=R0904
    """Tests for MapCppEmrMCDigitization"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppEmrMCDigitization()
        cls.c = Configuration()

    def test_empty(self):
        """Check can handle empty configuration"""
        result = self.mapper.birth("")
        self.assertFalse(result)
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"] or 
                        "no_channel_map" in doc["errors"])

    def test_init(self):
        """Check birth with default configuration"""
        success = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppEmrMCDigitization/noDataTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = json.loads(result)
        n_ev = len(spill_out['recon_events'])
        #print spill_out["errors"]
        self.assertEqual(0, n_ev)
        self.assertFalse("bad_json_document" in spill_out["errors"])
        self.assertFalse("bad_cpp_data" in spill_out["errors"])

    #def test_process(self):
        #"""Test MapCppEmrMCDigitization process method"""

if __name__ == "__main__":
    unittest.main()

