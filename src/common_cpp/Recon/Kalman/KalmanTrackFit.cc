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

KalmanTrackFit::KalmanTrackFit():_seed_cov(1000.) {
  std::cerr << "---------------------Birth of Kalman Filter--------------------" << std::endl;
}

KalmanTrackFit::~KalmanTrackFit() {
  std::cerr << "---------------------Death of Kalman Filter--------------------" << std::endl;
}

//
// Straight track fit.
//
void KalmanTrackFit::process(std::vector<KalmanSeed*> seeds) {
  unsigned int num_tracks = seeds.size();
  KalmanMonitor monitor;
  KalmanSciFiAlignment kalman_align;
  kalman_align.load_misaligments();

  for ( unsigned int i = 0; i < num_tracks; ++i ) {
    KalmanSeed* seed = seeds[i];
    std::vector<KalmanSite> sites;
    initialise(seed, sites, kalman_align);

    // std::cerr << "Number of sites: " << sites.size() << "\n";
    KalmanTrack *track = 0;
    if ( seed.is_straight() ) {
      track = new StraightTrack();
    } else if ( seed.is_helical() ) {
      track = new HelicalTrack();
    }
    // muon assumption for now.
    double muon_mass    = 105.7; // MeV/c
    track->set_mass(muon_mass);
    double momentum     = 200.; // MeV/c
    track->set_momentum(momentum);
    // Filter the first state.
    // std::cerr << "Filtering site 0" << std::endl;
    filter(sites, track, 0);

    unsigned int numb_measurements = sites.size();

    assert(numb_measurements < 16);

    for ( unsigned int j = 1; j < numb_measurements; ++j ) {
      // Predict the state vector at site i...
      // std::cerr << "Extrapolating to site " << i << std::endl;
      extrapolate(sites, track, j);
      // ... Filter...
      // std::cerr << "Filtering site " << i << std::endl;
      filter(sites, track, j);
    }

    track->prepare_for_smoothing(sites);
    // ...and Smooth back all sites.
    for ( unsigned int j = numb_measurements-2; j > -1; --j ) {
      // std::cerr << "Smoothing site " << i << std::endl;
      smooth(sites, track, j);
    }
    track->compute_chi2(sites);

    monitor.fill(sites);
    // monitor.print_info(sites);

    if ( 0 && track->get_chi2() < 15. && numb_measurements == 15 ) {
      std::cerr << "Good chi2; lauching KalmanAlignment...\n";
      update_alignment_parameters(sites, track, kalman_align);
      // Update Stored misalignments using values stored in each site.
      kalman_align.update(sites);
    }
    delete track;
  }
}

void KalmanTrackFit::update_alignment_parameters(std::vector<KalmanSite> &sites,
                                                 KalmanTrack *track,
                                                 KalmanSciFiAlignment &kalman_align) {
  unsigned int numb_measurements = sites.size();

  for ( int i = 0; i < numb_measurements; ++i ) {
    // ... Filter...
    // std::cerr << "Updating site " << i << std::endl;
    filter_updating_misalignments(sites, track, i);
  }
}

//
// Helical track fit.
//
/*
void KalmanTrackFit::process(std::vector<SciFiHelicalPRTrack*> helical_tracks) {
  unsigned int num_tracks  = helical_tracks.size();
  KalmanMonitor monitor;

  for ( int i = 0; i < num_tracks; ++i ) {
    std::vector<KalmanSite> sites;

    SciFiHelicalPRTrack* seed = helical_tracks[i];
    std::vector<SciFiCluster*> clusters;
    std::vector<SciFiSpacePoint> spacepoints;
    for ( size_t i = 0; i < seed->get_spacepoints().size(); ++i ) {
      SciFiSpacePoint sp = *seed->get_spacepoints()[i];
      spacepoints.push_back(sp);
    }
    double pz_from_timing;
    process_clusters(spacepoints, clusters, pz_from_timing);

    double momentum;
    initialise(seed, sites, momentum);

  // Get seed values.
  double r  = seed->get_R();
  double B = -4.;
  double pt = -0.3*B*r;

  double dsdz  = seed->get_dsdz();
  double tan_lambda = 1./dsdz;
  // PR doesnt see Eloss, so overstimates pz.
  // Total Eloss = 2 MeV/c.
  double pz = pt*tan_lambda - 2./sqrt(12.);
  double seed_pz;

  momentum = pow(pt*pt+pz*pz, 0.5);
  double kappa = fabs(1./pz);

  int numb_sites = clusters.size();

  int tracker = clusters[0]->get_tracker();

  double pi = acos(-1.);
  double phi_0 = seed->get_phi0();
  double phi = phi_0 + pi/2.;
  double px  = pt*cos(phi);
  double py  = pt*sin(phi);

  double x = spacepoints[0].get_position().x();
  double y = spacepoints[0].get_position().y();

  TMatrixD a(5, 1);
  a(0, 0) = x;
  a(1, 0) = px;
  a(2, 0) = y;
  a(3, 0) = py;
  a(4, 0) = kappa;
///////////////////////////////////

    KalmanTrack *track = new HelicalTrack();


    // muon assumption for now.
    double muon_mass    = 105.7; // MeV/c
    track->set_mass(muon_mass);
    track->set_momentum(momentum);
    // Filter the first state.
    // std::cerr << "Filtering site 0" << std::endl;
    filter(sites, track, 0);

    unsigned int numb_measurements = sites.size();

    assert(numb_measurements < 16);

    for ( unsigned int j = 1; j < numb_measurements; ++j ) {
      // Predict the state vector at site i...
      // std::cerr << "Extrapolating to site " << i << std::endl;
      extrapolate(sites, track, j);
      // ... Filter...
      // std::cerr << "Filtering site " << j << std::endl;
      filter(sites, track, j);
    }

    track->prepare_for_smoothing(sites);
    // ...and Smooth back all sites.
    for ( unsigned int j = numb_measurements-2; j > -1; --j ) {
      // std::cerr << "Smoothing site " << j << std::endl;
      smooth(sites, track, j);
    }

    track->compute_chi2(sites);

    // monitor.fill(sites);
    // monitor.print_info(sites);
    delete track;
  }
}
*/

//void KalmanTrackFit::initialise(SciFiHelicalPRTrack* seed, std::vector<KalmanSite> &sites,
//                                double &momentum) {
void KalmanTrackFit::initialise(KalmanSeed &seed,
                                std::vector<KalmanSite> &sites) {
  TMatrixD a0(5, 1);
  a0 = seed.get_initial_state_vector();



  TMatrixD C(5, 5);
  C.Zero();
  // for ( int i = 0; i < 5; ++i ) {
  C(0, 0) = _seed_cov/100.; // dummy values
  C(1, 1) = _seed_cov; // dummy values
  C(2, 2) = _seed_cov/100.; // dummy values
  C(3, 3) = _seed_cov; // dummy values
  C(4, 4) = _seed_cov; // dummy values

  KalmanSite first_plane;
  first_plane.set_projected_a(a0);
  first_plane.set_projected_covariance_matrix(C);
  first_plane.set_measurement(clusters[0]->get_alpha());
  first_plane.set_direction(clusters[0]->get_direction());
  first_plane.set_z(clusters[0]->get_position().z());
  first_plane.set_id(clusters[0]->get_id());
  first_plane.set_type(2);
  // first_plane
  sites.push_back(first_plane);

  for ( int j = 1; j < numb_sites; ++j ) {
    KalmanSite a_site;
    a_site.set_measurement(clusters[j]->get_alpha());
    a_site.set_direction(clusters[j]->get_direction());
    a_site.set_z(clusters[j]->get_position().z());
    a_site.set_id(clusters[j]->get_id());
    a_site.set_type(2);
    sites.push_back(a_site);
  }

  for ( int j = 0; j < numb_sites; ++j ) {
    CLHEP::Hep3Vector true_position = clusters[j]->get_true_position();
    CLHEP::Hep3Vector true_momentum = clusters[j]->get_true_momentum();
    sites[j].set_true_position(true_position);
    sites[j].set_true_momentum(true_momentum);
  }
}

void KalmanTrackFit::initialise(SciFiStraightPRTrack *seed, std::vector<KalmanSite> &sites,
                                KalmanSciFiAlignment &kalman_align) {
  TMatrixD a0(5, 1);
  a0 = seed.get_initial_state_vector();
/*
  // Process PR seed.
  std::vector<SciFiSpacePoint> spacepoints;
  for ( size_t i = 0; i < seed->get_spacepoints().size(); ++i ) {
    SciFiSpacePoint sp = *seed->get_spacepoints()[i];
    spacepoints.push_back(sp);
  }
  std::vector<SciFiCluster*> clusters;
  double seed_pz = 200.; // MeV/c
  process_clusters(spacepoints, clusters, seed_pz);

  // std::cerr << "Number of clusters: " << clusters.size() << "\n";
  // Initialise Kalman.
  double x = spacepoints[0].get_position().x();
  double y = spacepoints[0].get_position().y();
  double mx = seed->get_mx();
  double my = seed->get_my();

  TMatrixD a(5, 1);
  a(0, 0) = x;
  a(1, 0) = mx;
  a(2, 0) = y;
  a(3, 0) = my;
  a(4, 0) = 1./seed_pz;
*/
  TMatrixD C(5, 5);
  C.Zero();
  // for ( int i = 0; i < 5; ++i ) {
  C(0, 0) = _seed_cov/200.; // dummy values
  C(1, 1) = _seed_cov/50.; // dummy values
  C(2, 2) = _seed_cov/200.; // dummy values
  C(3, 3) = _seed_cov/50.; // dummy values
  C(4, 4) = _seed_cov/50.; // dummy values
  // }

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

  int numb_sites = clusters.size();
  for ( int j = 1; j < numb_sites; ++j ) {
    KalmanSite a_site;
    a_site.set_measurement(clusters[j]->get_alpha());
    a_site.set_direction(clusters[j]->get_direction());
    a_site.set_z(clusters[j]->get_position().z());
    int id = clusters[j]->get_id();
    a_site.set_id(id);
    a_site.set_type(2);
    a_site.set_shifts(kalman_align.get_shifts(id));
    // a_site.set_rotations(kalman_align.get_rotations(id));
    a_site.set_S_covariance(kalman_align.get_cov_shifts(id));
    // a_site.set_R_covariance(kalman_align.get_cov_rotat(id));
    sites.push_back(a_site);
  }

  for ( int j = 0; j < numb_sites; ++j ) {
    CLHEP::Hep3Vector true_position = clusters[j]->get_true_position();
    CLHEP::Hep3Vector true_momentum = clusters[j]->get_true_momentum();
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
  KalmanSite *alignment_projection_site = 0;
  // Get previous site too.
  // if ( !(current_site%3) ) {
  int id = a_site->get_id();
  if ( id == 3 ) {
    std::cout << "Site 3; shift is: " << std::endl;
    alignment_projection_site = a_site;
    alignment_projection_site->get_shifts().Print();
  } else if ( id == 4 || id == 5 ) {
    std::cout << "Site " << id << std::endl;
    alignment_projection_site = &sites[current_site-1];
    alignment_projection_site->get_shifts().Print();
  }
    track->update_V(a_site);
    track->update_H(a_site);
    track->update_W(a_site);
    track->update_misaligments(a_site, alignment_projection_site);
/*
  // Update measurement error:
  // (non-const std for the perp direction)
  track->update_V(a_site);
  track->update_H(a_site);
  track->update_W(a_site);

  track->update_misaligments(a_site, alignment_projection_site);
*/
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
  // track->subtract_energy_loss(old_site, new_site);

  // Calculate the system noise...
  track->calc_system_noise(old_site, new_site);

  // ... so that we can compute the prediction for the
  // covariance matrix.
  track->calc_covariance(old_site, new_site);
}

void KalmanTrackFit::smooth(std::vector<KalmanSite> &sites, KalmanTrack *track, int k) {
  // Get site to be smoothed...
  KalmanSite *smoothing_site = &sites[k];

  // ... and the already perfected site.
  KalmanSite *optimum_site = &sites[k+1];

  // Set the propagator right.
  track->update_propagator(optimum_site, smoothing_site);

  // Compute A_k.
  track->update_back_transportation_matrix(optimum_site, smoothing_site);

  // Compute smoothed a_k and C_k.
  track->smooth_back(optimum_site, smoothing_site);
}

} // ~namespace MAUS

