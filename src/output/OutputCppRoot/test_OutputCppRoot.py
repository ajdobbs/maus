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
Test for OutputCppRoot
"""

import unittest
import json
import OutputCppRoot

class TestOutputCppRoot(unittest.TestCase):
    """
    Test we can write out ROOT tree.

    Check we can birth and death correctly; check we can write simple ROOT trees
    especially in context of - how are errors handled - 
    """

    def setUp(self): # pylint: disable=C0103, C0202
        """
        Define cards and initialise Output
        """
        self.output = OutputCppRoot.OutputCppRoot()
        self.cards = json.dumps({"root_output_filename":"maus_output.root"})
        self.output.birth(self.cards)

    def test_birth_death(self):
        """
        Check that we can birth and death properly
        """
        an_output = OutputCppRoot.OutputCppRoot()
        self.assertFalse(an_output.birth('{}'))
        self.assertFalse(an_output.birth('{"root_output_filename":""}'))
        self.assertTrue(an_output.birth(self.cards))
        # makes a segv if I don't call death()
        self.assertTrue(an_output.death())

    def test_save_normal_event(self):
        """
        Try saving a few standard events
        """
        self.assertTrue(self.output.save(
                      json.dumps({"mc":[], "digits":[]})
        ))

    def test_save_bad_event(self):
        """
        Check that if passed a bad event, code fails gracefully
        """
        self.assertFalse(True)
        self.assertFalse(self.output.save(
                      json.dumps({"no_branch":{}})
        ))
        print 0
        self.assertFalse(self.output.save(''))
        print 3
        my_output.death()
  

if __name__=="__main__":
    unittest.main()


