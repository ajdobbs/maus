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

#include "DataStructure/GlobalTrack.hh"

#include <algorithm>

#include "Interface/Squeak.hh"
#include "DataStructure/GlobalTrackPoint.hh"

namespace MAUS {
namespace recon {
namespace global {

/// Global function to sort MAUS::recon::global::TrackPoint's based on
/// their Z coordinate.
bool SortByZ(const MAUS::GlobalTrackPoint* tp1,
             const MAUS::GlobalTrackPoint* tp2) {
  return (tp1->get_position().Z() < tp2->get_position().Z());
}

// Default constructor
Track::Track()
    : _mapper_name(""),
      _pid(MAUS::recon::global::kNoPID),
      _charge(0),
      _detectorpoints(0),
      _goodness_of_fit(0.) {
  _trackpoints = new TRefArray();
  _constituent_tracks = new TRefArray();
}

// Copy contructor
Track::Track(const Track &track)
    : _mapper_name(track.get_mapper_name()),
      _pid(track.get_pid()),
      _charge(track.get_charge()),
      _trackpoints(track.get_trackpoints()),
      _detectorpoints(track.get_detectorpoints()),
      _geometry_paths(track.get_geometry_paths()),
      _constituent_tracks(track.get_constituent_tracks()),
      _goodness_of_fit(track.get_goodness_of_fit()) {
  _trackpoints = new TRefArray(*track.get_trackpoints());
  _constituent_tracks = new TRefArray(*track.get_constituent_tracks());
}

// Destructor
Track::~Track() {
  delete _trackpoints;
  delete _constituent_tracks;
}

// Assignment operator
Track& Track::operator=(const Track &track) {
  if (this == &track) {
    return *this;
  }
  _mapper_name        = track.get_mapper_name();
  _pid                = track.get_pid();
  _charge             = track.get_charge();
  _trackpoints = new TRefArray(*track.get_trackpoints());
  _constituent_tracks = new TRefArray(*track.get_constituent_tracks());
  _detectorpoints     = track.get_detectorpoints();
  _geometry_paths     = track.get_geometry_paths();
  _goodness_of_fit    = track.get_goodness_of_fit();
  
  return *this;
}

// Create a new Track, identical to the original, but separate.  All
// TrackPoints are also cloned.
Track* Track::Clone() const {
  MAUS::GlobalTrack* trackNew =
      new MAUS::GlobalTrack();

  trackNew->set_mapper_name(get_mapper_name());
  trackNew->set_pid(get_pid());
  trackNew->set_charge(get_charge());

  // Track points may be edited, so we clone the original points
  MAUS::GlobalTrackPoint* tp = NULL;
  for(int i = 0; i < _trackpoints->GetLast()+1; ++i) {
    tp = (MAUS::GlobalTrackPoint*) _trackpoints->At(i);
    if(!tp) continue;
    trackNew->PushBackTrackPoint(tp->Clone());
  }

  trackNew->set_detectorpoints(this->get_detectorpoints());
  trackNew->set_geometry_paths(this->get_geometry_paths());

  // This is just book-keeping, so we copy the TRefArray whole.
  MAUS::GlobalTrack* t = NULL;
  for(int i = 0; i < _constituent_tracks->GetLast()+1; ++i) {
    t = (MAUS::GlobalTrack*) _constituent_tracks->At(i);
    if(!t) continue;
    trackNew->AddTrack(t);
  }
  
  trackNew->set_goodness_of_fit(this->get_goodness_of_fit());
  
  return trackNew;
}

// Trackpoint methods

void Track::AddTrackPoint(MAUS::GlobalTrackPoint* trackpoint) {
  if(!trackpoint){
    Squeak::mout(Squeak::error) << "Adding NULL Track Point" << std::endl;
    return;
  }
  if(trackpoint->get_detector() == MAUS::recon::global::kVirtual) {
    AddGeometryPath(trackpoint->get_geometry_path());
  }
  SetDetector(trackpoint->get_detector());
  PushBackTrackPoint(trackpoint);
}
  
void Track::PushBackTrackPoint(MAUS::GlobalTrackPoint* trackpoint) {
  if(trackpoint)
    _trackpoints->Add(trackpoint);
  else
    Squeak::mout(Squeak::error)
        << "recon::global::track - Attempting to add a NULL trackpoint pointer"
        << std::endl;
}

// Correctly remove the trackpoint, unsetting the detector bit and/or
// removing the geometry path if appropriate.
void Track::RemoveTrackPoint(MAUS::GlobalTrackPoint* trackpoint) {
  if(!trackpoint) {
    Squeak::mout(Squeak::error)
        << "recon::global::track - "
        << "Attempting to remove a NULL trackpoint pointer"
        << std::endl;
    return;
  }

  // Remove trackpoint from TRefArray
  TObject* result = _trackpoints->FindObject(trackpoint);
  if(!result)
    Squeak::mout(Squeak::debug)
        << "recon::global::track - "
        << "Trying to remove a trackpoint not stored in track"
        << std::endl;
  else {
    _trackpoints->Remove(result);
    _trackpoints->Compress();
  }

  // Check if trackpoint detector point should still be set.
  MAUS::recon::global::DetectorPoint targetDP = trackpoint->get_detector();
  MAUS::recon::global::TrackPoint *eachTP;
  bool stillNeeded = false;
  for(int i = 0; i < _trackpoints->GetLast()+1; ++i) {
    eachTP = (MAUS::recon::global::TrackPoint*) _trackpoints->At(i);
    if(!eachTP) continue;
    if(eachTP->get_detector() == targetDP) {
      stillNeeded = true;
      break;
    }
  }
  if(!stillNeeded){
    RemoveDetector(targetDP);
  }

  // If we have an associated geometry path, remove that as well
  if(targetDP == MAUS::recon::global::kVirtual) {
    RemoveGeometryPath(trackpoint->get_geometry_path());
  }
}

void Track::SortTrackPointsByZ() {
  MAUS::GlobalTrackPointPArray temp_trackpoints;
  MAUS::GlobalTrackPoint* tp = NULL;
  for(int i = 0; i < _trackpoints->GetLast()+1; ++i) {
    tp = (MAUS::GlobalTrackPoint*) _trackpoints->At(i);
    if(!tp) continue;
    temp_trackpoints.push_back(tp);
  }
  
  std::sort(temp_trackpoints.begin(), temp_trackpoints.end(), SortByZ);

  _trackpoints->Clear();
  _trackpoints->Expand(temp_trackpoints.size()); // Can be used to shrink too
  for(size_t i = 0; i < temp_trackpoints.size(); ++i) {
    _trackpoints->AddAt(temp_trackpoints.at(i), i);
  }
}

// Detector Point Methods
void Track::SetDetector(MAUS::recon::global::DetectorPoint detector) {
  // Set the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  _detectorpoints |= (1u << detector);
}

void Track::RemoveDetector(MAUS::recon::global::DetectorPoint detector) {
  // Clear the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  _detectorpoints &= ~(1u << detector);
}

bool Track::HasDetector(MAUS::recon::global::DetectorPoint detector) {
  // Return the Nth bit of the integer, where N is the value of the
  // DetectorPoint enumerator.
  return (_detectorpoints & (1u << detector));
}

void Track::ClearDetectors() {
  // Set all bits to 0, i.e. set int 0.
  _detectorpoints = 0;
}

std::vector<MAUS::recon::global::DetectorPoint> Track::GetDetectorPoints() {
  std::vector<MAUS::recon::global::DetectorPoint> result;
  MAUS::recon::global::DetectorPoint test;
  for (int i = 0; i < MAUS::recon::global::kDetectorPointSize; ++i) {
    test = static_cast<MAUS::recon::global::DetectorPoint>(i);
    if (HasDetector(test)) result.push_back(test);
  }

  return result;
}

// Geometry Path methods
void Track::AddGeometryPath(std::string geometry_path) {
  _geometry_paths.push_back(geometry_path);
}

void Track::RemoveGeometryPath(std::string geometry_path) {
  std::vector<std::string>::iterator result =
      find(_geometry_paths.begin(), _geometry_paths.end(), geometry_path);

  if(result == _geometry_paths.end()) {
    Squeak::mout(Squeak::debug)
        << "recon::global::track - "
        << "Trying to remove a geometry_path not stored in track"
        << std::endl;
  } else {
    _geometry_paths.erase(result);
  }
}

bool Track::HasGeometryPath(std::string geometry_path) {
  std::vector<std::string>::iterator result =
      find(_geometry_paths.begin(), _geometry_paths.end(), geometry_path);

  return (result != _geometry_paths.end());
}

void Track::ClearGeometryPaths() {
  _geometry_paths.clear();
}

// Constituent Tracks methods
void Track::AddTrack(MAUS::GlobalTrack* track) {
  _constituent_tracks->Add(track);
}

void Track::RemoveTrack(MAUS::GlobalTrack* track) {
  TObject *result = _constituent_tracks->FindObject(track);
  if(!result) {
    Squeak::mout(Squeak::debug)
        << "recon::global::track - "
        << "Trying to remove a constituent track not stored in track"
        << std::endl;
  } else {
    _constituent_tracks->Remove(track);
  }
}

bool Track::HasTrack(MAUS::GlobalTrack* track) {
  TObject *result = _constituent_tracks->FindObject(track);

  return (result != NULL);
}

} // ~namespace global
} // ~namespace recon
} // ~namespace MAUS
