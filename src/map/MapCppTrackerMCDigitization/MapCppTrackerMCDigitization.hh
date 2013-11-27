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

/** @class MapCppTrackerMCDigitization
 *  Digitize events by running Tracker electronics simulation.
 *
 */

#ifndef _COMPONENTS_MAP_MAPCPPTRACKERMCDIGITIZATION_H_
#define _COMPONENTS_MAP_MAPCPPTRACKERMCDIGITIZATION_H_

// C headers
#include <json/json.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <cmath>
#include <vector>
#include <string>

#include "Config/MiceModule.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/SciFiMCLookup.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

namespace MAUS {

class MapCppTrackerMCDigitization {
 public:
  /** Constructor - initialises pointers to NULL */
  MapCppTrackerMCDigitization();

  /** Constructor - deletes any allocated memory */
  ~MapCppTrackerMCDigitization();

  /** Sets up the worker
   *
   *  \param argJsonConfigDocument a JSON document with
   *         the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** @brief Shutdowns the worker
   *
   *  This takes no arguments and does nothing
   */
  bool death();

  /** @brief process JSON document
   *
   *  Receive a document with MC hits and return
   *  a document with digits
   */
  std::string process(std::string document);

  /** @brief reads in json data to a Spill object
   *
   */
  void read_in_json(std::string json_data);

  /** @brief builds digits
   */
  void construct_digits(MAUS::SciFiHitArray *hits,
                        int spill_num, int event_num,
                        MAUS::SciFiDigitPArray &digits);

  void add_noise(MAUS::SciFiNoiseHitArray *noises, MAUS::SciFiDigitPArray &digits);

  void lookup_noise(MAUS::SciFiNoiseHitArray *noises, MAUS::SciFiMCLookupArray *lookups);

  /** @brief computes scifi chan numb from GEANT fibre copy numb
   */
  int compute_chan_no(MAUS::SciFiHit *ahit);

  /** @brief computes tdc from time.
   */
  int compute_tdc_counts(double time);

  /** @brief computes adc from npe.
   */
  int compute_adc_counts(double numb_pe);

  /** @brief checks if hits belong to the same scifi channel.
   */
  bool check_param(MAUS::SciFiHit *hit1, MAUS::SciFiHit *hit2);

  /** @brief saves digits to json.
   */
  void save_to_json(MAUS::Spill &spill);

 private:
  /// This should be the classname
  std::string _classname;
  /// This will contain the root value after parsing
  Json::Value* _spill_json;
  Spill* _spill_cpp;
  ///  JsonCpp setup
  Json::Reader reader;
  /// The ratio of deposited eV to NPE
  double _eV_to_phe;
  double _SciFiNPECut;
  double _SciFivlpcEnergyRes;
  double _SciFiadcFactor;
  double _SciFitdcBits;
  double _SciFivlpcTimeRes;
  double _SciFitdcFactor;
  double _SciFiFiberConvFactor;
  double _SciFiFiberTrappingEff;
  double _SciFiFiberMirrorEff;
  double _SciFivlpcQE;
  double _SciFiFiberTransmissionEff;
  double _SciFiMUXTransmissionEff;

  /// an array contaning all MiceModules
  std::vector<const MiceModule*> modules;
};  // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
