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


# pylint: disable=C0103

"""
Test for OutputCppRoot
"""

import unittest
import json
import ROOT
import os

import _InputCppRoot as InputCppRoot

class TestInputCppRoot(unittest.TestCase): # pylint: disable=R0904
    """
    Test we can write out ROOT tree.

    Check we can birth and death correctly; check we can write simple ROOT trees
    especially in context of - how are errors handled - 
    """
    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202, R0915, R0914
        """
        Make a sample TFile
        """
        self.fname = os.path.join \
                  (os.environ["MAUS_ROOT_DIR"], "tmp", "test_inputCppRoot.root")
        root_file = ROOT.TFile(self.fname, "RECREATE") # pylint: disable = E1101
        spill = ROOT.MAUS.Spill() # pylint: disable = E1101
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = ROOT.TTree("Spill", "TTree") # pylint: disable = E1101
        tree.Branch("data", data, data.GetSizeOf(), 1)
        spill.SetScalars(ROOT.MAUS.Scalars()) # pylint: disable = E1101
        spill.SetDAQData(ROOT.MAUS.DAQData()) # pylint: disable = E1101
        spill.SetMCEvents(ROOT.MAUS.MCEventPArray()) # pylint: disable = E1101
        spill.SetReconEvents(ROOT.MAUS.ReconEventPArray()) # pylint: disable = E1101, C0301
        # test branch makes segmentation fault... from ROOT side
        # spill.SetTestBranch(ROOT.MAUS.TestBranch()) # pylint: disable = E1101
        spill.SetRunNumber(10)
        data.SetSpill(spill)
        for num in range( 1, 11 ) : # Add 10 spills
            spill.SetSpillNumber(num)
            tree.Fill()
        spill.SetRunNumber(11)
        for num in range( 1, 3 ) : # Add 2 spills
            spill.SetSpillNumber(num)
            tree.Fill()
        spill.SetRunNumber(12)
        for num in range( 1, 3 ) : # Add 2 spills
            spill.SetSpillNumber(num)
            tree.Fill()
        tree.Write()

        job_header_data = ROOT.MAUS.JobHeaderData() # pylint: disable = E1101
        job_header = ROOT.MAUS.JobHeader() # pylint: disable = E1101
        tree2 = ROOT.TTree("JobHeader", "TTree") # pylint: disable = E1101
        tree2.Branch("job_header", job_header_data,
                    job_header_data.GetSizeOf(), 1)
        job_header.SetJsonConfiguration("mushrooms")
        job_header_data.SetJobHeader(job_header)
        tree2.Fill()
        job_header.SetJsonConfiguration("omelette")
        tree2.Fill()
        tree2.Write()

        job_footer_data = ROOT.MAUS.JobFooterData() # pylint: disable = E1101
        job_footer = ROOT.MAUS.JobFooter() # pylint: disable = E1101
        tree3 = ROOT.TTree("JobFooter", "TTree") # pylint: disable = E1101
        tree3.Branch("job_footer", job_footer_data,
                    job_footer_data.GetSizeOf(), 1)
        job_footer_data.SetJobFooter(job_footer)
        tree3.Fill()
        tree3.Fill()
        tree3.Write()

        run_header_data = ROOT.MAUS.RunHeaderData() # pylint: disable = E1101
        run_header = ROOT.MAUS.RunHeader() # pylint: disable = E1101
        tree4 = ROOT.TTree("RunHeader", "TTree") # pylint: disable = E1101
        tree4.Branch("run_header", run_header_data,
                    run_header_data.GetSizeOf(), 1)
        run_header_data.SetRunHeader(run_header)
        run_header.SetRunNumber(10)
        tree4.Fill()
        tree4.Fill()
        run_header.SetRunNumber(11)
        tree4.Fill()
        run_header.SetRunNumber(13)
        tree4.Fill()
        tree4.Write()

        run_footer_data = ROOT.MAUS.RunFooterData() # pylint: disable = E1101
        run_footer = ROOT.MAUS.RunFooter() # pylint: disable = E1101
        tree5 = ROOT.TTree("RunFooter", "TTree") # pylint: disable = E1101
        tree5.Branch("run_footer", run_footer_data,
                    run_footer_data.GetSizeOf(), 1)
        run_footer_data.SetRunFooter(run_footer)
        run_footer.SetRunNumber(10)
        tree5.Fill()
        tree5.Fill()
        run_footer.SetRunNumber(11)
        tree5.Fill()
        run_footer.SetRunNumber(13)
        tree5.Fill()
        tree5.Write()

        root_file.Close()

    def test_birth_death(self):
        """
        Check that we can birth and death properly
        """
        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"input_root_file_name":self.fname}))
        inputter.death()
        inputter.death()
        inputter.birth(json.dumps({"input_root_file_name":self.fname}))
        inputter.death()

        inputter_2 = InputCppRoot.InputCppRoot(self.fname)
        inputter_2.death()

        inputter_3 = InputCppRoot.InputCppRoot()
        inputter_3.death()

    def __test_event(self, inputter, checks):
        """
        Run the inputter, check that output contains key:value pair.
        """
        event = inputter.emitter().next()
        json_event = json.loads(event)
        for key, value in checks.iteritems():
            self.assertEqual(json_event[key], value,
              msg=str(key)+":"+str(value)+"\n"+json.dumps(json_event, indent=2))

    def test_read_events(self):
        """
        Try reading a realistic data structure
        """
        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"input_root_file_name":self.fname,
                                   "selected_spills":[]}))
        # job header
        self.__test_event(inputter, {"maus_event_type":"JobHeader",
                           "json_configuration":"mushrooms"})
        self.__test_event(inputter, {"maus_event_type":"JobHeader",
                           "json_configuration":"omelette"})
        # run 10
        self.__test_event(inputter, {"maus_event_type":"RunHeader",
                                     "run_number":10})
        self.__test_event(inputter, {"maus_event_type":"RunHeader",
                                     "run_number":10})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":1})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":2})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":3})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":4})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":5})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":6})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":7})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":8})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":9})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":10})
        self.__test_event(inputter, {"maus_event_type":"RunFooter",
                                     "run_number":10})
        self.__test_event(inputter, {"maus_event_type":"RunFooter",
                                     "run_number":10})
        # run 11
        self.__test_event(inputter, {"maus_event_type":"RunHeader",
                                     "run_number":11})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":11,
                                     "spill_number":1})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":11,
                                     "spill_number":2})
        self.__test_event(inputter, {"maus_event_type":"RunFooter",
                                     "run_number":11})
        # run 12 & 13 - note this is a bit of a bug
        self.__test_event(inputter, {"maus_event_type":"RunHeader",
                                     "run_number":13})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":12,
                                     "spill_number":1})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":12,
                                     "spill_number":2})
        self.__test_event(inputter, {"maus_event_type":"RunFooter",
                                     "run_number":13})

        # job footers
        self.__test_event(inputter, {"maus_event_type":"JobFooter"})
        self.__test_event(inputter, {"maus_event_type":"JobFooter"})

        # out of events
        self.assertEqual(inputter.emitter().next(), "")

    def test_read_spills_only(self):
        """
        Try reading a data structure with missing tree
        """
        fname = os.path.join \
                (os.environ["MAUS_ROOT_DIR"], "tmp", "test_inputCppRoot_2.root")
        root_file = ROOT.TFile(fname, "RECREATE") # pylint: disable = E1101

        spill = ROOT.MAUS.Spill() # pylint: disable = E1101
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree2 = ROOT.TTree("Spill", "TTree") # pylint: disable = E1101
        tree2.Branch("data", data, data.GetSizeOf(), 1)
        spill.SetScalars(ROOT.MAUS.Scalars()) # pylint: disable = E1101
        spill.SetDAQData(ROOT.MAUS.DAQData()) # pylint: disable = E1101
        spill.SetMCEvents(ROOT.MAUS.MCEventPArray()) # pylint: disable = E1101
        spill.SetReconEvents(ROOT.MAUS.ReconEventPArray()) # pylint: disable = E1101, C0301
        data.SetSpill(spill)
        tree2.Fill()
        tree2.Fill()
        tree2.Write()

        root_file.Close()

        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"input_root_file_name":fname}))

        # job footer
        self.__test_event(inputter, {"maus_event_type":"Spill"})
        self.__test_event(inputter, {"maus_event_type":"Spill"})
        self.assertEqual(inputter.emitter().next(), "")

    def test_read_job_header_footer_only(self):
        """
        Try reading a data structure with missing tree
        """
        fname = os.path.join \
                (os.environ["MAUS_ROOT_DIR"], "tmp", "test_inputCppRoot_3.root")
        root_file = ROOT.TFile(fname, "RECREATE") # pylint: disable = E1101

        spill = ROOT.MAUS.Spill() # pylint: disable = E1101
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = ROOT.TTree("Spill", "TTree") # pylint: disable = E1101
        tree.Branch("data", data, data.GetSizeOf(), 1)
        spill.SetScalars(ROOT.MAUS.Scalars()) # pylint: disable = E1101
        spill.SetDAQData(ROOT.MAUS.DAQData()) # pylint: disable = E1101
        spill.SetMCEvents(ROOT.MAUS.MCEventPArray()) # pylint: disable = E1101
        spill.SetReconEvents(ROOT.MAUS.ReconEventPArray()) # pylint: disable = E1101, C0301
        data.SetSpill(spill)
        tree.Write()

        job_header_data = ROOT.MAUS.JobHeaderData() # pylint: disable = E1101
        job_header = ROOT.MAUS.JobHeader() # pylint: disable = E1101
        tree2 = ROOT.TTree("JobHeader", "TTree") # pylint: disable = E1101
        tree2.Branch("job_header", job_header_data,
                    job_header_data.GetSizeOf(), 1)
        job_header.SetJsonConfiguration("mushrooms")
        job_header_data.SetJobHeader(job_header)
        tree2.Fill()
        job_header.SetJsonConfiguration("omelette")
        tree2.Fill()
        tree2.Write()

        job_footer_data = ROOT.MAUS.JobFooterData() # pylint: disable = E1101
        job_footer = ROOT.MAUS.JobFooter() # pylint: disable = E1101
        tree3 = ROOT.TTree("JobFooter", "TTree") # pylint: disable = E1101
        tree3.Branch("job_footer", job_footer_data,
                    job_footer_data.GetSizeOf(), 1)
        job_footer_data.SetJobFooter(job_footer)
        tree3.Fill()
        tree3.Fill()
        tree3.Write()

        root_file.Close()

        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps({"input_root_file_name":fname}))
        
        # job header
        self.__test_event(inputter, {"maus_event_type":"JobHeader",
                           "json_configuration":"mushrooms"})
        self.__test_event(inputter, {"maus_event_type":"JobHeader",
                           "json_configuration":"omelette"})
        # job footers
        self.__test_event(inputter, {"maus_event_type":"JobFooter"})
        self.__test_event(inputter, {"maus_event_type":"JobFooter"})
        self.assertEqual(inputter.emitter().next(), "")


    def _test_read_selected_spills( self ):
        """
          Tests the functionality to select individual spills from a data file.
          Selection is performed by using an array of spill numbers passed via
          the configuration file.
        """
        inputter = InputCppRoot.InputCppRoot()
        inputter.birth(json.dumps( { "input_root_file_name":self.fname, \
                                                "selected_spills":[2, 5, 8] } ))
        self.assertTrue(inputter.useSelectedSpills())
        # job header
        self.__test_event(inputter, {"maus_event_type":"JobHeader",
                           "json_configuration":"mushrooms"})
        self.__test_event(inputter, {"maus_event_type":"JobHeader",
                           "json_configuration":"omelette"})
        self.__test_event(inputter, {"maus_event_type":"RunHeader",
                                     "run_number":10})
        self.__test_event(inputter, {"maus_event_type":"RunHeader",
                                     "run_number":10})
        # run 10
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":2})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":5})
        self.__test_event(inputter, {"maus_event_type":"Spill",
                                     "run_number":10,
                                     "spill_number":8})
        self.__test_event(inputter, {"maus_event_type":"RunFooter",
                                     "run_number":10})
        self.__test_event(inputter, {"maus_event_type":"RunFooter",
                                     "run_number":10})
       

if __name__ == "__main__":
    unittest.main()

