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

#include "DetModel/MAUSSD.hh"

namespace MAUS {

MAUSSD::MAUSSD(MiceModule* mod)
    : G4VSensitiveDetector(mod->fullName()) {
}

bool MAUSSD::isHit() {
  if ( _hits.size() > 0 ) {
    return true;
  }
  return false;
}

}  // ends MAUS namespace
