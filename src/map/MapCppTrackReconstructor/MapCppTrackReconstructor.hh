/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

// FIXME(plane@hawk.iit.edu) UNDER CONSTRUCTION. For now this just uses Minuit
// and transfer matrices

/** @class MapCppTrackReconstructor
 *  Reconstruct tracks at the desired longitudinal spacing using the desired
 *  track fitting method.
 */


#ifndef _SRC_MAP_MAPCPPTRACKRECONSTRUCTOR_HH_
#define _SRC_MAP_MAPCPPTRACKRECONSTRUCTOR_HH_

// C headers
#include <stdlib.h>

// C++ headers
#include <string>
#include <sstream>
#include <vector>

// external libraries
#include "TMinuit.h"
#include "json/json.h"

//  MAUS code
#include "src/common_cpp/TrackReconstructor/MAUSGeant4Manager.hh"

// G4MICE from legacy
#include "BeamTools/BTField.hh"
#include "Config/MiceModule.hh"
#include "Interface/Squeak.hh"

// Minuit requires a gobal, static function to minimize. This requires a
// global instance of TMinuit to use TMinuit::GetObjectFit().
TMinuit * map_cpp_track_reconstructor_minuit;

void transfer_map_score_function(Int_t &    number_of_parameters,
                                 Double_t * gradiants,
                                 Double_t & function_value,
                                 Double_t * phase_space_coordinate_values,
                                 Int_t      execution_stage_flag);

class MapCppTrackReconstructor {
 public:
  /** @brief Allocates the global-scope Minuit instance used for minimization.
   */
  MapCppTrackReconstructor();

  /** @brief Destroys the *global* scope Minuit object
   *
   *  This takes no arguments.
   */
  ~MapCppTrackReconstructor();

  /** @brief Begin the startup procedure for TrackReconstructor
   *
   *
   *  @param config a JSON document with the configuration.
   */
  bool birth(std::string argJsonConfigDocument);

  /** @brief Shuts down the reconstructor
   *
   *  This takes no arguments
   */
  bool death();

  /** @brief Set up configuration information on the MICERun
   *  
   *  Sets the datacards, json configuration, Squeak standard outputs,
   *  and MiceModules
   */
  void SetConfiguration(std::string config);

  /** @brief Generate a list of reconstructed tracks.
   *
   *  This function takes a single spill and generates a list of tracks based
   *  on the desired method of reconstruction.
   *
   * @param document a JSON document for a spill
   */
  std::string process(std::string document);

  const double ScoreTrack(PhaseSpaceVector const * const start_plane_track)
      const;

 private:
  static const size_t kPhaseSpaceDimension;

  std::string classname_;
  BTField * electromagnetic_field_;
};

const size_t MapCppTrackReconstructor::kPhaseSpaceDimension = 6;

#endif  // _SRC_MAP_MAPCPPTRACKRECONSTRUCTOR_HH_
