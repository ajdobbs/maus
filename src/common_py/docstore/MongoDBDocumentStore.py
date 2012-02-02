"""
MongoDB-based document store.
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

import pymongo

class MongoDBDocumentStore:
    """
    MongoDB-based document store.
    """

    def __init__(self):
        """ 
        Constructor.
        @param self Object reference.
        """
        self.__mongodb = None
        self.__data_store = None
        
    def connect(self, parameters):
        """ 
        Connect to the data store. The connection is configured
        via the following parameters:

        -mongodb_host - MongoDB host name. This is a mandatory parameter.
        -mongodb_port - MongoDB port. This is a mandatory parameter.
        -mongodb_database_name - MongoDB database name. This is a
         mandatory parameter. If the database does not exist then it
         is created.
        -mongodb_collection_name - MongoDB collection name. This is a
         mandatory parameter. If the collection does not exist then it
         is created.

        @param self Object reference.
        @param parameters Connection information.
        @throws KeyError. If mongodb_host, mongodb_port,
        mongodb_database_name or mongo_collection_name are not
        provided. 
        @throws pymongo.errors.AutoReconnect. If the database cannot
        be contacted using the given host and port.
        """
        self.__mongodb = pymongo.Connection(
            parameters["mongodb_host"],
            parameters["mongodb_port"])
        database = self.__mongodb[parameters["mongodb_database_name"]]
        self.__data_store = database[parameters["mongodb_collection_name"]]

    def __len__(self):
        """ 
        Get number of documents in the data store.
        @param self Object reference.
        @return number >= 0.
        """
        return self.__data_store.count()

    def ids(self):
        """ 
        Get a list of IDs of the documents in the data store.
        @param self Object reference.
        @return ID list.
        """
        return self.__data_store.distinct("_id")

    def put(self, docid, doc):
        """ 
        Put a document with the given ID into the data store. Any existing
        document with the same ID is overwritten.
        @param self Object reference.
        @param docid Document ID.
        @param doc Document.
        """
        self.__data_store.save({'_id':docid, 'doc':doc})

    def get(self, docid):
        """ 
        Get the document with the given ID from the data store or
        None if there is none.
        @param self Object reference.
        @param docid Document ID.
        @return document or None.
        """
        doc = self.__data_store.find_one({"_id":docid})
        if doc != None:
            return doc['doc']
        else:
            return None

    def delete(self, docid):
        """ 
        Delete the document with the given ID from the data store.
        If there is no such document then this is a no-op.
        @param self Object reference.
        @param docid Document ID.
        @param doc Document.
        """
        self.__data_store.remove({"_id":docid})

    def clear(self):
        """ 
        Clear all the documents from the data store.
        @param self Object reference.
        """
        self.__data_store.remove()