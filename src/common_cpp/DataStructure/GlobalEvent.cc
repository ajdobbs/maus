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
 */

#include <algorithm>
#include <vector>

#include "TRefArray.h"
#include "TProcessID.h"

#include "src/common_cpp/DataStructure/GlobalEvent.hh"

namespace MAUS {

GlobalEvent::GlobalEvent()
    : _primary_chains(NULL), _tracks(NULL),
      _track_points(NULL), _space_points(NULL) {
  _primary_chains =
      new std::vector<MAUS::DataStructure::Global::PrimaryChain*>();
  _tracks        = new std::vector<MAUS::DataStructure::Global::Track*>();
  _track_points   = new std::vector<MAUS::DataStructure::Global::TrackPoint*>();
  _space_points   = new std::vector<MAUS::DataStructure::Global::SpacePoint*>();
}

GlobalEvent::GlobalEvent(const GlobalEvent& globalevent)
    : _primary_chains(NULL), _tracks(NULL),
      _track_points(NULL), _space_points(NULL) {
  *this = globalevent;
}

GlobalEvent& GlobalEvent::operator=(const GlobalEvent& globalevent) {
  if (this == &globalevent) {
    return *this;
  }

  if (_space_points != NULL) {
    for (size_t i = 0; i < _space_points->size(); ++i)
      delete _space_points->at(i);
    delete _space_points;
  }
  if (globalevent._space_points == NULL) {
    _space_points = NULL;
  } else {
    _space_points = new std::vector<MAUS::DataStructure::Global::SpacePoint*>();
    std::vector<MAUS::DataStructure::Global::SpacePoint*>* old_space_points =
        globalevent._space_points;
    for (size_t i = 0; i < old_space_points->size(); ++i) {
      DataStructure::Global::SpacePoint* sp =
          new DataStructure::Global::SpacePoint(*old_space_points->at(i));
      _space_points->push_back(sp);
    }
  }

  if (_track_points != NULL) {
    for (size_t i = 0; i < _track_points->size(); ++i)
      delete _track_points->at(i);
    delete _track_points;
  }
  if (globalevent._track_points == NULL) {
    _track_points = NULL;
  } else {
    _track_points = new std::vector<MAUS::DataStructure::Global::TrackPoint*>();
    std::vector<MAUS::DataStructure::Global::TrackPoint*>* old_track_points =
        globalevent._track_points;
    for (size_t i = 0; i < old_track_points->size(); ++i) {
      DataStructure::Global::TrackPoint* tp =
          new DataStructure::Global::TrackPoint(*old_track_points->at(i));
      _track_points->push_back(tp);
      // copy across also the space point pointer-as-reference to the new
      // structure
      MAUS::DataStructure::Global::SpacePoint* sp =
          _track_points->at(i)->get_space_point();
      if (!sp)
          continue;
      for (size_t j = 0; j < globalevent._space_points->size(); ++j) {
          if (globalevent._space_points->at(j) == sp) {
              _track_points->at(i)->set_space_point(_space_points->at(j));
              break;
          }
      }
    }
  }

  if (_tracks != NULL) {
    for (size_t i = 0; i < _tracks->size(); ++i)
      delete _tracks->at(i);
    delete _tracks;
  }
  if (globalevent._tracks == NULL) {
    _tracks = NULL;
  } else {
    _tracks = new std::vector<MAUS::DataStructure::Global::Track*>();
    std::vector<MAUS::DataStructure::Global::Track*>* old_tracks =
        globalevent._tracks;
    for (size_t i = 0; i < old_tracks->size(); ++i) {
      DataStructure::Global::Track* temp_track =
          new DataStructure::Global::Track(*old_tracks->at(i));
      _tracks->push_back(temp_track);

      TRefArray* old_track_points_on_track = old_tracks->at(i)->get_track_points();
      TRefArray* new_track_points_on_track = new TRefArray(
          TProcessID::GetProcessWithUID(old_track_points_on_track));
      new_track_points_on_track->Expand(old_track_points_on_track->GetSize());
      for (int j = 0; j < old_track_points_on_track->GetLast()+1; ++j) {
        for (size_t k = 0; k < globalevent._track_points->size(); ++k) {
          if (old_track_points_on_track->At(j) == globalevent._track_points->at(k)) {
            new_track_points_on_track->AddAt(_track_points->at(k), j);
            break;
          }
        }
      }

      TRefArray* old_constituent_tracks = old_tracks->at(i)->get_constituent_tracks();
      TRefArray* new_constituent_tracks = new TRefArray(
          TProcessID::GetProcessWithUID(old_constituent_tracks));
      new_constituent_tracks->Expand(old_constituent_tracks->GetSize());
      for (int j = 0; j < old_constituent_tracks->GetLast()+1; ++j) {
        for (size_t k = 0; k < globalevent._tracks->size(); ++k) {
          if (old_constituent_tracks->At(j) == globalevent._tracks->at(k)) {
            new_constituent_tracks->AddAt(_tracks->at(k), j);
            break;
          }
        }
      }

      _tracks->at(i)->set_track_points(new_track_points_on_track);
      _tracks->at(i)->set_constituent_tracks(new_constituent_tracks);
    }
  }

  // This is a deep copy operation; we go through, deepcopying data and
  // processing any cross links (pointer-as-reference) so that they point into
  // the new data structure
  if (_primary_chains != NULL) {
    if (!_primary_chains->empty())
      for (size_t i = 0; i < _primary_chains->size(); ++i)
        delete _primary_chains->at(i);
    delete _primary_chains;
  }
  if (globalevent._primary_chains == NULL) {
    _primary_chains = NULL;
  } else {
    _primary_chains =
        new std::vector<MAUS::DataStructure::Global::PrimaryChain*>();
    std::vector<MAUS::DataStructure::Global::PrimaryChain*>* old_primary_chains =
        globalevent._primary_chains;
    for (size_t i = 0; i < old_primary_chains->size(); ++i) {
      DataStructure::Global::PrimaryChain* pchain =
          new DataStructure::Global::PrimaryChain(*old_primary_chains->at(i));
      _primary_chains->push_back(pchain);
      TRefArray* old_tracks = old_primary_chains->at(i)->get_tracks();
      TRefArray* new_tracks = new TRefArray(TProcessID::GetProcessWithUID(old_tracks));
      new_tracks->Expand(old_tracks->GetSize());
      for (int j = 0; j < old_tracks->GetLast()+1; j++) {
        for (size_t k = 0; k < globalevent._tracks->size(); k++) {
          if (old_tracks->At(j) == globalevent._tracks->at(k)) {
            new_tracks->AddAt(_tracks->at(k), j);
            break;
          }
        }
      }
      _primary_chains->at(i)->set_tracks(new_tracks);
      MAUS::DataStructure::Global::PrimaryChain* us_daughter_chain =
          _primary_chains->at(i)->GetUSDaughter();
      MAUS::DataStructure::Global::PrimaryChain* ds_daughter_chain =
          _primary_chains->at(i)->GetDSDaughter();
      // A chain will always either have two or no daughters, never one
      if (!us_daughter_chain) {
        continue;
      }
      for (size_t j = 0; j < globalevent._primary_chains->size(); j++) {
        if (globalevent._primary_chains->at(j) == us_daughter_chain) {
          _primary_chains->at(i)->SetUSDaughter(_primary_chains->at(j));
        } else if (globalevent._primary_chains->at(j) == ds_daughter_chain) {
          _primary_chains->at(i)->SetDSDaughter(_primary_chains->at(j));
        }
      }
    }
  }
  return *this;
}

GlobalEvent::~GlobalEvent() {
  if (_primary_chains != NULL) {
    for (size_t i = 0; i < _primary_chains->size(); ++i) {
      delete _primary_chains->at(i);
    }
    delete _primary_chains;
  }

  if (_tracks != NULL) {
    for (size_t i = 0; i < _tracks->size(); ++i) {
      delete _tracks->at(i);
    }
    delete _tracks;
  }

  if (_track_points != NULL) {
    for (size_t i = 0; i < _track_points->size(); ++i) {
      delete _track_points->at(i);
    }
    delete _track_points;
  }

  if (_space_points != NULL) {
    for (size_t i = 0; i < _space_points->size(); ++i) {
      delete _space_points->at(i);
    }
    delete _space_points;
  }
}

void GlobalEvent::add_primary_chain(
    MAUS::DataStructure::Global::PrimaryChain* pchain) {
  _primary_chains->push_back(pchain);
};

std::vector<MAUS::DataStructure::Global::PrimaryChain*>*
GlobalEvent::get_primary_chains() const {
  return _primary_chains;
};

void GlobalEvent::set_primary_chains(
    std::vector<MAUS::DataStructure::Global::PrimaryChain*> *primary_chains) {
  _primary_chains = primary_chains;
};

void GlobalEvent::add_track(MAUS::DataStructure::Global::Track* track) {
  _tracks->push_back(track);
};

bool GlobalEvent::add_track_check(MAUS::DataStructure::Global::Track* track) {
  // Check if the provided track matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  std::vector<MAUS::DataStructure::Global::Track*>::iterator track_iter =
      std::find(_tracks->begin(), _tracks->end(), track);

  bool exists = (track_iter != _tracks->end());
  if (!exists)
    add_track(track);

  return exists;
}

void GlobalEvent::add_track_recursive(
    MAUS::DataStructure::Global::Track* track) {
  // Add the Track, checking if it already exists in the GlobalEvent.
  bool already_added = add_track_check(track);
  // If the chain had been added, then we will loop over the
  // constituent tracks and track_points, adding them too.
  if (!already_added) {
    // Constituent Tracks
    MAUS::DataStructure::Global::Track* ct = NULL;
    for (int i = 0; i < track->get_constituent_tracks()->GetLast() + 1; ++i) {
      ct = (MAUS::DataStructure::Global::Track*)
          track->get_constituent_tracks()->At(i);
      if (!ct) continue;
      add_track_recursive(ct);
    }

    // TrackPoints
    MAUS::DataStructure::Global::TrackPoint* tp = NULL;
    for (int i = 0; i < track->get_track_points()->GetLast() + 1; ++i) {
      tp = (MAUS::DataStructure::Global::TrackPoint*)
          track->get_track_points()->At(i);
      if (!tp) continue;
      add_track_point_recursive(tp);
    }
  }
}

std::vector<MAUS::DataStructure::Global::Track*>*
GlobalEvent::get_tracks() const {
  return _tracks;
};

void GlobalEvent::set_tracks(
    std::vector<MAUS::DataStructure::Global::Track*> *tracks) {
  _tracks = tracks;
};

void GlobalEvent::add_track_point(
    MAUS::DataStructure::Global::TrackPoint* track_point) {
  _track_points->push_back(track_point);
};

bool GlobalEvent::add_track_point_check(
    MAUS::DataStructure::Global::TrackPoint* track_point) {
  // Check if the provided track_point matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  std::vector<MAUS::DataStructure::Global::TrackPoint*>::iterator
      track_point_iter = std::find(_track_points->begin(),
                                  _track_points->end(),
                                  track_point);

  bool exists = (track_point_iter != _track_points->end());
  if (!exists)
    add_track_point(track_point);

  return exists;
}

void GlobalEvent::add_track_point_recursive(
    MAUS::DataStructure::Global::TrackPoint* track_point) {
  // Add the TrackPoint, checking if it already exists in the GlobalEvent.
  bool already_added = add_track_point_check(track_point);

  // If the chain had been added, then we will add the constituent
  // space_point.
  if (!already_added) {
    // SpacePoints
    add_space_point_check(track_point->get_space_point());
  }
}

std::vector<MAUS::DataStructure::Global::TrackPoint*>*
GlobalEvent::get_track_points() const {
  return _track_points;
};

void GlobalEvent::set_track_points(
    std::vector<MAUS::DataStructure::Global::TrackPoint*> *track_points) {
  _track_points = track_points;
};

void GlobalEvent::add_space_point(
    MAUS::DataStructure::Global::SpacePoint* space_point) {
  _space_points->push_back(space_point);
};

bool GlobalEvent::add_space_point_check(
    MAUS::DataStructure::Global::SpacePoint* space_point) {
  // Check if the provided space_point matches an existing entry (this is
  // to save repeated entries and wasted disk space).
  bool exists = false;

  for (unsigned int i = 0; i < _space_points->size(); ++i) {
    MAUS::DataStructure::Global::SpacePoint* test = _space_points->at(i);
    if (test == space_point) {
      exists = true;
      break;
    }
  }

  if (!exists)
    add_space_point(space_point);

  return exists;
}

std::vector<MAUS::DataStructure::Global::SpacePoint*>*
GlobalEvent::get_space_points() const {
  return _space_points;
};

void GlobalEvent::set_space_points(
    std::vector<MAUS::DataStructure::Global::SpacePoint*> *space_points) {
  _space_points = space_points;
};

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
    GlobalEvent::GetPrimaryChains(MAUS::DataStructure::Global::ChainType chain_type) const {
  std::vector<MAUS::DataStructure::Global::PrimaryChain*> chains;
  for (size_t i = 0; i < _primary_chains->size(); i++) {
    if (_primary_chains->at(i)->get_chain_type() == chain_type) {
      chains.push_back(_primary_chains->at(i));
    }
  }
  return chains;
}

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
    GlobalEvent::GetUSPrimaryChains() const {
  return GetPrimaryChains(MAUS::DataStructure::Global::kUS);
}

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
    GlobalEvent::GetDSPrimaryChains() const {
  return GetPrimaryChains(MAUS::DataStructure::Global::kDS);
}

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
    GlobalEvent::GetThroughPrimaryChains() const {
  return GetPrimaryChains(MAUS::DataStructure::Global::kThrough);
}

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
    GlobalEvent::GetPrimaryChainOrphans() const {
  std::vector<MAUS::DataStructure::Global::PrimaryChain*> orphan_chains;
  for (size_t i = 0; i < _primary_chains->size(); i++) {
    MAUS::DataStructure::Global::ChainType chain_type = _primary_chains->at(i)->get_chain_type();
    if (chain_type == MAUS::DataStructure::Global::kUSOrphan or
        chain_type == MAUS::DataStructure::Global::kDSOrphan) {
      orphan_chains.push_back(_primary_chains->at(i));
    }
  }
  return orphan_chains;
}

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
    GlobalEvent::GetUSPrimaryChainOrphans() const {
  return GetPrimaryChains(MAUS::DataStructure::Global::kUSOrphan);
}

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
    GlobalEvent::GetDSPrimaryChainOrphans() const {
  return GetPrimaryChains(MAUS::DataStructure::Global::kDSOrphan);
}

std::vector<MAUS::DataStructure::Global::PrimaryChain*>
    GlobalEvent::GetNonThroughPrimaryChains() const {
  std::vector<MAUS::DataStructure::Global::PrimaryChain*> segment_chains;
  for (size_t i = 0; i < _primary_chains->size(); i++) {
    MAUS::DataStructure::Global::ChainType chain_type = _primary_chains->at(i)->get_chain_type();
    if (chain_type == MAUS::DataStructure::Global::kUSOrphan or
        chain_type == MAUS::DataStructure::Global::kDSOrphan or
        chain_type == MAUS::DataStructure::Global::kUS or
        chain_type == MAUS::DataStructure::Global::kDS) {
      segment_chains.push_back(_primary_chains->at(i));
    }
  }
  return segment_chains;
}

std::vector<MAUS::DataStructure::Global::Track*> GlobalEvent::GetLRTracks() const {
  std::vector<MAUS::DataStructure::Global::Track*> lr_tracks;
  for (size_t i = 0; i < _tracks->size(); i++) {
    if (_tracks->at(i)->get_mapper_name() == "MapCppGlobalReconImport") {
      lr_tracks.push_back(_tracks->at(i));
    }
  }
  return lr_tracks;
}

std::vector<MAUS::DataStructure::Global::Track*>
    GlobalEvent::GetLRTracks(MAUS::DataStructure::Global::DetectorPoint detector) const {
  std::vector<MAUS::DataStructure::Global::Track*> lr_tracks;
  for (size_t i = 0; i < _tracks->size(); i++) {
    if (_tracks->at(i)->get_mapper_name() == "MapCppGlobalReconImport" and
        _tracks->at(i)->HasDetector(detector)) {
      lr_tracks.push_back(_tracks->at(i));
    }
  }
  return lr_tracks;
}

std::vector<MAUS::DataStructure::Global::SpacePoint*> GlobalEvent::GetLRSpacePoints() const {
  std::vector<MAUS::DataStructure::Global::SpacePoint*> lr_spacepoints;
  for (size_t i = 0; i < _space_points->size(); i++) {
    if (_space_points->at(i)->get_mapper_name() == "MapCppGlobalReconImport") {
      lr_spacepoints.push_back(_space_points->at(i));
    }
  }
  return lr_spacepoints;
}

std::vector<MAUS::DataStructure::Global::SpacePoint*>
    GlobalEvent::GetLRSpacePoints(MAUS::DataStructure::Global::DetectorPoint detector) const {
  std::vector<MAUS::DataStructure::Global::SpacePoint*> lr_spacepoints;
  for (size_t i = 0; i < _space_points->size(); i++) {
    if (_space_points->at(i)->get_mapper_name() == "MapCppGlobalReconImport" and
        _space_points->at(i)->get_detector() == detector) {
      lr_spacepoints.push_back(_space_points->at(i));
    }
  }
  return lr_spacepoints;
}
}
