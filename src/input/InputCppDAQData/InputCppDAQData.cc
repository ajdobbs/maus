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

#include "src/input/InputCppDAQData/InputCppDAQData.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/API/PyWrapInputBase.hh"

namespace MAUS {
PyMODINIT_FUNC init_InputCppDAQData(void) {
    PyWrapInputBase<MAUS::InputCppDAQData>::PyWrapInputBaseModInit(
                  "InputCppDAQData", "", "", "", "");
}

InputCppDAQData::InputCppDAQData() : InputBase<MAUS::Data>("InputCppDAQData") {
  _eventPtr = NULL;
  _eventsCount = 0;

  _v1290PartEventProc_cpp  = NULL;
  _v1724PartEventProc_cpp  = NULL;
  _v1731PartEventProc_cpp  = NULL;
  _v830FragmentProc_cpp    = NULL;
  _vLSBFragmentProc_cpp   = NULL;
  _DBBFragmentProc_cpp     = NULL;
  _DBBChainFragmentProc_cpp = NULL;
  _TriggerEngineFragmentProc_cpp = NULL;
  _EpicsInterfaceFragmentProc_cpp = NULL;
}


void InputCppDAQData::_childbirth(const std::string& jsonDataCards) {
  if ( _dataFileManager.GetNFiles() ) {
     throw(MAUS::Exceptions::Exception(Exceptions::recoverable, "STRING",
                                      "InputCppDAQData::_childbirth"));
  }

  //  JsonCpp setup
  Json::Value configJSON;   //  this will contain the configuration
  Json::Reader reader;

  // Check if the JSON document can be parsed, else return error only
  bool parsingSuccessful = reader.parse(jsonDataCards, configJSON);
  if (!parsingSuccessful) {
    throw(MAUS::Exceptions::Exception(Exceptions::recoverable, "STRING",
                                      "InputCppDAQData::_childbirth"));
  }

  // Comfigure the V830 (scaler) data processor.
  initProcessor(_v830FragmentProc_cpp, configJSON);

  // Comfigure the V1290 (TDC) data processor.
  initProcessor(_v1290PartEventProc_cpp,  configJSON);

  // Comfigure the V1724 (TOF and KL fADC) data processor.
  initProcessor(_v1724PartEventProc_cpp, configJSON);
  configureZeroSupression(_v1724PartEventProc_cpp, configJSON);

  // Comfigure the V1731 (CKOV and EMR fADC) data processor.
  initProcessor(_v1731PartEventProc_cpp, configJSON);
  configureZeroSupression(_v1731PartEventProc_cpp, configJSON);

  // Comfigure the VLSB (tracker board) data processor.
  initProcessor(_vLSBFragmentProc_cpp, configJSON);
  configureZeroSupressionTK(_vLSBFragmentProc_cpp, configJSON);

  // Comfigure the DBB (EMR board) data processor.
  initProcessor(_DBBFragmentProc_cpp, configJSON);

  // Comfigure the DBB Chain (chain of 6 EMR boards) data processor.
  initProcessor(_DBBChainFragmentProc_cpp, configJSON);

  // Comfigure the _Trigger Engine data processor.
  initProcessor(_TriggerEngineFragmentProc_cpp, configJSON);

  // Comfigure the _Epics Interface data processor.
  initProcessor(_EpicsInterfaceFragmentProc_cpp, configJSON);

  // frankliuao: moved the _map.InitFromCards(configJSON) to InputCppDAQOnlineData
  // and InputCppDAQOfflineData. There, daq_cabling_source is forced to be "CDB"

  // Set the map (a static data member) of all the processors.
  MDarranger::set_DAQ_map(&_map);
}

bool InputCppDAQData::readNextEvent() {

  Squeak::mout(Squeak::error)
  << "ERROR : InputCppDAQData is a base imput class and can not be used to access the DAQ data!"
  << std::endl << "*** Use InputCppDAQOfflineData or InputCppDAQOnlineData instead. ***"
  << std::endl << std::endl;

  return false;
}

int InputCppDAQData::getCurEvent(MAUS::Data *data) {
  MAUS::Spill* spill = data->GetSpill();
  int nPartEvts(0);
  try {
    // Do the loop over the binary DAQ data.
    nPartEvts = _dataProcessManager.Process(_eventPtr);
    // The data is now processed and is ready to be filled.
    unsigned int event_type = _dataProcessManager.GetEventType();
    spill->SetDaqEventType(event_type_to_str(event_type));
    spill->SetRunNumber(_dataProcessManager.GetRunNumber());
    spill->SetSpillNumber(_dataProcessManager.GetSpillNumber());
    spill->SetEventId(_dataProcessManager.GetEventId());
    spill->SetTimeStamp(_dataProcessManager.GetTimeStamp());

    if (event_type == PHYSICS_EVENT) {
      // Create a new DAQData object.
      MAUS::DAQData *daq_data = new MAUS::DAQData;
      daq_data->SetEventSize(_dataProcessManager.GetSuperEventSize());
      // Set the DAQData object (a static data member) of all the processors.
      MDarranger::set_daq_data(daq_data);

      // Now fill the DAQData object with the data processed by all the processors.
      if (_DBBFragmentProc_cpp)
        _DBBFragmentProc_cpp->fill_daq_data();

      if (_DBBChainFragmentProc_cpp)
        _DBBChainFragmentProc_cpp->fill_daq_data();

      if (_v1731PartEventProc_cpp)
        _v1731PartEventProc_cpp->fill_daq_data();

      if (_v1724PartEventProc_cpp)
        _v1724PartEventProc_cpp->fill_daq_data();

      if (_v1290PartEventProc_cpp)
        _v1290PartEventProc_cpp->fill_daq_data();

      if (_v830FragmentProc_cpp)
        _v830FragmentProc_cpp->fill_daq_data();

      if (_vLSBFragmentProc_cpp)
        _vLSBFragmentProc_cpp->fill_daq_data();

      if (_TriggerEngineFragmentProc_cpp)
        _TriggerEngineFragmentProc_cpp->fill_daq_data();

      if (_EpicsInterfaceFragmentProc_cpp)
        _EpicsInterfaceFragmentProc_cpp->fill_daq_data();

      // Set the DAQData object of the spill.
      spill->SetDAQData(daq_data);
    }
  }
  // Deal with exceptions
  catch (MDexception & lExc) {
    Squeak::mout(Squeak::error) << lExc.GetDescription() << std::endl
    << "*** Unpacking exception in void  "
    << "InputCppDAQData::getCurEvent(MAUS::Data *data) : " << std::endl;
    Squeak::mout(Squeak::error) <<"DAQ Event skipped!" << std::endl << std::endl;

    std::stringstream ss;
    ss << _classname << " says:" << lExc.GetDescription() << "  Phys. Event " << std::endl
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    MAUS::ErrorsMap errors = spill->GetErrors();
    errors["bad_data_input"] = ss.str();
    spill->SetErrors(errors);

    nPartEvts = 0;
  }
  catch (Exceptions::Exception exc) {
    Squeak::mout(Squeak::error) << exc.GetLocation() << ": "
    << exc.GetMessage() << std::endl
    << "*** MAUS exception in "
    << "InputCppDAQData::getCurEvent(MAUS::Data *data) : " << std::endl;
    Squeak::mout(Squeak::error) <<"DAQ Event skipped!" << std::endl << std::endl;

    std::stringstream ss;
    ss << exc.GetLocation() << " says:" << exc.GetMessage() << "  Phys. Event " << std::endl
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    MAUS::ErrorsMap errors = spill->GetErrors();
    errors["bad_data_input"] = ss.str();
    spill->SetErrors(errors);
  }
  catch (std::exception & lExc) {
    Squeak::mout(Squeak::error) << lExc.what() << std::endl
    << "*** Standard exception in "
    << "InputCppDAQData::getCurEvent(MAUS::Data *data) : " << std::endl;
    Squeak::mout(Squeak::error) <<"DAQ Event skipped!" << std::endl << std::endl;

    std::stringstream ss;
    ss << _classname << " says:" << lExc.what() << " Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    MAUS::ErrorsMap errors = spill->GetErrors();
    errors["bad_data_input"] = ss.str();
    spill->SetErrors(errors);
  }
  catch (...) {
    Squeak::mout(Squeak::error) << "*** InputCppDAQData::getCurEvent(MAUS::Data *data) : "
    << "Unknown exception occurred." << std::endl;
    Squeak::mout(Squeak::error) << "DAQ Event skipped!" << std::endl << std::endl;

    std::stringstream ss;
    ss << _classname << " says: Unknown exception occurred. Phys. Event "
    << _dataProcessManager.GetPhysEventNumber() << " skipped!";
    MAUS::ErrorsMap errors = spill->GetErrors();
    errors["bad_data_input"] = ss.str();
    spill->SetErrors(errors);
  }
  this->resetAllProcessors();

  return nPartEvts;
}

std::string InputCppDAQData::getCurEvent() {

  MAUS::Data  *data_cpp  = new MAUS::Data;
  MAUS::Spill *spill_cpp = new MAUS::Spill;
  data_cpp->SetSpill(spill_cpp);
  this->getCurEvent(data_cpp);

  Json::Value* spill_json_out = MAUS::CppJsonSpillConverter().convert(data_cpp);
//   std::cerr << *spill_json_out << std::endl;
  _eventsCount++;

  Json::FastWriter xJSONWr;
  std::string output = xJSONWr.write(*spill_json_out);

  delete spill_json_out;
  delete data_cpp;

  return output;
}

void InputCppDAQData::_death() {
  // Free the memory.
  if (_v1290PartEventProc_cpp)        delete _v1290PartEventProc_cpp;
  if (_v1724PartEventProc_cpp)        delete _v1724PartEventProc_cpp;
  if (_v1731PartEventProc_cpp)        delete _v1731PartEventProc_cpp;
  if (_v830FragmentProc_cpp)          delete _v830FragmentProc_cpp;
  if (_vLSBFragmentProc_cpp)          delete _vLSBFragmentProc_cpp;
  if (_DBBFragmentProc_cpp)           delete _DBBFragmentProc_cpp;
  if (_DBBChainFragmentProc_cpp)      delete _DBBChainFragmentProc_cpp;
  if (_TriggerEngineFragmentProc_cpp) delete _TriggerEngineFragmentProc_cpp;
  if (_EpicsInterfaceFragmentProc_cpp) delete _EpicsInterfaceFragmentProc_cpp;
}

void InputCppDAQData::resetAllProcessors() {
  // Reset all the processors.
  if (_v1290PartEventProc_cpp)        _v1290PartEventProc_cpp->reset();
  if (_v1724PartEventProc_cpp)        _v1724PartEventProc_cpp->reset();
  if (_v1731PartEventProc_cpp)        _v1731PartEventProc_cpp->reset();
  if (_v830FragmentProc_cpp)          _v830FragmentProc_cpp->reset();
  if (_vLSBFragmentProc_cpp)          _vLSBFragmentProc_cpp->reset();
  if (_DBBFragmentProc_cpp)           _DBBFragmentProc_cpp->reset();
  if (_DBBChainFragmentProc_cpp)      _DBBChainFragmentProc_cpp->reset();
  if (_TriggerEngineFragmentProc_cpp) _TriggerEngineFragmentProc_cpp->reset();
  if (_EpicsInterfaceFragmentProc_cpp) _EpicsInterfaceFragmentProc_cpp->reset();
}

template <class procType>
bool InputCppDAQData::initProcessor(procType* &processor, Json::Value configJSON) {
  processor = new procType();
  string xName, xDataCard;
  xName = processor->get_equipment_name();
  xDataCard = "Enable_" + xName + "_Unpacking";

  // Enable or disable this equipment.
  assert(configJSON.isMember(xDataCard));
  bool enableThis = configJSON[xDataCard].asBool();

  if (enableThis) {
    // Get a pointer to the equipment fragment object from the static equipment map.
    unsigned int xFragType = MDequipMap::GetType(xName);
    MDfragment* xFragPtr = MDequipMap::GetFragmentPtr(xFragType);

    // Check is the data from this equipment is made of particle events.
    try {
      if (xFragPtr->IsMadeOfParticles()) {
        // Set a processor for particle events.
        _dataProcessManager.SetPartEventProc(xName, processor);
      } else {
        // Set a processor for the entire equipment fragment
        _dataProcessManager.SetFragmentProc(xName, processor);
      }
    }
    // Deal with exceptions
    catch (MDexception & lExc) {
      Squeak::mout(Squeak::error) << lExc.GetDescription() << std::endl
      << "*** Unpacking exception in InputCppDAQData::initProcessor() : " << endl;
    }
    return true;
  } else {
    this->disableEquipment(xName);
    return false;
  }
}

void InputCppDAQData::configureZeroSupression(ZeroSupressionFilter* processor,
                                              Json::Value configJSON) {
  string xName, xDataCard;
  xName = processor->get_equipment_name();
  xDataCard = "Do_" + xName + "_Zero_Suppression";

  // Enable or disable zero supression.
  assert(configJSON.isMember(xDataCard));
  bool zs = configJSON[xDataCard].asBool();
  processor->set_zero_supression(zs);

  if (zs) {
    xDataCard = xName + "_Zero_Suppression_Threshold";
    assert(configJSON.isMember(xDataCard));
    int zs_threshold = configJSON[xDataCard].asInt();
    processor->set_zs_threshold(zs_threshold);
  }
}

void InputCppDAQData::configureZeroSupressionTK(ZeroSupressionFilterTK* processor,
                                                Json::Value configJSON) {
  string xName, xDataCard, calib_file;
  xName = processor->get_equipment_name();
  xDataCard = "Do_" + xName + "_Zero_Suppression";

  // Enable or disable zero supression.
  assert(configJSON.isMember(xDataCard));
  bool zs = configJSON[xDataCard].asBool();
  processor->set_zero_supression(zs);

  if (zs) {
    xDataCard = xName + "_Zero_Suppression_Threshold";
    assert(configJSON.isMember(xDataCard));
    int zs_threshold = configJSON[xDataCard].asInt();
    processor->set_zs_threshold(zs_threshold);

    xDataCard = "SciFiCalibrationFileName";
    assert(configJSON.isMember(xDataCard));
    calib_file = configJSON[xDataCard].asString();
    char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
    std::string fname = std::string(pMAUS_ROOT_DIR)+"/files/calibration/"+calib_file;
    std::ifstream inf(fname.c_str());
    if (!inf) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                      "Could not load Tracker calibration",
                       "InputCppDAQData::configureZeroSupressionTK"));
    } else {
      std::cerr << "Tracker calibration file found into DAQ\n";
    }
    std::string calib((std::istreambuf_iterator<char>(inf)), std::istreambuf_iterator<char>());
    Json::Reader reader;
    Json::Value calibration_data;
    TrackerCalibMap *calibration = processor->get_calibration_ptr();
    if (!reader.parse(calib, calibration_data)) {
      throw(Exceptions::Exception(Exceptions::recoverable,
                      "Could not load Tracker calibration",
                      "InputCppDAQData::configureZeroSupressionTK"));
    } else {
      std::cerr << "Tracker calibration parsing into DAQ\n";
    }
    size_t n_channels = calibration_data.size();
    for ( Json::Value::ArrayIndex i = 0; i < n_channels; ++i ) {
      int bank            = calibration_data[i]["bank"].asInt();
      int channel_n       = calibration_data[i]["channel"].asInt();
      double adc_pedestal = calibration_data[i]["adc_pedestal"].asDouble();
      double adc_gain     = calibration_data[i]["adc_gain"].asDouble();
      (*calibration)[bank][channel_n].first  = adc_pedestal;
      (*calibration)[bank][channel_n].second = adc_gain;
    }
  }
}

std::string InputCppDAQData::event_type_to_str(int pType) {
  std::string event_type;
  switch (pType) {
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
      std::stringstream xConv;
      xConv << pType << "unknown";
      event_type = xConv.str();
      break;
  }
  return event_type;
}
}
