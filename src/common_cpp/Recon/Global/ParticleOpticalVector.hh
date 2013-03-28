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

#ifndef COMMON_CPP_RECONSTRUCTION_PARTICLE_OPTICAL_VECTOR_HH
#define COMMON_CPP_RECONSTRUCTION_PARTICLE_OPTICAL_VECTOR_HH

#include <iostream>

#include "DataStructure/Step.hh"
#include "src/common_cpp/Optics/PhaseSpaceVector.hh"
#include "Recon/Global/Particle.hh"
#include "Recon/Global/Detector.hh"

namespace MAUS {
namespace recon {
namespace global {

/* @class ParticleOpticalVector a phase space vector with redundant t/E and z/Pz
 * coordinates as well as an ID that links the track to the detector that
 * measured it.
 *
 * The redundant sets of coordinates can be filled in by explicitly using the
 * FillInCoordinates() function or by setting all of the coordinates at once
 * using the array or parameterised constructors (which implicitly call
 * FillInCoordinates()). If t < 0 it fills in t and E from z, Pz, and the given
 * mass parameter. If t >= 0 and z < 0, it fills in z and Pz from t, E, and
 * the mass.
 */
class ParticleOpticalVector : public MAUS::PhaseSpaceVector {
 public:
  /* @brief	Construct with all elements initialized to zero and phase space type
   *        temporal.
   */
  ParticleOpticalVector();

  ParticleOpticalVector(
      const MAUS::PhaseSpaceVector& vector,
      const double t0, const double E0, const double P0);

  explicit ParticleOpticalVector(
      const MAUS::PhaseSpaceVector& original_instance);

  /* @brief  Copy constructor.
   */
  ParticleOpticalVector(const ParticleOpticalVector & original_instance);

  /* @brief Create with coordinates from an array.
     Order is t, E, x, Px, y, Py.
   */
  explicit ParticleOpticalVector(double const * const array);

  /* @brief	Reconstructed track point constructor. The track point was not
   * generated by a detector, so an uncertanty matrix must be supplied. The
   * detector_id field is set to Detector::None. 
   */
  ParticleOpticalVector(const double l, const double delta,
             const double x, const double a,
             const double y, const double b);

  ~ParticleOpticalVector();

  // *************************
  //  Assignment Operators
  // *************************
  ParticleOpticalVector& operator =(
    const ParticleOpticalVector&                  rhs);
  ParticleOpticalVector& operator+=(
    const ParticleOpticalVector&                  rhs);
  ParticleOpticalVector& operator-=(
    const ParticleOpticalVector&                  rhs);
  ParticleOpticalVector& operator*=(
    const ParticleOpticalVector&                  rhs);
  ParticleOpticalVector& operator/=(
    const ParticleOpticalVector&                  rhs);
  ParticleOpticalVector& operator+=(const double& rhs);
  ParticleOpticalVector& operator-=(const double& rhs);
  ParticleOpticalVector& operator*=(const double& rhs);
  ParticleOpticalVector& operator/=(const double& rhs);

  // *************************
  //  Algebraic Operators
  // *************************
  const ParticleOpticalVector operator+(
    const ParticleOpticalVector&                      rhs) const;
  const ParticleOpticalVector operator-(
    const ParticleOpticalVector&                      rhs) const;
  const ParticleOpticalVector operator*(
    const ParticleOpticalVector&                      rhs) const;
  const ParticleOpticalVector operator/(
    const ParticleOpticalVector&                      rhs) const;
  const ParticleOpticalVector operator+(const double& rhs) const;
  const ParticleOpticalVector operator-(const double& rhs) const;
  const ParticleOpticalVector operator*(const double& rhs) const;
  const ParticleOpticalVector operator/(const double& rhs) const;

  // *************************
  //  Comparison Operators
  // *************************
  const bool operator==(const ParticleOpticalVector& rhs) const;
  const bool operator!=(const ParticleOpticalVector& rhs) const;

  double l()            const {return (*this)[0];}
  double delta()        const {return (*this)[1];}
  double x()            const {return (*this)[2];}
  double a()            const {return (*this)[3];}
  double y()            const {return (*this)[4];}
  double b()            const {return (*this)[5];}

  // mutators
  void set_l(double time)                 {(*this)[0] = time;}
  void set_delta(double energy)           {(*this)[1] = energy;}
  void set_x(double x)                    {(*this)[2] = x;}
  void set_a(double a)                    {(*this)[3] = a;}
  void set_y(double y)                    {(*this)[4] = y;}
  void set_b(double b)                    {(*this)[5] = b;}
};

std::ostream& operator<<(std::ostream& out, const ParticleOpticalVector& vector);

}  // namespace global
}  // namespace recon
}  // namespace MAUS

#endif
