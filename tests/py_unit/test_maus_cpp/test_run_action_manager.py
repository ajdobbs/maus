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
Test maus_cpp.run_action_manager
"""

import StringIO
import unittest
import json

import Configuration

import maus_cpp.globals
import maus_cpp.run_action_manager

class GlobalsTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.run_action_manager"""

    def setUp(self): # pylint: disable=C0103
        """Set up test"""
        self.test_config = ""
        if maus_cpp.globals.has_instance():
            self.test_config = maus_cpp.globals.get_configuration_cards()
        config_options = StringIO.StringIO(unicode("""
simulation_geometry_filename = "Test.dat"
reconstruction_geometry_filename = "Test.dat"
        """))
        self.config = Configuration.Configuration().getConfigJSON(
                                                         config_options, False)

    def tearDown(self): # pylint: disable = C0103
        """Reset the globals"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        if self.test_config != "":
            maus_cpp.globals.birth(self.test_config)

    def test_start_end_of_run(self):
        """Test maus_cpp.run_action_manager.start_of_run(...)"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        maus_cpp.globals.birth(self.config)
        run_header = json.loads(maus_cpp.run_action_manager.start_of_run(1))
        self.assertEqual(run_header["maus_event_type"], "RunHeader")
        run_footer = json.loads(maus_cpp.run_action_manager.end_of_run(1))
        self.assertEqual(run_footer["maus_event_type"], "RunFooter")
        
if __name__ == "__main__":
    unittest.main()

