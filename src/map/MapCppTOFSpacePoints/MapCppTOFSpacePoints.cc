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

#include <vector>
#include <string>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/map/MapCppTOFSpacePoints/MapCppTOFSpacePoints.hh"

bool MapCppTOFSpacePoints::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "MapCppTOFSpacePoints";

  // JsonCpp setup
  Json::Value configJSON;
  try {
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    // this will contain the configuration

    // Load the calibration.
    _map_init = true;
    bool loaded = _map.InitializeFromCards(configJSON);
    if (!loaded)
      _map_init = false;
    std::cout << "_map_init = " << _map_init << std::endl;
    _makeSpacePointCut =
    JsonWrapper::GetProperty(configJSON,
                             "TOF_makeSpacePointCut",
                             JsonWrapper::realValue).asDouble(); // nanoseconds
    _findTriggerPixelCut =
    JsonWrapper::GetProperty(configJSON,
                             "TOF_findTriggerPixelCut",
                             JsonWrapper::realValue).asDouble(); // nanoseconds

    _triggerStation
      = JsonWrapper::GetProperty(configJSON,
                                 "TOF_trigger_station",
                                 JsonWrapper::stringValue).asString();

    // The first element of the vectro has to be the trigger station.
    // This is mandatory!!!
    _stationKeys.push_back(_triggerStation);
    if (_triggerStation == "tof1") {
      _stationKeys.push_back("tof0");
      _stationKeys.push_back("tof2");
    } else if (_triggerStation == "tof0") {
      _stationKeys.push_back("tof1");
      _stationKeys.push_back("tof2");
    } else {
      Squeak::mout(Squeak::error)
      << "Error in MapCppTOFSpacePoints::birth. TOF trigger station is wrong."
      << "It can be tof1 or tof0. The provided trigger station is : "
      << _triggerStation << std::endl;

      return false;
    }

  return true;
  } catch(Squeal squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}

bool MapCppTOFSpacePoints::death()  {return true;}

std::string MapCppTOFSpacePoints::process(std::string document) {
  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;
  if (!_map_init) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to initialize calibration map";
    errors["no_tofcalib"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  // Check if the JSON document can be parsed, else return error only
  try {root = JsonWrapper::StringToJson(document);}
  catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  try {
    xEventType = JsonWrapper::GetProperty(root,
                                          "daq_event_type",
                                          JsonWrapper::stringValue);
    if (xEventType == "physics_event" || xEventType == "calibration_event") {
      _triggerhit_pixels.clear();
      Json::Value xRecEvent = JsonWrapper::GetProperty(root,
                                                       "recon_events",
                                                     JsonWrapper::arrayValue);
      for (unsigned int n_event = 0; n_event < xRecEvent.size(); n_event++) {
        Json::Value xTofEvent = JsonWrapper::GetItem
                                 (xRecEvent, n_event, JsonWrapper::objectValue);
        xTofEvent = JsonWrapper::GetProperty
                             (xTofEvent, "tof_event", JsonWrapper::objectValue);
        if (xTofEvent.isMember("tof_slab_hits")) {
          Json::Value xSlabHits = JsonWrapper::GetProperty
                         (xTofEvent, "tof_slab_hits", JsonWrapper::objectValue);

          // NOTE: DR March15
          // Cheating -- until I figure out how to handle trig-req-time in MC:
          //   I have to change the triggerpixelcut for MC in order for the
          //   calib corrections to be applied correctly.
          //   2 options --
          //   a) change the cut in ConfigDefaults
          //      but this <may> mess up data -- though I did run on real data
          //      with this modified cut and things (resol,time) look OK @ 1st
          //      glance
          //   b) use a different cut if it's MC
          //      this breaks the agreement that we'll treat real data/MC same
          //      way but for now it at least lets MC get reconstructed without
          //      clobbering real data
          // For now I have chosen option a) with option b) commented out below
          // need to be sure to change ConfigDefaults for switching between
          // data<->mc
          if (root.isMember("mc_events")) _findTriggerPixelCut = 50.0;

          // Loop over each station starting from the trigger station.
          for (unsigned int n_station = 0;
               n_station < _stationKeys.size();
               n_station++) {
            std::string detector = _stationKeys[n_station];
            if (xSlabHits.isMember(detector))
              root["recon_events"][n_event]["tof_event"]["tof_space_points"]
                   [detector] = processTOFStation(xSlabHits, detector, n_event);
          }
          // The slab hit document is now modified. The calibrated time
          // measurements are added. Save the modifications.
          root["recon_events"][n_event]["tof_event"]["tof_slab_hits"]
            = xSlabHits;
        }
      }
    }
  } catch(Squeal squee) {
    root = MAUS::CppErrorHandler::getInstance()
                                       ->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    root = MAUS::CppErrorHandler::getInstance()
                                         ->HandleStdExc(root, exc, _classname);
  }

  // if (root.isMember("slab_hits")) std::cout << root["slab_hits"] << std::endl;
  // if (root.isMember("space_points")) std::cout << root["space_points"] << std::endl;
  return writer.write(root);
}

Json::Value MapCppTOFSpacePoints::processTOFStation(Json::Value &xSlabHits,
                                                    std::string detector,
                                                    unsigned int part_event) {
  // Get the slab hits document for this TOF station.
  Json::Value xDocPartEvent = JsonWrapper::GetProperty(xSlabHits,
                                                       detector,
                                                       JsonWrapper::anyValue);
  Json::Value xDocPartEventSpacePoints(Json::arrayValue);
  if (xDocPartEvent.isArray()) {
    int n_slab_hits = xDocPartEvent.size();
    // Delete the information from the previous particle event.
    _xPlane0Hits.resize(0);
    _xPlane1Hits.resize(0);

    // Loop over the slab hits and select the hits in
    // plane0 (horizontal) and plane1 (vertical).
    for (int SlHit = 0; SlHit < n_slab_hits; SlHit++) {
      // Get the slab hit.
      Json::Value xThisSlabHit = JsonWrapper::GetItem(xDocPartEvent,
                                                      SlHit,
                                                      JsonWrapper::objectValue);

      int xPlane  = JsonWrapper::GetProperty(xThisSlabHit,
                                             "plane",
                                             JsonWrapper::intValue).asInt();

      // According to the convention used in the cabling file the horizontal
      // slabs are always in plane 0 and the vertical slabs are always in
      // plane 1.
      switch (xPlane) {
        case 0 :
          _xPlane0Hits.push_back(SlHit);
          break;
        case 1 :
          _xPlane1Hits.push_back(SlHit);
          break;
      }
    }

    // If this is the trigger station find the pixel that is giving the trigger.
    if (detector == _triggerStation)
      _triggerhit_pixels[part_event] = findTriggerPixel(xDocPartEvent);
    // If we do not know the trigger pixel there is no way to reconstruct the
    // time.
    if (_triggerhit_pixels[part_event] != "unknown") {
      // Create the space point. Add the calibrated value of the time to the
      // slab hits.
      xDocPartEventSpacePoints = makeSpacePoints(xDocPartEvent);

      // The slab hit document is now modified. The calibrated time measurements
      // are added. Save the modifications.
      xSlabHits[detector] = xDocPartEvent;
    }
  }

  return xDocPartEventSpacePoints;
}

std::string MapCppTOFSpacePoints::findTriggerPixel(Json::Value xDocPartEvent) {
  // Loop over all possible combinations of slab hits in the trigger station.
  for (unsigned int nX = 0; nX < _xPlane0Hits.size(); nX++) {
    for (unsigned int nY = 0; nY < _xPlane1Hits.size(); nY++) {
      // Get the two slab hits.
      Json::Value xSlabHit_X = JsonWrapper::GetItem(xDocPartEvent,
                                                    _xPlane0Hits[nX],
                                                    JsonWrapper::objectValue);

      Json::Value xSlabHit_Y = JsonWrapper::GetItem(xDocPartEvent,
                                                    _xPlane1Hits[nY],
                                                    JsonWrapper::objectValue);

      int slabX = xSlabHit_X["slab"].asInt();
      int slabY = xSlabHit_Y["slab"].asInt();
      TOFPixelKey xTriggerPixelKey(1, slabX, slabY, _triggerStation);
      // Apply the calibration corrections assuming that this pixel gives the
      // trigger. If this assumption is correct the value of the time after the
      // corrections has to be approximately 0.
      double t_x, t_y;
      if (calibrateSlabHit(xTriggerPixelKey, xSlabHit_X, t_x) &&
          calibrateSlabHit(xTriggerPixelKey, xSlabHit_Y, t_y)) {
        if (fabs(t_x/2. + t_y/2.) < _findTriggerPixelCut) {
          // The trigger pixel has been found.
          // std::cout << xTriggerPixelKey << std::endl;
          return xTriggerPixelKey.str();
        }
      }
    }
  }
  return "unknown";
}

Json::Value MapCppTOFSpacePoints::makeSpacePoints(Json::Value &xDocPartEvent) {
  Json::Value xDocSpacePoints(Json::arrayValue);
  // Loop over all possible combinations of slab hits in the trigger station.
  for (unsigned int nX = 0; nX < _xPlane0Hits.size(); nX++) {
    for (unsigned int nY = 0; nY < _xPlane1Hits.size(); nY++) {
      Json::Value xDocSpacePoint;
      int xPartEvent = JsonWrapper::GetProperty(xDocPartEvent[_xPlane0Hits[0]],
                                               "part_event_number",
                                               JsonWrapper::intValue).asInt();
      TOFPixelKey xTriggerPixelKey(_triggerhit_pixels[xPartEvent]);
      double t_x, t_y;
      if (calibrateSlabHit(xTriggerPixelKey,
                           xDocPartEvent[_xPlane0Hits[nX]],
                           t_x) &&
          calibrateSlabHit(xTriggerPixelKey,
                           xDocPartEvent[_xPlane1Hits[nY]],
                           t_y)) {
        // The first argument should be the hit in the horizontal slab and the
        // second should be the hit in the vertical slab. This is mandatory!!!
        Json::Value xDocSpacePoint
          = fillSpacePoint(xDocPartEvent[_xPlane0Hits[nX]],
                                         xDocPartEvent[_xPlane1Hits[nY]]);
        double deltaT = xDocSpacePoint["dt"].asInt();
        if (fabs(deltaT) < _makeSpacePointCut) {
          xDocSpacePoints.append(xDocSpacePoint);
        }
      }
    }
  }

  return xDocSpacePoints;
}

Json::Value MapCppTOFSpacePoints::fillSpacePoint(Json::Value &xDocSlabHit_X,
                                                 Json::Value &xDocSlabHit_Y) {

  Json::Value xDocSpacePoint;

  // First get the two channel keys and make the pixel key.
  std::string keyStr_SlabX_digit0
    = JsonWrapper::GetProperty(xDocSlabHit_X["pmt0"],
                               "tof_key",
                               JsonWrapper::stringValue).asString();
  TOFChannelKey xKey_SlabX_digit0(keyStr_SlabX_digit0);

  std::string keyStr_SlabY_digit0
    = JsonWrapper::GetProperty(xDocSlabHit_Y["pmt0"],
                               "tof_key",
                               JsonWrapper::stringValue).asString();
  TOFChannelKey xKey_SlabY_digit0(keyStr_SlabY_digit0);

  // ATTENTION : according to the convention used in the cabling file the
  // horizontal slabs are always in plane 0 and the vertical slabs are always in
  // plane 1.
  // The second argument in the constructor has to be the number of the
  // horizontal slab, and the third argument has to be the vertical slab.
  // This is mandatory!!!
  TOFPixelKey xSPKey(xKey_SlabX_digit0.station(),
                     xKey_SlabX_digit0.slab(),
                     xKey_SlabY_digit0.slab(),
                     xKey_SlabY_digit0.detector());

  // Get the corrected time from the two slab hits.
  double time_SlabX
    = JsonWrapper::GetProperty(xDocSlabHit_X,
                               "time",
                               JsonWrapper::realValue).asDouble();

  double time_SlabY
    = JsonWrapper::GetProperty(xDocSlabHit_Y,
                               "time",
                               JsonWrapper::realValue).asDouble();

  // Get the charge and the charge product.
  int charge_SlabX = JsonWrapper::GetProperty(xDocSlabHit_X,
                                              "charge",
                                              JsonWrapper::intValue).asInt();
  int chargeProduct_SlabX
    = JsonWrapper::GetProperty(xDocSlabHit_X,
                               "charge_product",
                               JsonWrapper::intValue).asInt();

  int charge_SlabY = JsonWrapper::GetProperty(xDocSlabHit_Y,
                                              "charge",
                                              JsonWrapper::intValue).asInt();
  int chargeProduct_SlabY
    = JsonWrapper::GetProperty(xDocSlabHit_Y,
                               "charge_product",
                               JsonWrapper::intValue).asInt();

  // Now calculate the time of the space point.
  double time = (time_SlabX + time_SlabY)/2.;
  double dt = time_SlabX - time_SlabY;
  xDocSpacePoint["time"] = time;
  xDocSpacePoint["dt"] = dt;

  // Fill all other necessary information in the Space point.
  xDocSpacePoint["part_event_number"] = xDocSlabHit_X["part_event_number"];
  xDocSpacePoint["phys_event_number"] = xDocSlabHit_X["phys_event_number"];
  xDocSpacePoint["slabX"]     = xKey_SlabX_digit0.slab();
  xDocSpacePoint["station"]   = xKey_SlabX_digit0.station();
  xDocSpacePoint["detector"]  = xKey_SlabX_digit0.detector();
  xDocSpacePoint["slabY"]     = xKey_SlabY_digit0.slab();
  xDocSpacePoint["pixel_key"] = xSPKey.str();
  xDocSpacePoint["charge"] = charge_SlabX + charge_SlabY;
  xDocSpacePoint["charge_product"] = chargeProduct_SlabX + chargeProduct_SlabY;

  // std::cout << xSPKey << "  t = " << time << " dt = " << dt << std::endl;
  // std::cout << xDocSpacePoint << std::endl;
  return xDocSpacePoint;
}

bool MapCppTOFSpacePoints::calibratePmtHit(TOFPixelKey xTriggerPixelKey,
                                           Json::Value &xPmtHit,
                                           double &time) {
  int charge;
  // Charge of the digit can be unset because of the Zero suppresion of the
  // fADCs.
  if (xPmtHit.isMember("charge"))
    charge= JsonWrapper::GetProperty(xPmtHit,
                                     "charge",
                                     JsonWrapper::intValue).asInt();
  else
    return  false;

  std::string keyStr
    = JsonWrapper::GetProperty(xPmtHit,
                               "tof_key",
                               JsonWrapper::stringValue).asString();
  TOFChannelKey xChannelKey(keyStr);

  double raw_time 
    = JsonWrapper::GetProperty(xPmtHit,
                               "raw_time",
                               JsonWrapper::realValue).asDouble();

  // Get the calibration correction.
  double dT = _map.dT(xChannelKey, xTriggerPixelKey, charge);
  if (dT == TOFCalibrationMap::NOCALIB)
    return  false;

  time = raw_time - dT;
  xPmtHit["time"] = time;
  std::cout << "calibratePmtHit " << xChannelKey << " " << xTriggerPixelKey
            << " t = " << raw_time << " - " << dT << " = " << time << std::endl;
  return true;
}

bool MapCppTOFSpacePoints::calibrateSlabHit(TOFPixelKey xTriggerPixelKey,
                                            Json::Value &xSlabHit,
                                            double &time) {

  double time_digit0, time_digit1;

  // Calibrate the digit measurements.
  if (calibratePmtHit(xTriggerPixelKey, xSlabHit["pmt0"], time_digit0) &&
      calibratePmtHit(xTriggerPixelKey, xSlabHit["pmt1"], time_digit1)) {
    time = (time_digit0 + time_digit1)/2.;
    xSlabHit["time"] = time;
    return true;
  }
  return false;
}


