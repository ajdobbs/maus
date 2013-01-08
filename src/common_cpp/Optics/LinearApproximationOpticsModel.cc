/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

/* Author: Peter Lane
 */

#include "Optics/LinearApproximationOpticsModel.hh"

#include <cmath>
#include <vector>
#include <iostream>

#include "src/common_cpp/Optics/CovarianceMatrix.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "src/common_cpp/Recon/Global/Particle.hh"
#include "src/common_cpp/Recon/Global/WorkingTrackPoint.hh"
#include "Maths/Vector.hh"

#include "Interface/Squeal.hh"

namespace MAUS {

const TransferMap * LinearApproximationOpticsModel::CalculateTransferMap(
    const std::vector<recon::global::WorkingTrackPoint> & start_plane_hits,
    const std::vector<recon::global::WorkingTrackPoint> & station_hits)
    const {
  const recon::global::Particle::ID particle_id
    = recon::global::Particle::ID(start_plane_hits[0].particle_id());

  double start_plane = start_plane_hits[0].z();

  double hit_total = 0.0;
  std::vector<recon::global::WorkingTrackPoint>::const_iterator hit;
  for (hit = station_hits.begin(); hit != station_hits.end(); ++hit) {
    hit_total += hit->z();
  }
  double end_plane = hit_total / station_hits.size();

  const double mass
    = recon::global::Particle::GetInstance()->GetMass(particle_id);

  return new LinearApproximationTransferMap(start_plane, end_plane, mass);
}

TransferMap * LinearApproximationTransferMap::Inverse() const {
  return new LinearApproximationTransferMap(end_plane_, start_plane_, mass_);
}

CovarianceMatrix LinearApproximationTransferMap::Transport(
    const CovarianceMatrix & covariances) const {
  // This method of transport is too stupid to handle covariance matrices,
  // so just return what was given.
  return covariances;
}

PhaseSpaceVector LinearApproximationTransferMap::Transport(
    const PhaseSpaceVector & vector) const {
  // Use the energy and momentum to determine when and where the particle would
  // be if it were traveling in free space from start_plane_ to end_plane_.
  PhaseSpaceVector transported_vector(vector);
  double delta_z = end_plane_ - start_plane_;

  const double px = vector.Px();
  const double py = vector.Py();

  double energy = vector.E();
  if (mass_ > energy) {
    throw(Squeal(Squeal::nonRecoverable,
                 "Attempting to transport a particle that is off mass shell.",
                 "MAUS::LinearApproximationOpticsModel::Transport()"));
  }
  const double momentum = ::sqrt(energy*energy - mass_*mass_);
  const double beta = momentum / energy;

  // delta_t = delta_z / v_z ~= delta_z / velocity
  double delta_t = delta_z / beta / ::CLHEP::c_light;

  const double gamma = energy / mass_;

  // delta_x = v_x * delta_t, px = gamma * m * v_x
  double delta_x = px * delta_t / gamma / mass_;

  // delta_y = v_y * delta_t, py = gamma * m v_y
  double delta_y = py * delta_t / gamma / mass_;

  transported_vector[0] += delta_t;
  transported_vector[2] += delta_x;
  transported_vector[4] += delta_y;

  return transported_vector;
}

}  // namespace MAUS
