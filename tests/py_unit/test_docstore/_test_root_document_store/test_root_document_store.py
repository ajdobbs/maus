#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

# ROOT crap
# pylint: disable = E1101

"""
Tests ROOT document store and document queue
"""

import unittest
import sys

import ROOT
import libMausCpp #pylint: disable=W0611

from docstore.DocumentStore import DocumentStoreException
from docstore.root_document_store import RootDocumentDB
from docstore.root_document_store import RootDocumentStore

from test_docstore.test_DocumentStoreBase import DocumentStoreTests

def ttree_data(spill_number):
    """
    Generate a data tree for testing
    """
    spill = ROOT.MAUS.Spill()
    data = ROOT.MAUS.Data()
    tree = ROOT.TTree("Spill", "TTree")
    tree.Branch("data", data, data.GetSizeOf(), 1)
    spill.SetScalars(ROOT.MAUS.Scalars())
    spill.SetDAQData(ROOT.MAUS.DAQData())
    spill.SetMCEvents(ROOT.MAUS.MCEventPArray())
    spill.SetReconEvents(ROOT.MAUS.ReconEventPArray())
    # test branch makes segmentation fault... from ROOT side
    # spill.SetTestBranch(ROOT.MAUS.TestBranch())
    spill.SetSpillNumber(spill_number)
    spill.SetRunNumber(10)
    data.SetSpill(spill)
    tree.Fill()
    return tree

def get_spill_number(data_tree):
    """
    Check the spill number in the data tree
    """
    data = ROOT.MAUS.Data() # pylint: disable = E1101
    data_tree.SetBranchAddress("data", data)
    data_tree.GetEntry()
    return data.GetSpill().GetSpillNumber()

PORT = 48110

class ROOTDBDocumentStoreTestCase(unittest.TestCase, DocumentStoreTests): # pylint: disable=R0904, C0301
    """
    Test class for RootDocumentStore class. Common tests are
    defined in the DocumentStoreTests mix-in.
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Create ROOTDBDocumentStore with test-specific database.
        @param self Object reference.
        """
        unittest.TestCase.setUp(self)
        DocumentStoreTests.setUp(self)
        self._max_size = 5
        self._host = "localhost"
        self._port = PORT
        self._root_db = RootDocumentDB([self._port, self._port+1], 0.1)
        self._data_store = RootDocumentStore(10., 0.1)
        self._collection = self.__class__.__name__
        parameters = {
            "host":self._host,
            "port":self._port}
        self._data_store.connect(parameters)
        self._data_store.create_collection(self._collection, self._max_size)

    def tearDown(self): # pylint: disable=C0103
        """
        Delete test-specific database.
        @param self Object reference.
        """
        global PORT #pylint: disable=W0603
        unittest.TestCase.tearDown(self)
        self._data_store.disconnect()
        PORT += 5

    def test_put_get_root_documents(self):
        """
        Check that we can put and get ROOT documents (json documents are tested
        elsewhere)
        """
        rds = self._data_store
        rds.create_collection("test_collection", 5)
        rds.put("test_collection", 0, ttree_data(77))
        data_out = rds.get("test_collection", 0)
        rds.put("test_collection", 0, ttree_data(78))
        data_out = rds.get("test_collection", 0) # check we overwrite
        self.assertEqual(rds.count("test_collection"), 1)
        rds.put("test_collection", 11, {"test":"document"}) # try json document
        data_out = rds.get("test_collection", 11)
        self.assertEqual(data_out, {"test":"document"})

        try: # put to a non existent collection
            rds.put("no_collection", 0, ttree_data(78))
            self.assertTrue(False, msg="Should have thrown")
        except DocumentStoreException:
            pass
        try: # put an int, not a ROOT document
            rds.put("test_collection", 0, 99)
            self.assertTrue(False, msg="Should have thrown")
        except DocumentStoreException:
            pass
        for i in range(90, 80, -1): # check we can only write 5 items
            rds.put("test_collection", i, ttree_data(i))
        self.assertEqual(rds.count("test_collection"), 5)
        data_out = rds.get("test_collection", 81) # check we keep new docs
        self.assertEqual(get_spill_number(data_out), 81)
        data_out = rds.get("test_collection", 90) # check we push old docs out
        self.assertEqual(data_out, None)

    def test_disconnect_reconnect(self):
        """
        Check that we can disconnect from the DB
        """
        data_in = {"test":"document"}
        self._data_store.disconnect()
        try:
            self._data_store.put(self._collection, 0, data_in)
        except DocumentStoreException:
            sys.excepthook(*sys.exc_info())
        self._data_store.connect({"host":self._host, "port":self._port})
        self._data_store.put(self._collection, 0, data_in)
        data_out = self._data_store.get(self._collection, 0)
        self.assertEqual(data_in, data_out)

if __name__ == "__main__":
    unittest.main()
