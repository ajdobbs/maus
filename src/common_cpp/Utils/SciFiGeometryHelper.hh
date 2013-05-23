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

#ifndef _SRC_COMMON_CPP_UTILS_SCIFIGEOMETRYHELPER_HH
#define _SRC_COMMON_CPP_UTILS_SCIFIGEOMETRYHELPER_HH

#include <vector>
#include <map>

#include "src/legacy/Config/MiceModule.hh"
#include "Geant4/G4RotationMatrix.hh"
#include "src/common_cpp/Utils/ThreeVectorUtils.hh"

namespace MAUS {

struct SciFiPlaneGeometry {
  ThreeVector Direction;
  ThreeVector Position;
  double CentralFibre;
  double Pitch;
};

class SciFiGeometryHelper {
 public:
  explicit SciFiGeometryHelper(std::vector<const MiceModule*> modules);

  ~SciFiGeometryHelper();

  std::map<int, SciFiPlaneGeometry> build_geometry_map();

  const MiceModule* find_plane(int tracker, int station, int plane);

  ThreeVector get_reference_frame_pos(int tracker);

 private:
  std::vector<const MiceModule*> _modules;
}; // Don't forget this trailing colon!!!!
} // ~namespace MAUS

#endif
