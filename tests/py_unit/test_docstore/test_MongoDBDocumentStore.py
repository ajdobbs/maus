"""
Tests for MongoDBDocumentStore module.
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

import pymongo
import unittest

import docstore
from docstore.MongoDBDocumentStore import MongoDBDocumentStore
from test_docstore.test_DocumentStoreBase import DocumentStoreTests

class MongoDBDocumentStoreTestCase(unittest.TestCase, DocumentStoreTests): # pylint: disable=R0904, C0301
    """
    Test class for MongoDBDocumentStore module. Common tests are
    defined in the DocumentStoreTests mix-in.
    """

    def setUp(self):
        """ 
        Create MongoDBDocumentStore with test-specific database.
        @param self Object reference.
        """
        unittest.TestCase.setUp(self)
        DocumentStoreTests.setUp(self)
        self._host = "localhost"
        self._port = 27017
        try:
            test_conx = pymongo.Connection(self._host, self._port)
        # Need ConnectionFailure here for pymongo > 2.4
        except pymongo.errors.ConnectionFailure: # pylint: disable=W0702
            unittest.TestCase.skipTest(self, 
                                       "MongoDB server is not accessible")
        test_conx.disconnect()
        # Create data store and connect.
        self._database_name = self.__class__.__name__
        self._collection = self.__class__.__name__
        self._data_store = MongoDBDocumentStore()

        parameters = {
            "mongodb_host":self._host,
            "mongodb_port":self._port,
            "mongodb_database_name":self._database_name}
        self._data_store.connect(parameters)
        self._data_store.create_collection(self._collection, self._max_size)

    def tearDown(self):
        """
        Delete test-specific database.
        @param self Object reference.
        """
        unittest.TestCase.tearDown(self)
        self._data_store.delete_collection(self._collection)
        self._data_store.disconnect()
        server = pymongo.Connection(self._host, self._port)
        server.drop_database(self._database_name)
        server.disconnect()

    def test_connect_no_parameters(self):
        """
        Test connect with no parameters throws a KeyError as a host
        is expected.
        @param self Object reference.
        """
        try:
            self._data_store.connect({})
        except KeyError:
            pass

    def test_connect_bad_host(self):
        """
        Test connect with a bad host throws an AutoReconnect error as
        expected. 
        @param self Object reference.
        """
        parameters = {"mongodb_host":"nonExistant", "mongodb_port":999999}
        try:
            self._data_store.connect(parameters)
        except docstore.DocumentStore.DocumentStoreException:
            pass

    def test_connect_bad_port(self):
        """
        Test connect with a bad port throws an AutoReconnect error as
        expected. 
        @param self Object reference.
        """
        parameters = {"mongodb_host":self._host, "mongodb_port":999999}
        try:
            self._data_store.connect(parameters)
        except docstore.DocumentStore.DocumentStoreException:
            pass

    def test_connect_no_database_name(self):
        """
        Test connect with no database name parameter throws a KeyError.
        @param self Object reference.
        """
        parameters = {
            "mongodb_host":self._host,
            "mongodb_port":self._port}
        try:
            self._data_store.connect(parameters)
        except KeyError:
            pass

    def test_put_max(self):
        """
        Test put for > max_id.
        @param self Object reference.
        """
        # Insert document.
        doc = {"a":"b"}
        max_size = 0
        # first determine the max number of documents in the docstore
        # (I don't know how storage is done so I can't figure this out)
        self._data_store.put(self._collection, "0", doc)
        while self._data_store.get(self._collection, "0") != None:
            self._data_store.put(self._collection, str(max_size), doc)
            max_size += 1
        count = self._data_store.count(self._collection)
        for i in range(5):
            self._data_store.put(self._collection, str(max_size+i), doc)
            self.assertEquals(self._data_store.count(self._collection), count)

if __name__ == '__main__':
    unittest.main()
