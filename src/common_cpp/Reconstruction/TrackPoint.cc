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

#include "Reconstruction/TrackPoint.hh"

#include <cmath>
#include "CLHEP/Units/PhysicalConstants.h"

#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "Interface/Squeak.hh"
#include "Reconstruction/Detector.hh"

namespace MAUS {

TrackPoint::TrackPoint()
    : PhaseSpaceVector(),
      z_(0.), z_momentum_(0.), detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix());
{ }

TrackPoint::TrackPoint(const TrackPoint& original_instance)
    : PhaseSpaceVector(original_instance),
      z_(original_instance.z_), z_momentum_(original_instance.z_momentum_),
      detector_id_(original_instance.detector_id_),
      uncertainties_(new CovarianceMatrix(original_instance.uncertainties_))
{ }

TrackPoint::TrackPoint(const double t, const double E,
                                   const double x, const double Px,
                                   const double y, const double Py,
                                   const double z, const double Pz,
                                   const CovarianceMatrix & uncertainties)
    : PhaseSpaceVector(t, E, x, Px, y, Py),
      z_(z), z_momentum_(Pz), detector_id_(Detector::kNone),
      uncertainties_(new CovarianceMatrix(uncertainties)) {
}

TrackPoint::TrackPoint(const double t, const double E,
                                   const double x, const double Px,
                                   const double y, const double Py,
                                   const double z, const double Pz,
                                   const Detector & detector)
    : PhaseSpaceVector(t, E, x, Px, y, Py), z_(z), z_momentum_(Pz),
      detector_id_(detector.id_),
      uncertainties_(new CovarianceMatrix(detector.uncertainties())) {
}

TrackPoint::TrackPoint(double const * const array)
    : PhaseSpaceVector(array), z_(array[6]), z_momentum(array[7]),
      detector_id_(Detector::kNone), uncertainties_(new CovarianceMatrix()) {
}

TrackPoint::~TrackPoint() {
  delete uncertainties_;
}

double TrackPoint::z() const {
  return z_;
}

void TrackPoint::set_z(const double z) {
  z_ = z;
}

double TrackPoint::z_momentum() const {
  return z_momentum_;
}

void TrackPoint::set_z_momentum(const double z_momentum) {
  z_momentum_ = z_momentum;
}

double TrackPoint::Pz() const {
  return z_momentum_;
}

void TrackPoint::set_Pz(const double Pz) {
  z_momentum_ = Pz;
}

void TrackPoint::set_detector_id(unsigned int id) {
  detector_id_ = id;
}

unsigned int TrackPoint::detector_id() const {
  return detector_id_;
}

void TrackPoint::set_uncertainties(const CovarianceMatrix & uncertainties {
  delete uncertainties_;
  uncertainties_ = new CovarianceMatrix(uncertainties);
}

const CovarianceMatrix & TrackPoint::uncertainties() const {
  return uncertainties_;
}

/* If t < 0 it fills in t and E from z, Pz, and the given mass parameter.
 * If t >= 0 and z < 0, it fills in z and Pz from t, E, and the mass.
 */
void TrackPoint::FillInCoordinates(const double mass) {
  double c_squared = ::CLHEP::c_light * ::CLHEP::c_light;
  double px = (*this)[3];
  double py = (*this)[5];

  if ((*this)[0] < 0) {
    // fill in the time coordinate
    double velocity = z_momentum_ / mass;
    (*this)[0] = z_ / velocity;

    // fill in the energy coordinate
    (*this)[1] = ::sqrt(mass*mass + px*px + py*py);
  } else if (z_ < 0) {
    // fill in the Pz coordinate
    double energy = (*this)[1]
    z_momentum_ = ::sqrt(energy*energy - mass*mass - px*px - py*py);

    // fill in the z coordinate
    double velocity = z_momentum_ / mass;
    double time = (*this)[0];
    z_ = velocity * time;
  }
}

MAUS::MAUSPrimaryGeneratorAction::PGParticle
PrimaryGeneratorParticle(const TrackPoint & point) {
  MAUSPrimaryGeneratorAction::PGParticle particle;
  particle.time = point.t();
  particle.energy = point.E();
  particle.x = point.x();
  particle.px = point.Px();
  particle.y = point.y();
  particle.py = point.Py();
  particle.z = point.z();
  particle.pz = point.Pz();
  particle.pid = point.particle_id_;
  particle.seed = 0; //BUG

  return particle;
}

std::ostream& operator<<(std::ostream& out, const TrackPoint& vector) {
  out << "t: " << vector[0]  << "E: "   << vector[1]
      << "x: " << vector[2]  << "Px: "  << vector[3]
      << "y: " << vector[4]  << "Py: "  << vector[5]
      << "z: " << vector.z() << "Pz: "  << vector.Pz();
  return out;
}

}  // namespace MAUS
