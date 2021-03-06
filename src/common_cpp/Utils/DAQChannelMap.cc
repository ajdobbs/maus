/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Utils/DAQChannelMap.hh"
#include "Utils/Squeak.hh"
#include "cabling/cabling.h"

namespace MAUS {

////////////////////////////////////////////////////////////

DAQChannelKey::DAQChannelKey(std::string keyStr) throw(Exceptions::Exception) {
  std::stringstream xConv;
  try {
    xConv << keyStr;
    xConv >> (*this);
  }catch(Exceptions::Exception e) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "DAQChannelKey::DAQChannelKey(std::string)"));
  }
}

bool DAQChannelKey::operator== ( DAQChannelKey const key ) {
  if ( _ldcId == key._ldcId &&
       _geo == key._geo &&
       _channel == key._channel &&
       _eqType == key._eqType &&
       _detector == key._detector) {
    return true;
  } else {
    return false;
  }
}

bool DAQChannelKey::operator!= ( DAQChannelKey const key ) {
  if ( _ldcId == key._ldcId &&
       _geo == key._geo &&
       _channel == key._channel &&
       _eqType == key._eqType &&
       _detector == key._detector) {
    return false;
  } else {
    return true;
  }
}

std::ostream& operator<<( std::ostream& stream, const DAQChannelKey key ) {
  stream << "DAQChannelKey " << key._ldcId;
  stream << " " << key._geo;
  stream << " " << key._channel;
  stream << " " << key._eqType;
  stream << " " << key._detector;
  return stream;
}

std::istream& operator>>( std::istream& stream, DAQChannelKey &key ) throw(Exceptions::Exception) {
  std::string xLabel;
  stream >> xLabel >> key._ldcId >> key._geo >> key._channel >> key._eqType >> key._detector;

  if (xLabel != "DAQChannelKey") {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "istream& operator>>(istream&, DAQChannelKey)"));
  }
  return stream;
}

std::string DAQChannelKey::str() {
  std::stringstream xConv;
  xConv << (*this);
  return xConv.str();
}

////////////////////////////////////////////////////////////

DAQChannelMap::~DAQChannelMap() {
  this->reset();
}

void DAQChannelMap::reset() {
  for (unsigned int i = 0;i < _chKey.size();i++)
    delete _chKey[i];

  _chKey.resize(0);

  cblstr.str("");
  cblstr.clear();
}

DAQChannelMap::DAQChannelMap() {
}

// load channel map based on data cards
bool DAQChannelMap::InitFromCards(Json::Value configJSON) {
  this->reset();
  _daq_devicename = "DAQ";
  std::string _cabling_source = JsonWrapper::GetProperty(configJSON,
                                   "DAQ_cabling_source",
                                   JsonWrapper::stringValue).asString();

  if (_cabling_source != "CDB" && _cabling_source != "file")
      throw(MAUS::Exceptions::Exception(Exceptions::recoverable,
                  std::string("DAQChannelMap::InitFromCards"),
                  "Invalid DAQ_cabling_source card"));

  _daq_cabling_by = JsonWrapper::GetProperty(configJSON,
                                             "DAQ_cabling_by",
                                             JsonWrapper::stringValue).asString();

  if (_daq_cabling_by != "run_number" && _daq_cabling_by != "date")
      throw(MAUS::Exceptions::Exception(Exceptions::recoverable,
                  std::string("DAQChannelMap::InitFromCards"),
                  "Invalid DAQ_cabling_by card"));

  _daq_cablingdate = "current";
  if (_daq_cabling_by == "date")
      _daq_cablingdate = JsonWrapper::GetProperty(configJSON,
                                               "DAQ_cabling_date",
                                               JsonWrapper::stringValue).asString();

  runNumber = -1;
  _datafiles = JsonWrapper::GetProperty(configJSON,
                                        "daq_data_file",
                                        JsonWrapper::stringValue).asString();

  // check if daa_data_file is a number, if it is get run-number from it
  // else parse it and extract the run number
  if (is_number(_datafiles)) {
      runNumber = atoi(_datafiles.c_str());
  } else {
      std::string delimiter = " ";
      std::string token = _datafiles.substr(0, _datafiles.find(delimiter));
      runNumber = atoi(token.c_str());
  }

  bool loaded = false;
//   std::cout << "#### Getting DAQ cabling by " << _cabling_source
//             << "  Run " << runNumber << " ####" << std::endl;

  if (_cabling_source == "CDB") {
//       std::cout << "#### initializing from CDB ####" << std::endl;
      loaded = this->InitFromCDB();
  } else if (_cabling_source == "file") {
      // load the appropriate cabling file depending on runNumber being > or < 6541
      // DR -- this is a hack which should go away once the maps are in the CDB
      std::string map_file_name = "";
      if (runNumber < 6541) {
          assert(configJSON.isMember("DAQ_cabling_file_StepI"));
          map_file_name = configJSON["DAQ_cabling_file_StepI"].asString();
         // load old file
      } else {
          assert(configJSON.isMember("DAQ_cabling_file"));
          map_file_name = configJSON["DAQ_cabling_file"].asString();
      }
//       std::cout << "#### initializing from FILE " << map_file_name << " ####" << std::endl;
      char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
      if (!pMAUS_ROOT_DIR) {
            Squeak::mout(Squeak::error) <<
            "Could not find the $MAUS_ROOT_DIR environmental variable."
            << std::endl;
            Squeak::mout(Squeak::error)
            << "Did you try running: source env.sh ?" << std::endl;
            throw(MAUS::Exceptions::Exception(Exceptions::recoverable,
                        "STRING", "DAQChannelMap::_InitFromCards"));
      }
      // Initialize the map by using text file.
      loaded = InitFromFile(std::string(pMAUS_ROOT_DIR) + map_file_name);
  }
  return loaded;
}

bool DAQChannelMap::InitFromFile(std::string filename) {
  std::ifstream stream(filename.c_str());
  if ( !stream ) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromFile: Can't open DAQ cabling file. "
    << filename << std::endl;
    return false;
  }
  std::stringstream key_s;
  DAQChannelKey* key;
  int lineNum = 0;
  try {
    while ( !stream.eof() ) {
      key = new DAQChannelKey();
      stream >> *key;
      _chKey.push_back(key);
      lineNum++;
    }
  }catch(Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromFile : Error during loading." << std::endl
    << "in file : " << filename << " line " << lineNum
    << e.GetMessage() << std::endl;
    return false;
  }

  if (_chKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromFile : No DAQ Channel Keys loaded. "
    << std::endl;
    return false;
  }
  return true;
}

bool DAQChannelMap::InitFromCurrentCDB() {
  _daq_devicename = "DAQ";
  _daq_cabling_by = "date";
  _daq_cablingdate = "current";
  if (!this->GetCablingCAPI(_daq_devicename))
      return false;
  int lineNum = 0;
  DAQChannelKey* key;
  try {
    while (!cblstr.eof()) {
      key = new DAQChannelKey();
      cblstr >> *key;
      _chKey.push_back(key);
      lineNum++;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromCDB : Error during loading map." << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  if (_chKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromCDB : No DAQ Channel Keys loaded. "
    << std::endl;
    return false;
  }
  return true;
}

bool DAQChannelMap::InitFromCDB() {
  if (!this->GetCablingCAPI(_daq_devicename))
      return false;
  int lineNum = 0;
  DAQChannelKey* key;
  try {
    while (!cblstr.eof()) {
      key = new DAQChannelKey();
      cblstr >> *key;
      _chKey.push_back(key);
      lineNum++;
    }
  } catch (MAUS::Exceptions::Exception e) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromCDB : Error during loading map." << std::endl
    << e.GetMessage() << std::endl;
    return false;
  }

  if (_chKey.size() == 0) {
    Squeak::mout(Squeak::error)
    << "Error in DAQChannelMap::InitFromCDB : No DAQ Channel Keys loaded. "
    << std::endl;
    return false;
  }
  return true;
}

DAQChannelKey* DAQChannelMap::find(int ldc, int geo, int ch, int eqType) {
  // To be optimised !!!
  for (unsigned int i = 0;i < _chKey.size();i++)
    if ( _chKey[i]->ldc() == ldc &&
        _chKey[i]->geo() == geo &&
        _chKey[i]->channel() == ch &&
        _chKey[i]->eqType() == eqType )
      return _chKey[i];

  return NULL;
}

DAQChannelKey* DAQChannelMap::find(std::string daqKeyStr) {
  DAQChannelKey xKey;
  std::stringstream xKeyConv;
  try {
    xKeyConv << daqKeyStr;
    xKeyConv >> xKey;
  }catch(Exceptions::Exception e) {
    throw(Exceptions::Exception(Exceptions::recoverable,
                 std::string("corrupted DAQ Channel Key"),
                 "DAQChannelKey::DAQChannelKey(std::string)"));
  }

  return find(xKey.ldc(), xKey.geo(), xKey.channel(), xKey.eqType());
}

std::string DAQChannelMap::detector(int ldc, int geo, int ch, int eqType) {
  DAQChannelKey* key = find(ldc, geo, ch, eqType);
  if ( key )
    return key->detector();

  return "unknown";
}

bool DAQChannelMap::is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool DAQChannelMap::GetCablingCAPI(std::string devname) {
  // setup the CDB Cabling service
  // default endpoint is the public slave cdb.mice.rl.ac.uk
  // in order to use preprod, do
  //    MAUS::CDB::Cabling cbl("http://preprodcdb.mice.rl.ac.uk")
  MAUS::CDB::Cabling cbl;
  // result holds the string returned by the cdb query
  std::string result;
  try {
      std::string status;
      cbl.getStatus(status);
      if (status.compare("OK") != 0) {
          std::cerr << "+++ CDB Error status = " << status << std::endl;
          return false;
      }
      std::cout << " DAQ Cabling status returned " << status << std::endl;
      if (_daq_cabling_by == "date") {
          std::cout << "+++ Getting DAQcabling by DATE: " << _daq_cablingdate.c_str() << std::endl;
          if (_daq_cablingdate.compare("current") == 0) {
              cbl.getCurrentDetectorCabling(devname.c_str(),
                                            result);
          } else {
              cbl.getDetectorCablingForDate(devname.c_str(),
                                            _daq_cablingdate.c_str(),
                                            result);
          }
      } else if (_daq_cabling_by == "run_number") {
          std::cout << "+++ Getting DAQcabling by RUN# " << runNumber << std::endl;
          cbl.getDetectorCablingForRun(devname.c_str(),
                                       runNumber,
                                       result);
      }
      // std::cout << result << "(" << result.size() << " characters)" << std::endl;
  } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      return false;
  }
  // store the result in a stringstream so it can be parsed by the InitFromCDB function
  cblstr.str(result);
  return true;
}

}  // namespace MAUS
