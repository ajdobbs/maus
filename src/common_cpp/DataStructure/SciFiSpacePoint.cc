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

#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"

namespace MAUS {

// Default constructor
SciFiSpacePoint::SciFiSpacePoint(): _used(false),
                                    _add_on(false),
                                    _spill(0),
                                    _event(0),
                                    _tracker(0),
                                    _station(0),
                                    _time(0.),
                                    _time_error(0.),
                                    _time_res(0.),
                                    _npe(0.),
                                    _chi2(0.),
                                    _prxy_pull(0.),
                                    _type(""),
                                    _position(ThreeVector(0, 0, 0)),
                                    _global_position(ThreeVector(0, 0, 0)) {
  _channels = new TRefArray();
}

// Copy contructor
SciFiSpacePoint::SciFiSpacePoint(const SciFiSpacePoint &sp)
    : _used(false),
      _add_on(false),
      _spill(0),
      _event(0),
      _tracker(0),
      _station(0),
      _time(0.),
      _time_error(0.),
      _time_res(0.),
      _npe(0.),
      _chi2(0.),
      _prxy_pull(0.),
      _type(""),
      _position(ThreeVector(0, 0, 0)),
      _global_position(ThreeVector(0, 0, 0)) {
  _used      = sp.is_used();
  _add_on    = sp.get_add_on();
  _spill     = sp.get_spill();
  _event     = sp.get_event();
  _tracker   = sp.get_tracker();
  _station   = sp.get_station();
  _time      = sp.get_time();
  _time_error= sp.get_time_error();
  _time_res  = sp.get_time_res();
  _npe       = sp.get_npe();
  _chi2      = sp.get_chi2();
  _prxy_pull = sp.get_prxy_pull();
  _type      = sp.get_type();
  _position  = sp.get_position();
  _global_position = sp.get_global_position();
  _channels = new TRefArray(*(sp.get_channels()));
}

// Three cluster constructor
SciFiSpacePoint::SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2, SciFiCluster *clust3)
    : _used(false),
      _add_on(false),
      _time(0.0),
      _chi2(0.0),
      _prxy_pull(0.0),
      _position(ThreeVector(0, 0, 0)) {

  _type = "triplet";
  clust1->set_used(true);
  clust2->set_used(true);
  clust3->set_used(true);

  _channels = new TRefArray();
  _channels->Add(clust1);
  _channels->Add(clust2);
  _channels->Add(clust3);

  _spill   = clust1->get_spill();
  _event   = clust1->get_event();
  _npe = clust1->get_npe()+clust2->get_npe()+clust3->get_npe();
  _tracker = clust1->get_tracker();
  _station = clust1->get_station();
}

// Two cluster constructor
SciFiSpacePoint::SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2)
    : _used(false),
      _add_on(false),
      _time(0.0),
      _chi2(0.0),
      _prxy_pull(0.0),
      _position(ThreeVector(0, 0, 0)) {

  _type = "duplet";
  clust1->set_used(true);
  clust2->set_used(true);

  _channels = new TRefArray();
  _channels->Add(clust1);
  _channels->Add(clust2);

  _spill   = clust1->get_spill();
  _event   = clust1->get_event();
  _tracker = clust1->get_tracker();
  _station = clust1->get_station();
  _npe = clust1->get_npe()+clust2->get_npe();
}

SciFiSpacePoint::~SciFiSpacePoint() {
  delete _channels;
}

SciFiSpacePoint& SciFiSpacePoint::operator=(const SciFiSpacePoint &sp) {
  if (this == &sp) {
    return *this;
  }
  _used      = sp.is_used();
  _add_on    = sp.get_add_on();
  _spill     = sp.get_spill();
  _event     = sp.get_event();
  _tracker   = sp.get_tracker();
  _station   = sp.get_station();
  _time      = sp.get_time();
  _time_error= sp.get_time_error();
  _time_res  = sp.get_time_res();
  _npe       = sp.get_npe();
  _chi2      = sp.get_chi2();
  _prxy_pull = sp.get_prxy_pull();
  _type      = sp.get_type();
  _position  = sp.get_position();
  _global_position  = sp.get_global_position();

  if (_channels) delete _channels;
  _channels = new TRefArray(*(sp.get_channels()));

  return *this;
}

void SciFiSpacePoint::add_channel(SciFiCluster *channel) {
  _channels->Add(channel);
}

SciFiClusterPArray SciFiSpacePoint::get_channels_pointers() const {
  SciFiClusterPArray cl_pointers;

  // Check the _spoints container is not initialised
  if (!_channels) {
    std::cerr << "Cluster TRefArray not initialised" << std::endl;
    return cl_pointers;
  }

  for (int i = 0; i < (_channels->GetLast()+1); ++i) {
    cl_pointers.push_back(static_cast<SciFiCluster*>(_channels->At(i)));
  }
  return cl_pointers;
}

void SciFiSpacePoint::set_channels_pointers(const SciFiClusterPArray &channels) {
  if (_channels) delete _channels;
  _channels = new TRefArray();
  for (
    std::vector<SciFiCluster*>::const_iterator cl = channels.begin(); cl != channels.end(); ++cl) {
    _channels->Add(*cl);
  }
}

} // ~namespace MAUS
