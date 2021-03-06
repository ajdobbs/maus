# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
#

"""Tests for ReduceCppPatternRecognition"""
# pylint: disable = C0103
import json
import unittest
from Configuration import Configuration
# import ROOT
import MAUS

# pylint: disable = R0904
class ReduceCppPatternRecognitionTestCase(unittest.TestCase):
    """Test for ReduceCppPatternRecognition"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Initialize a reducer object"""
        cls.reducer = MAUS.ReduceCppPatternRecognition()
        cls.c = Configuration()

    def test_empty(self):
        """Check against empty configuration"""
        result = self.reducer.birth("")
        self.assertFalse(result)
        result = self.reducer.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_init(self):
        """Check that birth works properly"""
        success = self.reducer.birth(self.c.getConfigJSON())
        self.assertTrue(success)

    #def test_process_helical(self):
        #"""Check ReduceCppPatternRecognition process function helical"""
        #fname = ('%s/src/reduce/ReduceCppPatternRecognition/h_spills.json' %
                 #os.environ.get("MAUS_ROOT_DIR"))
        #fin = open(fname, 'r')
        ## Remove header lines
        #fin.readline()
        #fin.readline()
        ## Add some helical track events
        #print 'Adding first spill'
        #data = fin.readline()
        #self.reducer.process(data)
        #print 'Adding second spill'
        #data = fin.readline()
        #self.reducer.process(data)
        ##pylint: disable = E1101
        #print 'Checking output file'
        #self.reducer.death()
        #foutput = ROOT.TFile('reduce_pattern_recognition.root')
        #htracks = foutput.Get('htracks')
        #self.assertEqual(htracks.GetEntries(), 20)

    #def test_process_straight(self):
        #"""Check ReduceCppPatternRecognition process function straight"""
        #fname = ('%s/src/reduce/ReduceCppPatternRecognition/s_spills.json' %
                 #os.environ.get("MAUS_ROOT_DIR"))
        #fin = open(fname, 'r')
        ## Remove header lines and first spill
        #fin.readline()
        #fin.readline()
        #fin.readline()
        #fin.readline()
        ## Add some straight track events
        #print 'Adding first spill'
        #data = fin.readline()
        #self.reducer.process(data)
        #print 'Adding second spill'
        #data = fin.readline()
        #self.reducer.process(data)
        ##pylint: disable = E1101
        #print 'Checking output file'
        #self.reducer.death()
        #foutput = ROOT.TFile('reduce_pattern_recognition.root')
        #stracks = foutput.Get('stracks')
        #self.assertEqual(stracks.GetEntries(), 9)

    def tearDown(self): #pylint: disable = C0103
        """Check death works"""
        success = self.reducer.death()
        if not success:
            raise Exception('Initialize Fail', 'Could not start worker')
        #self.reducer = None


if __name__ == '__main__':
    unittest.main()
