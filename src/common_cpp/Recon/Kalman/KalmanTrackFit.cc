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

#include "src/common_cpp/Recon/Kalman/KalmanTrackFit.hh"
#include <math.h>
#include "Interface/Squeal.hh"
// #define PI 3.14159265359

namespace MAUS {

KalmanTrackFit::KalmanTrackFit() {
  Json::Value *json = Globals::GetConfigurationCards();
  _seed_cov  = (*json)["SciFiSeedCovariance"].asDouble();
  _use_MCS   = (*json)["SciFiKalman_use_MCS"].asBool();
  _use_Eloss = (*json)["SciFiKalman_use_Eloss"].asBool();
  _update_misalignments = (*json)["SciFiUpdateMisalignments"].asBool();

  std::cerr << "---------------------Birth of Kalman Filter--------------------" << std::endl;
}

KalmanTrackFit::~KalmanTrackFit() {
  std::cerr << "---------------------Death of Kalman Filter--------------------" << std::endl;
}

void KalmanTrackFit::process(std::vector<KalmanSeed*> seeds, SciFiEvent &event) {
  KalmanMonitor monitor;
  KalmanSciFiAlignment kalman_align;
  kalman_align.load_misaligments();

  size_t num_tracks = seeds.size();
  for ( size_t i = 0; i < num_tracks; ++i ) {
    KalmanSeed* seed = seeds[i];
    std::vector<KalmanSite> sites;
    initialise(seed, sites, kalman_align);

    // std::cerr << "Number of sites: " << sites.size() << "\n";
    KalmanTrack *track = 0;
    if ( seed->is_straight() ) {
      track = new StraightTrack();
    } else if ( seed->is_helical() ) {
      track = new HelicalTrack();
    }
    // muon assumption for now.
    double muon_mass    = 105.7; // MeV/c
    track->set_mass(muon_mass);
    double momentum = seed->get_momentum(); // MeV/c
    track->set_momentum(momentum);
    // Filter the first state.
    // std::cerr << "Filtering site 0" << std::endl;
    filter(sites, track, 0);

    size_t numb_measurements = sites.size();

    if ( numb_measurements != 15 ) continue;
    for ( size_t j = 1; j < numb_measurements; ++j ) {
      // Predict the state vector at site i...
      // std::cerr << "Extrapolating to site " << j << std::endl;
      extrapolate(sites, track, j);
      // ... Filter...
      // std::cerr << "Filtering site " << j << std::endl;
      filter(sites, track, j);
    }

    track->prepare_for_smoothing(sites);
    // ...and Smooth back all sites.
    for ( int k = static_cast<int> (numb_measurements-2); k > -1; --k ) {
      // std::cerr << "Smoothing site " << k << std::endl;
      smooth(sites, track, k);
    }

    monitor.fill(sites);
    // monitor.print_info(sites);
    track->compute_chi2(sites);
    // Misalignment work.
    if ( track->get_chi2() < 25. && numb_measurements == 15 ) {
      for ( size_t j = 0; j < numb_measurements; ++j ) {
        KalmanSite *site = &sites[j];
        track->exclude_site(site);
        kalman_align.update_site(site);
      }
      // update_alignment_parameters(sites, track, kalman_align);
      // std::cerr << "Updating..." << std::endl;
      // Update Stored misalignments using values stored in each site.
      // kalman_align.update(sites);
    }
    save(track, sites, event);
    delete track;
  }
}

void KalmanTrackFit::update_alignment_parameters(std::vector<KalmanSite> &sites,
                                                 KalmanTrack *track,
                                                 KalmanSciFiAlignment &kalman_align) {
  size_t numb_measurements = sites.size();

  for ( size_t i = 0; i < numb_measurements; ++i ) {
    // ... Filter...
    std::cerr << "Updating site " << i << std::endl;
    filter_updating_misalignments(sites, track, i);
  }
}

void KalmanTrackFit::initialise(KalmanSeed *seed,
                                std::vector<KalmanSite> &sites,
                                KalmanSciFiAlignment &kalman_align) {
  TMatrixD a0(5, 1);
  a0 = seed->get_initial_state_vector();

  TMatrixD C(5, 5);
  C.Zero();
  // for ( int i = 0; i < 5; ++i ) {
  C(0, 0) = _seed_cov/50.; // dummy values
  C(1, 1) = _seed_cov; // dummy values
  C(2, 2) = _seed_cov/50.; // dummy values
  C(3, 3) = _seed_cov; // dummy values
  C(4, 4) = _seed_cov; // dummy values

  std::vector<SciFiCluster*> clusters = seed->get_clusters();

  KalmanSite first_plane;
  first_plane.set_projected_a(a0);
  first_plane.set_projected_covariance_matrix(C);
  first_plane.set_measurement(clusters[0]->get_alpha());
  first_plane.set_direction(clusters[0]->get_direction());
  first_plane.set_z(clusters[0]->get_position().z());
  int id_0 = clusters[0]->get_id();
  first_plane.set_id(id_0);
  first_plane.set_type(2);
  first_plane.set_shifts(kalman_align.get_shifts(id_0));
  first_plane.set_S_covariance(kalman_align.get_cov_shifts(id_0));
  sites.push_back(first_plane);

  size_t numb_sites = clusters.size();
  for ( size_t j = 1; j < numb_sites; ++j ) {
    KalmanSite a_site;
    a_site.set_measurement(clusters[j]->get_alpha());
    a_site.set_direction(clusters[j]->get_direction());
    a_site.set_z(clusters[j]->get_position().z());
    int id = clusters[j]->get_id();
    a_site.set_id(id);
    a_site.set_type(2);
    a_site.set_shifts(kalman_align.get_shifts(id));
    a_site.set_S_covariance(kalman_align.get_cov_shifts(id));
    sites.push_back(a_site);
  }

  for ( size_t j = 0; j < numb_sites; ++j ) {
    Hep3Vector true_position = clusters[j]->get_true_position();
    Hep3Vector true_momentum = clusters[j]->get_true_momentum();
    sites[j].set_true_position(true_position);
    sites[j].set_true_momentum(true_momentum);
  }
}

//
// General purpose routines.
//
void KalmanTrackFit::filter(std::vector<KalmanSite> &sites,
                            KalmanTrack *track, int current_site) {
  // Get Site...
  KalmanSite *a_site = &sites[current_site];

  // Update measurement error:
  // (non-const std for the perp direction)
  track->update_V(a_site);

  // Update H (depends on plane direction.)
  track->update_H(a_site);

  // a_k = a_k^k-1 + K_k x pull
  track->calc_filtered_state(a_site);
  // Cp = (C-KHC)
  track->update_covariance(a_site);
}

void KalmanTrackFit::filter_updating_misalignments(std::vector<KalmanSite> &sites,
                            KalmanTrack *track, int current_site) {
  // Get Site...
  KalmanSite *a_site = &sites[current_site];
  KalmanSite *alignment_projection_site = NULL;
  // Get previous site too.
  // if ( !(current_site%3) ) {
  int id = a_site->get_id();
  std::cout << id << std::endl;
  if ( !(current_site%3) ) {
    alignment_projection_site = a_site;
    alignment_projection_site->get_shifts().Print();
  } else {
    alignment_projection_site = &sites[current_site-1];
    alignment_projection_site->get_shifts().Print();
  }

  track->update_V(a_site);
  track->update_H(a_site);
  track->update_W(a_site);
  track->update_misaligments(a_site, alignment_projection_site);
}

void KalmanTrackFit::extrapolate(std::vector<KalmanSite> &sites, KalmanTrack *track, int i) {
  // Get current site...
  KalmanSite *new_site = &sites[i];

  // ... and the site we will extrapolate from.
  KalmanSite *old_site = &sites[i-1];

  // The propagator matrix...
  track->update_propagator(old_site, new_site);

  // Now, calculate prediction.
  track->calc_predicted_state(old_site, new_site);

  // Calculate the energy loss for the projected state.
  if ( _use_Eloss )
    track->subtract_energy_loss(old_site, new_site);


  // Calculate the system noise...
  if ( _use_MCS )
    track->calc_system_noise(old_site, new_site);

  // ... so that we can compute the prediction for the
  // covariance matrix.
  track->calc_covariance(old_site, new_site);
}

void KalmanTrackFit::smooth(std::vector<KalmanSite> &sites, KalmanTrack *track, int id) {
  // Get site to be smoothed...
  KalmanSite *smoothing_site = &sites[id];

  // ... and the already perfected site.
  KalmanSite *optimum_site = &sites[id+1];

  // Set the propagator right.
  track->update_propagator(optimum_site, smoothing_site);

  // Compute A_k.
  track->update_back_transportation_matrix(optimum_site, smoothing_site);

  // Compute smoothed a_k and C_k.
  track->smooth_back(optimum_site, smoothing_site);
}

void KalmanTrackFit::save(const KalmanTrack *kalman_track,
                          std::vector<KalmanSite> sites,
                          SciFiEvent &event) {
  SciFiTrack *track = new SciFiTrack(kalman_track);
  // track->add_track_points(sites);
  event.add_scifitrack(track);
}

} // ~namespace MAUS
