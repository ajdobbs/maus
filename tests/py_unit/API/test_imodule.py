"""
This file defines the tests for IModule
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

import unittest
from API.IModule import IModule

class TestIModule(unittest.TestCase):#pylint: disable=R0904
    """
    @class TestIModule
    Unit testing class for IModule
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            mmm = IModule()
            self.assertIsInstance(mmm, IModule, 'Not an instance of IModule')
            self.assertIsInstance(mmm, object, 'Not an instance of object')
        except: #pylint: disable=W0702
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_birth(self):
        """Test birth"""
        mmm = IModule()
        self.assertRaises(NotImplementedError, mmm.birth, 'config')

    def test_death(self):
        """Test death"""
        mmm = IModule()
        self.assertRaises(NotImplementedError, mmm.death)

if __name__ == '__main__':
    unittest.main()
