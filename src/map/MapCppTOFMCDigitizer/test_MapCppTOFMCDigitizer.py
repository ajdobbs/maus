#pylint: disable = C0103
""" 
basic test of TOFDigitizer to see if digits come out on real spills
and if errors come out on empty
""" 
import unittest
import os
import json
import maus_cpp.converter
import MAUS

from Configuration import Configuration

#from MapCppTOFMCDigitizer import MapCppTOFMCDigitizer

class MapCppTOFMCDigitizer(unittest.TestCase):  #pylint: disable = R0904
    """ basic MapCppTOFMCDigitizer test to check
    if we get digits on good evts and errors on bad
    """
    @classmethod
    def setUpClass(cls):  # pylint: disable-msg=C0103
        """ Class Initializer.
            The set up is called before each test function
            is called.
        """
        cls.mapper = MAUS.MapCppTOFMCDigitizer()
        conf_json = json.loads(Configuration().getConfigJSON())
        conf_json["reconstruction_geometry_filename"] = "Stage6.dat"
        conf_json["TOF_calib_by"] = "date"
        # Test whether the configuration files were loaded correctly at birth
        cls.mapper.birth(json.dumps(conf_json))

    def test_death(self):
        """ Test to make sure death occurs """
        self.mapper.death()

    def test_process(self):
        """ Test of the process function """
        root_dir = os.environ.get("MAUS_ROOT_DIR")
        assert root_dir != None
        assert os.path.isdir(root_dir)
        _filename = \
        '%s/src/map/MapCppTOFMCDigitizer/mc_test.json' % root_dir
        assert os.path.isfile(_filename)
        _file = open(_filename, 'r')
        # File is open.
        # Spill 1 is corrupted.
        spill = "{}"
        output = self.mapper.process(spill)
        self.assertTrue("errors" in maus_cpp.converter.json_repr(output))
        # a real spill
        #spill = _file.readline().rstrip()
        spill = _file.read()
        output = self.mapper.process(spill)
        doc = maus_cpp.converter.json_repr(output)
        tof_event = doc["recon_events"][0]["tof_event"]
        self.assertTrue("tof_digits" in tof_event)
        _file.close()

    @classmethod
    def tear_down_class(cls):
        """___"""
        cls.mapper.death()

if __name__ == '__main__':
    unittest.main()
