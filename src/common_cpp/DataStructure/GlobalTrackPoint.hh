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

/** @class MAUS::recon::global::TrackPoint
 *  @ingroup globalrecon
 *  @brief TrackPoint object for the global reconstruction.
 *
 *  A reconstructed track point, produced by the global
 *  reconstruction.  It represents a single point in space through
 *  which the reconstruction has determined the
 *  MAUS::recon::global::Track passed, defined by a position and
 *  momentum measurement, with associated errors.

 *  Most TrackPoints will correspond to a detector measurement (with a
 *  MAUS::recon::global::SpacePoint object), but some will be virtual
 *  (MAUS::recon::global::DetectorPoint kVirtual), matching a
 *  projected point.
 *
 */

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACKPOINT_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_GLOBALTRACKPOINT_HH_


// C++ headers
#include <string>
#include <vector>
#include <iostream>

// ROOT headers
#include "Rtypes.h"
#include "TLorentzVector.h"
#include "TRef.h"

// MAUS headers
#include "src/common_cpp/DataStructure/GlobalReconEnums.hh"
#include "src/common_cpp/DataStructure/GlobalSpacePoint.hh"

namespace MAUS {
namespace recon {
namespace global {

class TrackPoint : public TObject{
 public:

  /// Default constructor - initialises to 0/NULL
  TrackPoint();

  /// Copy constructor - any pointers are deep copied
  TrackPoint(const TrackPoint &trackpoint);

  /// Destructor
  virtual ~TrackPoint();

  /// Assignment operator - any pointers are deep copied
  TrackPoint& operator=(const TrackPoint &trackpoint);

  /// Clone method.  This allows us to create a new object and change
  /// that object, without changing the original.  This should be the
  /// standard copy method within a mapper.
  TrackPoint* Clone();

  // Getters and Setters for the member variables
  
  /// Set the name for the mapper which produced the track, #_mapper_name.
  void set_mapper_name(std::string mapper_name) {
    _mapper_name = mapper_name;
  }
  /// Get the name for the mapper which produced the track, #_mapper_name.
  std::string get_mapper_name() const {
    return _mapper_name;
  }

  /// Set the calibrated 'charge' or energy deposit of the track at
  /// this point, based on detector measurements.  This will be
  /// undefined for MAUS::recon::global::kVirtual points.
  void set_charge(double charge) {
    _charge = charge;
  }
  /// Get the calibrated 'charge' or energy deposit at this point,
  /// #_charge.
  double get_charge() const {
    return _charge;
  }

  /// Set the 4D position of the reconstructed
  /// MAUS::recon::global::Track at this point.
  void set_position(TLorentzVector position) {
    _position = position;
  }
  /// Get the 4D position of the reconstructed
  /// MAUS::recon::global::Track at this point.
  TLorentzVector get_position() const {
    return _position;
  }

  /// Set the error on the 4D position of the reconstructed
  /// MAUS::recon::global::Track at this point.
  void set_position_error(TLorentzVector position_error) {
    _position_error = position_error;
  }
  /// Get the error on the 4D position of the reconstructed
  /// MAUS::recon::global::Track at this point.
  TLorentzVector get_position_error() const {
    return _position_error;
  }

  /// Set the 4-momentum of the reconstructed
  /// MAUS::recon::global::Track at this point.
  void set_momentum(TLorentzVector momentum) {
    _momentum = momentum;
  }
  /// Get the 4-momentum of the reconstructed
  /// MAUS::recon::global::Track at this point.
  TLorentzVector get_momentum() const {
    return _momentum;
  }

  /// Set the error on the 4-momentum of the reconstructed
  /// MAUS::recon::global::Track at this point.
  void set_momentum_error(TLorentzVector momentum_error) {
    _momentum_error = momentum_error;
  }
  /// Get the error on the 4-momentum of the reconstructed
  /// MAUS::recon::global::Track at this point.
  TLorentzVector get_momentum_error() const {
    return _momentum_error;
  }

  /// Set the MAUS::global::recon::DetectorPoint flag for the
  /// SpacePoint, defining which detector produced the point. If the
  /// flag is kVirtual, then the TrackPoint does not correspond to a
  /// detector measurment.
  void set_detector(DetectorPoint detector) {
    _detector = detector;
  }
  /// Get the MAUS::global::recon::DetectorPoint flag for the
  /// SpacePoint, defining which detector produced the point. If the
  /// flag is kVirtual, then the TrackPoint does not correspond to a
  /// detector measurment.
  DetectorPoint get_detector() const {
    return _detector;
  }

  /// Set the fully qualified geometry path of this point in space.
  /// This will be primarily used for kVirtual TrackPoints, which have
  /// no matching detector measurement, but have been filled to define
  /// 'physics' points, e.g. immediately upstream of absorber region.
  void set_geometry_path(std::string geometry_path) {
    _geometry_path = geometry_path;
  }
  /// Get the fully qualified geometry path of this point in space.
  /// This will be primarily used for kVirtual TrackPoints, which have
  /// no matching detector measurement, but have been filled to define
  /// 'physics' points, e.g. immediately upstream of absorber region.
  std::string get_geometry_path() const {
    return _geometry_path;
  }

  /// Set the component MAUS::recon::global::SpacePoint, the detector
  /// measurement matching this reconstructed position.
  void set_spacepoint(MAUS::recon::global::SpacePoint* spacepoint) {
    _spacepoint = spacepoint;
    _spacepoint.ls();
  }
  /// Get the component MAUS::recon::global::SpacePoint, the detector
  /// measurement matching this reconstructed position.
  MAUS::recon::global::SpacePoint* get_spacepoint() const {
    MAUS::recon::global::SpacePoint* spacepoint =
        static_cast<MAUS::recon::global::SpacePoint*>(_spacepoint.GetObject());
    return spacepoint;
  }

 private:

  /// The name of the mapper which produced this track.
  std::string    _mapper_name;

  /// The calibrated 'charge' or energy deposit of this point.
  ///
  /// \todo It is possible that this should simply be the matching
  /// SpacePoint's charge, in which case it may be removed the and
  /// Set/Get functions changed to access the underlieing SpacePoint.
  double         _charge;

  /// The 4D position of the reconstructed Track at this point.
  TLorentzVector  _position;
  /// The error on the 4D position of the reconstructed Track at this
  /// point.
  TLorentzVector  _position_error;

  /// The 4-momentum of the reconstructed Track at this point.
  TLorentzVector  _momentum;
  /// The error on the 4-momentum of the reconstructed Track at this
  /// point.
  TLorentzVector  _momentum_error;

  /// The detector element for this TrackPoint, either the detector
  /// which produced the matching SpacePoint or kVirtual for 'physics'
  /// TrackPoint's at user-defined locations.
  DetectorPoint  _detector;
  /// The fully qualified geometry path for this point in space.
  std::string    _geometry_path;

  /// The reconstruction object defining the matching detector
  /// measurement.
  TRef _spacepoint;

  ClassDef(TrackPoint, 1)
}; // ~class TrackPoint
  
} // ~namespace global
} // ~namespace recon

typedef MAUS::recon::global::TrackPoint GlobalTrackPoint;

typedef std::vector<MAUS::GlobalTrackPoint*> GlobalTrackPointPArray;

typedef std::vector<const MAUS::GlobalTrackPoint*> ConstGlobalTrackPointPArray;

} // ~namespace MAUS

#endif
