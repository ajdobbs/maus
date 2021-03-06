"""
Tests for workers module.
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

# pylint: disable=C0103

import unittest

from framework.workers import WorkerBirthFailedException
from framework.workers import WorkerDeathFailedException
from framework.workers import WorkerDeadException
from framework.workers import WorkerProcessException
from framework.workers import WorkerUtilities
from MAUS import MapPyDoNothing
from MAUS import MapPyGroup

class WorkerUtilitiesTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.workers.WorkerUtilities.
    """

    def test_get_worker_names_none(self):
        """
        Test get_worker_names(None).
        @param self Object reference.
        """
        name = WorkerUtilities.get_worker_names(None)
        self.assertEquals(None.__class__.__name__, name, "Unexpected name")

    def test_get_worker_names(self):
        """
        Test get_worker_names(worker) where worker has no sub-workers. 
        @param self Object reference.
        """ 
        name = WorkerUtilities.get_worker_names(MapPyDoNothing())
        self.assertEquals(MapPyDoNothing.__name__,
                          name, 
                          "Unexpected name")
    
    def test_get_worker_names_no_sub_workers(self):
        """
        Test get_worker_names(worker) where worker is a MapPyGroup that
        has no sub-workers.
        @param self Object reference.
        """ 
        names = WorkerUtilities.get_worker_names(MapPyGroup())
        self.assertEquals([], names, "Unexpected names")
 
    def test_get_worker_names_sub_workers(self):
        """
        Test get_worker_names(worker) where worker is a MapPyGroup that
        has sub-workers.
        @param self Object reference.
        """ 
        sub_worker = MapPyGroup()
        sub_worker.append(MapPyDoNothing())
        sub_worker.append(MapPyDoNothing())
        worker = MapPyGroup()
        worker.append(MapPyDoNothing())
        worker.append(sub_worker)
        names = WorkerUtilities.get_worker_names(worker)
        self.assertEquals([MapPyDoNothing.__name__,
                           [MapPyDoNothing.__name__,
                            MapPyDoNothing.__name__]],
                          names, 
                          "Unexpected names")

    def test_create_transform_none(self):
        """
        Test get_create_transform(None) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*Transform name None is not a string.*"):
            WorkerUtilities.create_transform(None)

    def test_create_transform_none_list(self):
        """
        Test get_create_transform([None]) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*Transform name None is not a string.*"):
            WorkerUtilities.create_transform([None])

    def test_create_transform_integer(self):
        """
        Test get_create_transform(123) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*Transform name 123 is not a string.*"):
            WorkerUtilities.create_transform(123)

    def test_create_transform_integer_list(self):
        """
        Test get_create_transform([123]) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*Transform name 123 is not a string.*"):
            WorkerUtilities.create_transform([123])

    def test_create_transform_unknown(self):
        """
        Test get_create_transform("UnknownTransform") throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*No such transform: UnknownTransform.*"):
            WorkerUtilities.create_transform("UnknownTransform")

    def test_create_transform_unknown_list(self):
        """
        Test get_create_transform(["UnknownTransform"]) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*No such transform: UnknownTransform.*"):
            WorkerUtilities.create_transform(["UnknownTransform"])

    def test_create_transform(self):
        """
        Test get_create_transform("MapPyDoNothing") returns MapPyDoNothing. 
        @param self Object reference.
        """ 
        transform = \
            WorkerUtilities.create_transform(MapPyDoNothing.__name__)
        self.assertTrue(isinstance(transform, MapPyDoNothing), 
                        "Transform is not a %s" % MapPyDoNothing)

    def test_create_transform_unicode(self):
        """
        Test get_create_transform(u"MapPyDoNothing") returns MapPyDoNothing. 
        @param self Object reference.
        """ 
        transform = \
            WorkerUtilities.create_transform( \
                unicode(MapPyDoNothing.__name__))
        self.assertTrue(isinstance(transform, MapPyDoNothing), 
                        "Transform is not a %s" % MapPyDoNothing)

    def test_create_transform_empty_list(self):
        """
        Test get_create_transform([]) returns an empty MapPyGroup.
        @param self Object reference.
        """ 
        transform = WorkerUtilities.create_transform([])
        self.assertTrue(isinstance(transform, MapPyGroup), 
                        "Transform is not a %s" % MapPyGroup)
        self.assertEquals([], transform.get_worker_names(), 
                          "Transform contains unexpected workers")

    def test_create_transform_group(self):
        """
        Test get_create_transform, when given a list of transform names
        returns a MapPyGroup with these transforms.
        @param self Object reference.
        """ 
        names = [MapPyDoNothing.__name__, 
                 [MapPyDoNothing.__name__, MapPyDoNothing.__name__]]
        transform = WorkerUtilities.create_transform(names)
        self.assertTrue(isinstance(transform, MapPyGroup), 
                        "Transform is not a %s" % MapPyGroup)
        self.assertEquals(names, transform.get_worker_names(), 
                          "Transform contains unexpected workers")

    def test_validate_transform_none(self):
        """
        Test get_validate_transform(None) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*Transform name None is not a string.*"):
            WorkerUtilities.validate_transform(None)

    def test_validate_transform_none_list(self):
        """
        Test get_validate_transform([None]) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*Transform name None is not a string.*"):
            WorkerUtilities.validate_transform([None])

    def test_validate_transform_integer(self):
        """
        Test get_validate_transform(123) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*Transform name 123 is not a string.*"):
            WorkerUtilities.validate_transform(123)

    def test_validate_transform_integer_list(self):
        """
        Test get_validate_transform([123]) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*Transform name 123 is not a string.*"):
            WorkerUtilities.validate_transform([123])

    def test_validate_transform_unknown(self):
        """
        Test get_validate_transform("UnknownTransform") throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*No such transform: UnknownTransform.*"):
            WorkerUtilities.validate_transform("UnknownTransform")

    def test_validate_transform_unknown_list(self):
        """
        Test get_validate_transform(["UnknownTransform"]) throws an error.
        @param self Object reference.
        """ 
        with self.assertRaisesRegexp(ValueError,
            ".*No such transform: UnknownTransform.*"):
            WorkerUtilities.validate_transform(["UnknownTransform"])

    def test_validate_transform(self):
        """
        Test get_validate_transform("MapPyDoNothing") is OK.
        @param self Object reference.
        """ 
        WorkerUtilities.validate_transform(MapPyDoNothing.__name__)
        self.assertTrue("validate_transform(name) passed")

    def test_validate_transform_unicode(self):
        """
        Test get_validate_transform(u"MapPyDoNothing") is OK.
        @param self Object reference.
        """ 
        WorkerUtilities.validate_transform(unicode(MapPyDoNothing.__name__))
        self.assertTrue("validate_transform(unicode) passed")

    def test_validate_transform_empty_list(self):
        """
        Test get_validate_transform([]) is OK.
        @param self Object reference.
        """ 
        WorkerUtilities.validate_transform([])
        self.assertTrue("validate_transform([]) passed")

    def test_validate_transform_group(self):
        """
        Test get_validate_transform, when given a list of transform names
        is OK.
        @param self Object reference.
        """ 
        names = [MapPyDoNothing.__name__, 
                 [MapPyDoNothing.__name__, MapPyDoNothing.__name__]]
        WorkerUtilities.validate_transform(names)
        self.assertTrue("validate_transform(names) passed")

class WorkerBirthFailedExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.workers.WorkerBirthFailedException class.
    """

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        exception = WorkerBirthFailedException("TASK")
        self.assertEquals("TASK returned False", str(exception), \
            "Unexpected string")

class WorkerDeathFailedExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.workers.WorkerDeathFailedException class.
    """

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        exception = WorkerDeathFailedException("TASK")
        self.assertEquals("TASK returned False", str(exception), \
            "Unexpected string")

class WorkerDeadExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.workers.WorkerDeadException class.
    """

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        exception = WorkerDeadException("TASK")
        self.assertEquals("TASK process called after death", \
            str(exception), "Unexpected string")

class WorkerProcessExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for framework.workers.WorkerProcessException class.
    """

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        exception = WorkerProcessException("TASK")
        self.assertEquals("TASK process threw an exception", \
            str(exception), "Unexpected string")

    def test_init_details(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        exception = WorkerProcessException("TASK", "DETAILS")
        self.assertEquals("TASK process threw an exception: DETAILS", \
            str(exception), "Unexpected string")

if __name__ == '__main__':
    unittest.main()
