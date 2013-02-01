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

#include "src/common_cpp/DataStructure/SciFiTrack.hh"


namespace MAUS {

SciFiTrack::SciFiTrack(): _tracker(-1), _f_chi2(-1), _s_chi2(-1), _ndf(-1), _P_value(-1) {}

SciFiTrack::SciFiTrack(const SciFiTrack &a_track): _tracker(-1),
                                                   _f_chi2(-1),
                                                   _s_chi2(-1),
                                                   _ndf(-1),
                                                   _P_value(-1) {
  _tracker = a_track.get_tracker();
  _f_chi2    = a_track.get_f_chi2();
  _s_chi2    = a_track.get_s_chi2();
  _ndf     = a_track.get_ndf();
  _P_value = a_track.get_P_value();
}

SciFiTrack::SciFiTrack(const KalmanTrack *kalman_track): _tracker(-1),
                                                  _f_chi2(-1),
                                                  _s_chi2(-1),
                                                  _ndf(-1),
                                                  _P_value(-1) {
  _tracker = kalman_track->get_tracker();
  _f_chi2    = kalman_track->get_f_chi2();
  _s_chi2    = kalman_track->get_s_chi2();
  _ndf     = kalman_track->get_ndf();
  _P_value = kalman_track->get_P_value();
}

SciFiTrack& SciFiTrack::operator=(const SciFiTrack &a_track) {
    if (this == &a_track) {
        return *this;
    }
    _tracker = a_track.get_tracker();
    _f_chi2    = a_track.get_f_chi2();
    _s_chi2    = a_track.get_s_chi2();
    _ndf     = a_track.get_ndf();
    _P_value = a_track.get_P_value();
    return *this;
}

SciFiTrack::~SciFiTrack() {}

/*
void SciFiTrack::add_track_points(const std::vector<KalmanSite> &sites) {
  size_t n_sites = sites.size();
  for ( size_t i = 0; i < n_sites; ++i ) {
    const KalmanSite site = sites[i];
    double time = 0.0;
    TMatrixD state_vector(5, 1);
    state_vector = site->get_smoothed_state();
    double energy = 1.0;
    double x = state_vector(0, 0);
    double px= state_vector(1, 0);
    double y = state_vector(2, 0);
    double py= state_vector(3, 0);
    Detector & detector
    double z = site.get_z();
    recon::global::TrackPoint track_point = 
  }

  Detector(const ID id, const double plane, const CovarianceMatrix & uncertainties);
}
*/

} // ~namespace MAUS
