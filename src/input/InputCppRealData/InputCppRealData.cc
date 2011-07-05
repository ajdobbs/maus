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

#include "src/input/InputCppRealData/InputCppRealData.hh"

InputCppRealData::InputCppRealData(std::string pDataPath,
                                   std::string pRunNum) {
  _debug = false;
  _eventPtr = NULL;
  _inputFile = NULL;
  _dataPath = pDataPath;
  _runNum = pRunNum;

  _v1290PartEventProc = NULL;
  _v1724PartEventProc = NULL;
  _v1731PartEventProc = NULL;
  _v830FragmentProc = NULL;
	_vLSBFragmentProc = NULL;
  _DBBFragmentProc = NULL;
}


bool InputCppRealData::birth(std::string jsonDataCards) {
  unsigned int nfiles = _dataFileManager.AddRun( _runNum.c_str(), _dataPath.c_str() );
  if(!nfiles){
    std::cerr<<"Unable to load any data files. Check your run number and data path."<<std::endl;
	  exit(0);
  }

  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

	  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(jsonDataCards, configJSON);
  if (!parsingSuccessful) {
    return false;
  }

  assert( configJSON.isMember("DAQ_cabling_file") );
	string map_file_name = configJSON["DAQ_cabling_file"].asString();
  char* pMAUS_ROOT_DIR = getenv( "MAUS_ROOT_DIR" );
  map.InitFromFile( string( pMAUS_ROOT_DIR ) + map_file_name );

  assert(configJSON.isMember("Enable_V1290_Unpacking"));
  if( configJSON["Enable_V1290_Unpacking"].asBool() ){
    _v1290PartEventProc = new V1290DataProcessor();
    _v1290PartEventProc->set_DAQ_map( &map );
    _dataProcessManager.SetPartEventProc("V1290",_v1290PartEventProc);
  }
  else  this->disableEquipment("V1290");

  assert(configJSON.isMember("Enable_V1724_Unpacking"));
  if( configJSON["Enable_V1724_Unpacking"].asBool() ){
    _v1724PartEventProc = new V1724DataProcessor();
    _v1724PartEventProc->set_DAQ_map( &map );
    bool zs = configJSON["Do_V1724_Zero_Suppression"].asBool();
    _v1724PartEventProc->set_zero_supression( zs );
    _dataProcessManager.SetPartEventProc("V1724",_v1724PartEventProc);
  }
  else  this->disableEquipment("V1724");

  assert(configJSON.isMember("Enable_V1731_Unpacking"));
  if( configJSON["Enable_V1731_Unpacking"].asBool() ){
    _v1731PartEventProc = new V1731DataProcessor();
    _v1731PartEventProc->set_DAQ_map( &map );
    bool zs = configJSON["Do_V1731_Zero_Suppression"].asBool();
    _v1731PartEventProc->set_zero_supression( zs );
    _dataProcessManager.SetPartEventProc("V1731",_v1731PartEventProc);
  }
  else  this->disableEquipment("V1731");

  assert(configJSON.isMember("Enable_V830_Unpacking"));
  if( configJSON["Enable_V830_Unpacking"].asBool() ){
    _v830FragmentProc = new V830DataProcessor();
    _v830FragmentProc->set_DAQ_map( &map );
    _dataProcessManager.SetFragmentProc("V830",_v830FragmentProc);
  }
  else  this->disableEquipment("V830");

  assert(configJSON.isMember("Enable_VLSB_Unpacking"));
  if( configJSON["Enable_VLSB_Unpacking"].asBool() ){
    _vLSBFragmentProc = new VLSBDataProcessor();
    _vLSBFragmentProc->set_DAQ_map( &map );
    _dataProcessManager.SetFragmentProc("VLSB_C",_vLSBFragmentProc);
  }
  else  this->disableEquipment("VLSB_C");

  assert(configJSON.isMember("Enable_DBB_Unpacking"));
  if( configJSON["Enable_DBB_Unpacking"].asBool() ){
    _DBBFragmentProc = new DBBDataProcessor();
    _DBBFragmentProc->set_DAQ_map( &map );
    _dataProcessManager.SetFragmentProc("DBB",_DBBFragmentProc);
  }
  else  this->disableEquipment("DBB");

  //_eventPtr = _dataFileManager.GetNextEvent();

  if (_debug)
    std::cerr << nfiles << "files loaded for Run " << _runNum << " ("
              << _dataPath << ")" << std::endl;

	_dataProcessManager.DumpProcessors();

  return true;
}


bool InputCppRealData::readNextEvent() {
	_eventPtr = _dataFileManager.GetNextEvent();
  if (!_eventPtr)
    return false;

  return true;
}


std::string InputCppRealData::getCurEvent() {
	Json::Value xDocRoot;  // Root of the event
  Json::FastWriter xJSONWr;
  Json::Value xDocSpill;

  if( _v1290PartEventProc )
	  _v1290PartEventProc->set_JSON_doc( &xDocSpill );

	if( _v1724PartEventProc )
	  _v1724PartEventProc->set_JSON_doc( &xDocSpill );

  if( _v1731PartEventProc )
    _v1731PartEventProc->set_JSON_doc( &xDocSpill );

  if( _v830FragmentProc )
    _v830FragmentProc->set_JSON_doc( &xDocSpill );

  if( _vLSBFragmentProc )
    _vLSBFragmentProc->set_JSON_doc( &xDocSpill );

  if( _DBBFragmentProc )
    _DBBFragmentProc->set_JSON_doc( &xDocSpill );

  try {
    _dataProcessManager.Process(_eventPtr);
  }
  // Deal with exceptions
  catch ( MDexception & lExc) {
    cerr << "Unpacking exception,  DAQ Event skipped" << std::endl;
    cerr <<  lExc.GetDescription() << endl;
  }
  catch(std::exception & lExc) {
    cerr << "Standard exception" << std::endl;
    cerr << lExc.what() << std::endl;
  }
  catch(...) {
    cerr << "Unknown exception occurred..." << std::endl;
  }

  // Finally attach the spill to the document root
  xDocRoot["daq_data"] = xDocSpill;
  xDocRoot["spill_num"] = _dataProcessManager.GetSpillNumber();
	xDocRoot["daq_event_type"] = event_type_to_str( _dataProcessManager.GetEventType() );
	//cout<<xDocRoot<<endl;
  if (_debug)
    std::cerr << "Writing JSON..." << std::endl;
  return xJSONWr.write(xDocRoot);

}

bool InputCppRealData::death()
{
  if(_v1290PartEventProc) delete _v1290PartEventProc;
  if(_v1724PartEventProc) delete _v1724PartEventProc;
  if(_v1731PartEventProc) delete _v1731PartEventProc;
  if(_v830FragmentProc) delete _v830FragmentProc;
  if(_vLSBFragmentProc) delete _vLSBFragmentProc;
  if(_DBBFragmentProc) delete _DBBFragmentProc;

  return true;
}

string InputCppRealData::event_type_to_str(int pType)
{
  string event_type;
  switch(pType){
		case START_OF_BURST :
			event_type = "start_of_burst";
			break;

		case  END_OF_BURST:
      event_type = "end_of_burst";
      break;

		case PHYSICS_EVENT :
			event_type = "physics_event";
			break;

		case CALIBRATION_EVENT :
      event_type = "calibration_event";
      break;

		case START_OF_RUN :
			event_type = "start_of_run";
			break;

		case  END_OF_RUN:
      event_type = "end_of_run";
      break;

		default :
			stringstream xConv;
			xConv<<pType<<" (unknown)";
      event_type = xConv.str();
      break;
	}
  return event_type;
}







