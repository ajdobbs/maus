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

// C++ headers
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

// External libs headers
#include "gsl/gsl_fit.h"
#include "CLHEP/Matrix/Matrix.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/PatternRecognition.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"

// namespace MAUS {

PatternRecognition::PatternRecognition() {
  // Do nothing
};

PatternRecognition::~PatternRecognition() {
  // Do nothing
};

void PatternRecognition::process(SciFiEvent &evt) {

  std::cout << "\nBegining Pattern Recognition" << std::endl;
  std::cout << "Number of spacepoints in spill: " << evt.spacepoints().size() << std::endl;

  if ( static_cast<int>(evt.spacepoints().size()) > 0 ) {

    // Split spacepoints up according to which tracker they occured in and set used flag to false
    std::vector< std::vector<SciFiSpacePoint*> > spnts_by_tracker(_n_trackers);
    for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {  // Loop over trackers
      for ( unsigned int i = 0; i < evt.spacepoints().size(); ++i ) {  // Loop over spacepoints
        evt.spacepoints()[i]->set_used(false);
        if ( evt.spacepoints()[i]->get_tracker() == trker_no ) {
          spnts_by_tracker[trker_no].push_back(evt.spacepoints()[i]);
        }
      } // ~Loop over spacepoints
    } // ~Loop over trackers

    // Loop over trackers
    for ( int trker_no = 0; trker_no < _n_trackers; ++trker_no ) {
      std::cout << "Reconstructing for Tracker " << trker_no + 1 << std::endl;

      // Split spacepoints according to which station they occured in
      std::vector< std::vector<SciFiSpacePoint*> > spnts_by_station(_n_stations);
      sort_by_station(spnts_by_tracker[trker_no], spnts_by_station);

      // Count how many stations have at least one *unused* spacepoint
      int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

      // Make the tracks depending on how many stations have spacepoints in them
      if (num_stations_hit == 5) {
        std::vector<SciFiStraightPRTrack> trks;
        make_5tracks(spnts_by_station, trks);
        for ( int i = 0; i < static_cast<int>(trks.size()); ++i ) {
          trks[i].set_tracker(trker_no);
          evt.add_straightprtrack(trks[i]);
        }
      }
      if (num_stations_hit > 3) {
        std::vector<SciFiStraightPRTrack> trks;
        make_4tracks(spnts_by_station, trks);
        for ( int i = 0; i < static_cast<int>(trks.size()); ++i ) {
          trks[i].set_tracker(trker_no);
          evt.add_straightprtrack(trks[i]);
        }
      }
      if (num_stations_hit > 2) {
        std::vector<SciFiStraightPRTrack> trks;
        make_3tracks(spnts_by_station, trks);
        for ( int i = 0; i < static_cast<int>(trks.size()); ++i ) {
          trks[i].set_tracker(trker_no);
          evt.add_straightprtrack(trks[i]);
        }
      }
      std::cout << "Finished Tracker " << trker_no + 1 << std::endl;
    }// ~Loop over trackers
    std::cout << "Number of tracks found: " << evt.straightprtracks().size() << "\n\n";

  } else {
    std::cout << "No spacepoints in event" << std::endl;
  }
};

void PatternRecognition::make_5tracks(
                         std::vector< std::vector<SciFiSpacePoint*> >& spnts_by_station,
                         std::vector<SciFiStraightPRTrack>& trks) {
  std::cout << "Making 5 point tracks" << std::endl;

  int num_points = 5;
  std::vector<int> ignore_stations; // A zero size vector sets that all stations are used
  make_straight_tracks(num_points, ignore_stations, spnts_by_station, trks);

  std::cout << "Finished making 5 pt tracks" << std::endl;
} // ~make_spr_5pt(...)

void PatternRecognition::make_4tracks(
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<SciFiStraightPRTrack> &trks) {
  std::cout << "Making 4 point tracks" << std::endl;

  int num_points = 4;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

     std::cout << "4pt track: 5 stations with unused spacepoints" << std::endl;

    for (int i = 0; i < 5; ++i) { // Loop of stations, ignoring each one in turn
      // Recount how many stations have at least one unused spacepoint
      num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
      // If there are enough occupied stations left to make a 4 point track, keep making tracks
      if ( num_stations_hit  >= num_points ) {
        std::vector<int> ignore_stations(1, i);
        make_straight_tracks(num_points, ignore_stations, spnts_by_station, trks);
      } else {
        break;
      }
    } // ~Loop of stations, ignoring each one in turn
  } else if ( num_stations_hit == 4 ) {

     std::cout << "4pt track: 4 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 1 ) {
      make_straight_tracks(num_points, stations_not_hit, spnts_by_station, trks);
    } else {
      std::cout << "Wrong number of stations without spacepoints, ";
      std::cout << "aborting 4 pt track." << std::endl;
    }
  } else if ( num_stations_hit < 4 ) {
    std::cout << "Not enough unused spacepoints, quiting 4 point track." << std::endl;
  } else if ( num_stations_hit > 6 ) {
    std::cout << "Wrong number of stations with spacepoints, aborting 4 pt track." << std::endl;
  }

  std::cout << "Finished making 4 pt tracks" << std::endl;
} // ~make_straight_4tracks(...)

void PatternRecognition::make_3tracks(
                         std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<SciFiStraightPRTrack> &trks) {
  std::cout << "Making 3 point track" << std::endl;

  int num_points = 3;

  // Count how many stations have at least one *unused* spacepoint
  int num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);

  bool sufficient_hit_stations = true;

  // Call make_tracks with parameters depending on how many stations have unused spacepoints
  if ( num_stations_hit == 5 ) {

    std::cout << "3pt track: 5 stations with unused spacepoints" << std::endl;

    for (int i = 0; i < 4; ++i) { // Loop of first station to ignore
      if ( sufficient_hit_stations ) {
        for (int j = i + 1; j < 5; ++j) { // Loop of second station to ignore
          if ( sufficient_hit_stations ) {
            // Recount how many stations have at least one unused spacepoint
            num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
            // If there are enough occupied stations left to make a 3pt track, keep making tracks
            if ( num_stations_hit  >= num_points ) {
              std::vector<int> ignore_stations;
              ignore_stations.push_back(i);
              ignore_stations.push_back(j);
              make_straight_tracks(num_points, ignore_stations, spnts_by_station, trks);
            } else {
                sufficient_hit_stations = false;
            }
          } // ~if ( sufficient_hit_stations )
        } // ~Loop of second station to ignore
      } // ~if ( sufficient_hit_stations )
    } // ~Loop of first station to ignore
  } else if ( num_stations_hit == 4 ) {

    std::cout << "3pt track: 4 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);
    std::vector<int> ignore_stations;

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 1 ) {
      for (int i = 0; i < 5; ++i) { // Loop of stations, ignoring each one in turn
        // Recount how many stations have at least one unused spacepoint
        num_stations_hit = num_stations_with_unused_spnts(spnts_by_station);
        // If there are enough occupied stations left to make a 4 point track, keep making tracks
        if ( num_stations_hit  >= num_points ) {
          ignore_stations.clear();
          ignore_stations.push_back(stations_not_hit[0]);
          ignore_stations.push_back(i);
          make_straight_tracks(num_points, ignore_stations, spnts_by_station, trks);
        } else {
          break;
        }
      }
    }
  } else if ( num_stations_hit == 3 ) {

    std::cout << "3pt track: 3 stations with unused spacepoints" << std::endl;

    // Find out which station has no unused hits (1st entry in stations_not_hit vector)
    std::vector<int> stations_hit, stations_not_hit;
    stations_with_unused_spnts(spnts_by_station, stations_hit, stations_not_hit);

    // Make the tracks
    if ( static_cast<int>(stations_not_hit.size()) == 2 ) {
      make_straight_tracks(num_points, stations_not_hit, spnts_by_station, trks);
    } else {
      std::cout << "Wrong number of stations without spacepoints, ";
      std::cout << "aborting 3 pt track." << std::endl;
    }

  } else if ( num_stations_hit < 3 ) {
      std::cout << "Not enough unused spacepoints, quiting 3 point track." << std::endl;
  } else if ( num_stations_hit > 6 ) {
      std::cout << "Wrong number of stations with spacepoints, aborting 3 pt track." << std::endl;
  }
  std::cout << "Finished making 3 pt tracks" << std::endl;
} // ~make_straight_3tracks(...)

void PatternRecognition::make_straight_tracks(const int num_points,
                                     const std::vector<int> ignore_stations,
                                     std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                                     std::vector<SciFiStraightPRTrack> &trks) {
  // Set inner and outer stations
  int outer_station_num = -1, inner_station_num = -1;
  set_end_stations(ignore_stations, outer_station_num, inner_station_num);

  // Loop over spacepoints in outer station
  for ( int station_outer_sp = 0;
        station_outer_sp < static_cast<int>(spnts_by_station[outer_station_num].size());
        ++station_outer_sp ) {

    // Check the outer spacepoint is unused and enough stations are left with unused sp
    if ( !spnts_by_station[outer_station_num][station_outer_sp]->get_used() &&
         num_stations_with_unused_spnts(spnts_by_station) >= num_points) {

      // Loop over spacepoints in inner station
      for ( int station_inner_sp = 0;
          station_inner_sp < static_cast<int>(spnts_by_station[inner_station_num].size());
          ++station_inner_sp ) {

        // Check the inner spacepoint is unused and enough stations are left with unused sp
        if ( !spnts_by_station[inner_station_num][station_inner_sp]->get_used() &&
             num_stations_with_unused_spnts(spnts_by_station) >= num_points ) {

          // Vector to hold the good spacepoints in each station
          std::vector<SciFiSpacePoint*> good_spnts;
          // good_spnts.resize(num_points);

          // Set variables to hold which stations are to be ignored
          int ignore_station_1 = -1, ignore_station_2 = -1;
          set_ignore_stations(ignore_stations, ignore_station_1, ignore_station_2);

          // Draw a straight line between spacepoints in outer most and inner most stations
          SimpleLine line_x, line_y;
          draw_line(spnts_by_station[outer_station_num][station_outer_sp],
                    spnts_by_station[inner_station_num][station_inner_sp],
                    line_x, line_y);

          // Loop over intermediate stations and compare spacepoints with the line
          for ( int station_num = inner_station_num + 1;
                station_num < outer_station_num; ++station_num ) {
            if (station_num != ignore_station_1 && station_num != ignore_station_2) {

              double delta_sq = 1000000;  // A large number so initial value is set as best first

              for ( int sp_no = 0;
                    sp_no < static_cast<int>(spnts_by_station[station_num].size()); ++sp_no ) {

                // If the spacepoint has not already been used in a track fit
                if ( !spnts_by_station[station_num][sp_no]->get_used() ) {
                  Hep3Vector pos = spnts_by_station[station_num][sp_no]->get_position();

                  // Calculate the residuals
                  double dx = pos.x() - ( line_x.get_c() + ( pos.z() * line_x.get_m() ) );
                  double dy = pos.y() - ( line_y.get_c() + ( pos.z() * line_y.get_m() ) );

                  // Apply roadcuts & find the spacepoints with the smallest residuals for the line
                  if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut && delta_sq > (dx*dx + dy*dy) ) {
                    delta_sq = dx*dx + dy*dy;
                    good_spnts.push_back(spnts_by_station[station_num][sp_no]);
                  } // ~If pass roadcuts and beats previous best fit point
                } // ~If spacepoint is unused
              } // ~Loop over spacepoints
            } // ~if (station_num != ignore_station)
          } // ~Loop over intermediate stations

          // Clear the line objects so we can reuse them
          line_x.clear();
          line_y.clear();

          // Check we have at least 1 good spacepoint in each of the intermediate stations
          std::cout << "Num of intermediate stations with good sp: " << good_spnts.size() << "\n";

          if ( static_cast<int>(good_spnts.size()) > (num_points - 3) ) {
            std::cout << "Found good spacepoints in all stations, fitting a track..." << std::endl;

            good_spnts.insert(good_spnts.begin(),
                              spnts_by_station[inner_station_num][station_inner_sp]);
            good_spnts.push_back(spnts_by_station[outer_station_num][station_outer_sp]);

            // Fit track
            SimpleLine line_x, line_y;
            linear_fit(good_spnts, line_x, line_y);

            // Check track passes chisq test, then create SciFiStraightPRTrack
            if ( ( line_x.get_chisq() / ( num_points - 2 ) < _chisq_cut ) &&
                 ( line_y.get_chisq() / ( num_points - 2 ) < _chisq_cut ) ) {

              std::cout << "** chisq test passed, adding " << num_points << "pt track **\n";

              SciFiStraightPRTrack track(-1, num_points, line_x, line_y);

              // Set all the good sp to used and convert pointers to variables
              std::vector<SciFiSpacePoint> good_spnts_variables;
              good_spnts_variables.resize(good_spnts.size());

              for ( int i = 0; i < static_cast<int>(good_spnts.size()); ++i ) {
                good_spnts[i]->set_used(true);
                good_spnts_variables[i] = *good_spnts[i];
              }

              // Populate the sp of the track and then push the track back into the trks vector
              track.set_spacepoints(good_spnts_variables);
              trks.push_back(track);

            } else {
              std::cout << "x_chisq = " << line_x.get_chisq();
              std::cout << "\ty_chisq = " << line_y.get_chisq() << std::endl;
              std:: cout << "chisq test failed, " << num_points << "pt track rejected" << std::endl;
            } // ~Check track passes chisq test
          } // ~ if ( good_spnts.size() > 1 )
        } else {
          // std::cout << "...no" << std::endl;
        }// ~Check the inner spacepoint is unused
      } // ~Loop over sp in station 1
    } else {
      std::cout << "...no" << std::endl;
    }// ~Check the outer spacepoint is unused
  } // ~Loop over sp in station 5
}


void PatternRecognition::linear_fit(const std::vector<SciFiSpacePoint*> &spnts,
                                    SimpleLine &line_x, SimpleLine &line_y) {

  int num_points = static_cast<int>(spnts.size());

  CLHEP::HepMatrix A(num_points, 2); // rows, columns
  CLHEP::HepMatrix V(num_points, num_points); // error matrix
  CLHEP::HepMatrix X(num_points, 1); // measurements
  CLHEP::HepMatrix Y(num_points, 1); // measurements

  // The error on the position measurements of sp in a tracker (same in x and y)
  double sd = -1.0;

  for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {

    if ( i == 4 )     // For station 5
      sd = _sd_5;
    else              // For the other stations
      sd = _sd_1to4;

    A[i][0] = 1;
    A[i][1] = spnts[i]->get_position().z();
    V[i][i] = ( sd * sd );
    X[i][0] = spnts[i]->get_position().x();
    Y[i][0] = spnts[i]->get_position().y();
  }

  CLHEP::HepMatrix At, tmpx, tmpy, xparams, yparams;

  int ierr;
  V.invert(ierr);

  At = A.T();
  tmpx = At * V * A;
  tmpx.invert(ierr);
  xparams = tmpx * At * V * X;

  tmpy = At * V * A;
  tmpy.invert(ierr);
  yparams = tmpy * At * V * Y;

  line_x.set_c(xparams[0][0]);
  line_x.set_m(xparams[1][0]);
  line_x.set_c_err(sqrt(tmpx[0][0]));
  line_x.set_m_err(sqrt(tmpx[1][1]));

  line_y.set_c(yparams[0][0]);
  line_y.set_m(yparams[1][0]);
  line_y.set_c_err(sqrt(tmpy[0][0]));
  line_y.set_m_err(sqrt(tmpy[1][1]));

  CLHEP::HepMatrix C, D, result;

  C = X - (A * xparams);
  result = C.T() * V * C;
  line_x.set_chisq(result[0][0]);
  line_x.set_chisq_dof(result[0][0] / num_points);

  D = Y - (A * yparams);
  result = D.T()* V * D;
  line_y.set_chisq(result[0][0]);
  line_y.set_chisq_dof(result[0][0] / num_points);
}

void PatternRecognition::make_helix(
       const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
       const std::vector<int> ignore_stations,
       const int outer_station_num, const int inner_station_num,
       const int station_outer_sp, const int station_inner_sp,
       std::map<int, SciFiSpacePoint*> &good_spnts,
       std::vector<SciFiStraightPRTrack> &trks) {

  int ignore_station_1 = -1, ignore_station_2 = -1;
  if ( ignore_stations.size() == 0 ) {
     // Leave ignore stations as -1
  } else if ( ignore_stations.size() == 1 ) {
    ignore_station_1 = ignore_stations[0];
  } else if ( ignore_stations.size() == 2 ) {
    ignore_station_1 = ignore_stations[0];
    ignore_station_2 = ignore_stations[1];
  } else if ( ignore_stations.size() > 2 ) {
      std::cout << "Error: Invalid ignore station argument." << std::endl;
      return;
  }

  SimpleCircle circle1;

  // Find the circle that fits uniquely to the 3 spacepoints: from inner, outer
  // and intermediate stations
  Hep3Vector pos_outer = spnts_by_station[outer_station_num][station_outer_sp]->get_position();
  Hep3Vector pos_inner = spnts_by_station[inner_station_num][station_inner_sp]->get_position();

  // Loop over intermediate stations to find one more spacepoint for circle seed
  for ( int mid_station_num = inner_station_num + 1;
        mid_station_num < outer_station_num; ++mid_station_num ) {
    if (mid_station_num != ignore_station_1 && mid_station_num != ignore_station_2) {
      for ( int sp_no = 0;
            sp_no < static_cast<int>(spnts_by_station[mid_station_num].size()); ++sp_no ) {
        if ( !spnts_by_station[mid_station_num][sp_no]->get_used() ) {
          Hep3Vector pos_medium = spnts_by_station[mid_station_num][sp_no]->get_position();
          // Send off the three spacepoints to made the initial circle
          bool goodcircle = initial_circle(pos_inner, pos_outer, pos_medium, circle1);
          if ( goodcircle ) {
            for ( int station_num = mid_station_num + 1;
                  station_num < outer_station_num; ++station_num ) {
              if (station_num != ignore_station_1 && station_num != ignore_station_2) {
               /* for ( int sp
                Hep3Vector pos_interm = spnts_by_station[station_num][]
                double dR = delta_R(circle1, pos_intermediate);
                if (dr < R_cut)
                  // proceed...
*/
              } // ~if (station_num != ignore_station)
            } // ~Loop over intermediate stations not used in seed
          } // ~If the seed circle is formed correctly with finite radius
 /*         // Apply roadcuts & find the spacepoints with the smallest residuals for the line
          if ( fabs(dx) < _res_cut && fabs(dy) < _res_cut && delta_sq > (dx*dx + dy*dy) ) {
            delta_sq = dx*dx + dy*dy;
            good_spnts[station_num] = spnts_by_station[station_num][sp_no];
          } // ~If pass roadcuts and beats previous best fit point*/
        } // ~If spacepoint is unused
      } // ~Loop over spacepoints
    } // ~if (station_num != ignore_station)
  } // ~Loop over intermediate stations
} // ~initial_circle(...)

bool PatternRecognition::initial_circle(const CLHEP::Hep3Vector &p1, const CLHEP::Hep3Vector &p2,
                                        const CLHEP::Hep3Vector &p3, SimpleCircle &circle) {

  CLHEP::HepMatrix a(3, 3); // Rows, columns
  CLHEP::HepMatrix d(3, 3);
  CLHEP::HepMatrix e(3, 3);
  CLHEP::HepMatrix f(3, 3);

  // Row 1
  a[0][0] = p1.x();
  a[0][1] = p1.y();
  a[0][2] = 1.;
  // Row 2
  a[1][0] = p2.x();
  a[1][1] = p2.y();
  a[1][2] = 1.;
  // Row 3
  a[2][0] = p3.x();
  a[2][1] = p3.y();
  a[2][2] = 1.;

  // Row 1
  d[0][0] = p1.x() * p1.x() + p1.y() * p1.y();
  d[0][1] = p1.y();
  d[0][2] = 1.;
  // Row 2
  d[1][0] = p2.x() * p2.x() + p2.y() * p2.y();
  d[1][1] = p2.y();
  d[1][2] = 1.;
  // Row 3
  d[2][0] = p3.x() * p3.x() + p3.y() * p3.y();
  d[2][1] = p3.y();
  d[2][2] = 1.;

  // Row 1
  e[0][0] = p1.x() * p1.x() + p1.y() * p1.y();
  e[0][1] = p1.x();
  e[0][2] = 1.;
  // Row 2
  e[1][0] = p2.x() * p2.x() + p2.y() * p2.y();
  e[1][1] = p2.x();
  e[1][2] = 1.;
  // Row 3
  e[2][0] = p3.x() * p3.x() + p3.y() * p3.y();
  e[2][1] = p3.x();
  e[2][2] = 1.;

  // Row 1
  f[0][0] = p1.x() * p1.x() + p1.y() * p1.y();
  f[0][1] = p1.x();
  f[0][2] = p1.y();
  // Row 2
  f[1][0] = p2.x() * p2.x() + p2.y() * p2.y();
  f[1][1] = p2.x();
  f[1][2] = p2.y();
  // Row 3
  f[2][0] = p3.x() * p3.x() + p3.y() * p3.y();
  f[2][1] = p3.x();
  f[2][2] = p3.y();

  double detA = a.determinant();
  double detD = - d.determinant();
  double detE = e.determinant();
  double detF = - f.determinant();

  if (detA == 0)
    return false;

  double x0 = - detD / (2. * detA);
  double y0 = - detE / (2. * detA);

  double det = ( detD * detD + detE * detE ) / (4. * detA * detA ) - ( detF / detA );

  if (det < 0.)
    return false;

  double r = sqrt(det); // mm

  circle.set_x0(x0);
  circle.set_y0(y0);
  circle.set_R(r);

  return true;
}

double PatternRecognition::delta_R(const SimpleCircle &circle, const CLHEP::Hep3Vector &pos) {

  double x0 = circle.get_x0();
  double y0 = circle.get_y0();
  double R = circle.get_R();

  double R_i = sqrt((pos.x() - x0)*(pos.x() - x0) + (pos.y() - y0)*(pos.y() - y0));

  double delta = R_i - R;

  return delta;
}

void PatternRecognition::calculate_dipangle(const std::map<int, SciFiSpacePoint*> &spnts,
                                            const SimpleCircle &circle, SimpleLine &line_sz) {

  int num_points = static_cast<int>(spnts.size());

  CLHEP::HepMatrix A(num_points, 2); // rows, columns
  CLHEP::HepMatrix V(num_points, num_points); // error matrix
  CLHEP::HepMatrix S(num_points, 1); // measurements

  double R = circle.get_R();

  std::vector<double> dz;
  std::vector<double> dphi;
  std::vector<double> dphi_err;

  for ( std::map<int, SciFiSpacePoint*>::const_iterator ii = spnts.begin();
       ii != spnts.end(); ++ii ) {

    std::map<int, SciFiSpacePoint*>::const_iterator jj = ii;
    jj = jj++;

    double z_i = (*ii).second->get_position().z();
    double z_j = (*jj).second->get_position().z();
    double dz_ji = z_j - z_i;
    dz.push_back(dz_ji);

    double x_i = (*ii).second->get_position().x();
    double y_i = (*ii).second->get_position().y();
    double phi_i = calculate_Phi(x_i, y_i, circle);

    double x_j = (*jj).second->get_position().x();
    double y_j = (*jj).second->get_position().y();
    double phi_j = calculate_Phi(x_j, y_j, circle);

    double sd_phi = -1.0;
    if ( (*ii).first == 5 )
      sd_phi = _sd_phi_5;
    else
      sd_phi = _sd_phi_1to4;

    double dphi_ji = phi_j - phi_i;
    dphi.push_back(dphi_ji);
    dphi_err.push_back(sd_phi);
  }

    bool ok = turns_between_stations(dz, dphi);

    // if (ok) --> do linear fit.
}

double PatternRecognition::calculate_Phi(double xpos, double ypos, const SimpleCircle &circle) {

  double angle = atan2(ypos - circle.get_y0(), xpos - circle.get_x0());

  if ( angle < 0. )
    angle += 2. * pi;

  return angle;
}

void PatternRecognition::circle_fit(const std::map<int, SciFiSpacePoint*> &spnts,
                                    SimpleCircle &circle) {

  int num_points = spnts.size();

  CLHEP::HepMatrix A(num_points, 3); // rows, columns
  CLHEP::HepMatrix V(num_points, num_points); // error matrix
  CLHEP::HepMatrix K(num_points, 1);

  int counter = 0;
  for ( std::map<int, SciFiSpacePoint*>::const_iterator ii = spnts.begin();
       ii != spnts.end(); ++ii ) {

  // This part is subject to change once we figure out proper errors
  double sd = -1.0;
  if ( (*ii).first == 5 )
    sd = _sd_5;
  else
    sd = _sd_1to4;

    double tmp_xpos = (*ii).second->get_position().x();
    double tmp_ypos = (*ii).second->get_position().y();

    A[counter][0] = ( tmp_xpos * tmp_xpos ) + ( tmp_ypos * tmp_ypos );
    A[counter][1] = (*ii).second->get_position().x();
    V[counter][counter] = ( sd * sd );
    K[counter][0] = 1; // I don't entirely understand why this should be 1

    ++counter;
  }

  CLHEP::HepMatrix At, tmpx, tmp_params;

  int ierr;
  V.invert(ierr);

  At = A.T();
  tmpx = At * V * A;
  tmpx.invert(ierr);
  tmp_params = tmpx * At * V * K;

  double a, b, c;
  a = tmp_params[0][0];
  b = tmp_params[1][0];
  c = tmp_params[2][0];

  double x0, y0, R;
  x0 = -b / 2 * a;
  y0 = -c / 2 * a;
  R = sqrt((4 * a) + (b * b) + (c * c)) / (2 * a);

  circle.set_x0(x0);
  circle.set_y0(y0);
  circle.set_R(R);
  /*
  circle.set_x0_err(x0_err);
  circle.set_y0_err(y0_err);
  circle.set_R_err(R_err);
  */

  CLHEP::HepMatrix C, result;

  C = K - (A * tmp_params);
  result = C.T() * V * C;
  double xchi2 = result[0][0];
  circle.set_chisq(xchi2 / num_points);
}

bool PatternRecognition::turns_between_stations(const std::vector<double> &dz,
                                                std::vector<double> &dphi) {

  //  Make sure that you have enough points to make a line (2)
  if ( dz.size() < 2 || dphi.size() < 2 )
    return false;

  if ( dphi[0] < 0 )
    dphi[0] += 2 * pi;

  //  Make sure that dphi is always increasing between stations
  for ( int j = 1; j < static_cast<int>(dphi.size()); j++ ) {
    while ( dphi[j-1] > dphi[j] )
      dphi[j] += 2 * pi;
  }

  for ( int j = 1; j < static_cast<int>(dphi.size()) ; j++ ) {

    bool good_AB = AB_ratio(dphi[j-1] , dphi[j], dz[j-1], dz[j]);

    if ( good_AB )
      return true;
    else
      return false;
  } // end j
}

bool PatternRecognition::AB_ratio(double &dphi_kj, double &dphi_ji, double dz_kj,
                                  double dz_ji) {

  double A, B;
  for ( int n = 0; n < 5; ++n )
    for ( int m = 0; m < 5; ++m ) {
      A = ( dphi_kj + ( 2 * n * pi ) ) / ( dphi_ji + ( 2 * m * pi ) );
      B = dz_kj / dz_ji;

      std::cout << "A-B = " << fabs(A-B) << std::endl;

      if ( fabs(A - B) < _AB_cut ) {
        dphi_kj += 2 * n * pi;
        dphi_ji += 2 * m * pi;

        return true;
      }
    }
  return false; // Return false if _ABcut is never satisfied
}

void PatternRecognition::set_end_stations(const std::vector<int> ignore_stations,
                      int &outer_station_num, int &inner_station_num) {

  if ( static_cast<int>(ignore_stations.size()) == 0 ) { // 5pt track case
    outer_station_num = 4;
    inner_station_num = 0;
  } else if ( static_cast<int>(ignore_stations.size()) == 1 ) { // 4pt track case
      // Set outer station number
      if ( ignore_stations[0] != 4 )
        outer_station_num = 4;
      else
        outer_station_num = 3;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_station_num = 0;
      else
        inner_station_num = 1;
  } else if ( static_cast<int>(ignore_stations.size()) == 2 ) { // 3pt track case
      std::cout << "stations " << ignore_stations[0] << " and " << ignore_stations[1] << std::endl;
      // Set outer station number
      if ( ignore_stations[1] != 4 )
        outer_station_num = 4;
      else if ( ignore_stations[0] != 3 )
        outer_station_num = 3;
      else
        outer_station_num = 2;
      // Set inner station number
      if ( ignore_stations[0] != 0 )
        inner_station_num = 0;
      else if ( ignore_stations[1] != 1 )
        inner_station_num = 1;
      else
        inner_station_num = 2;
  } else {
    std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
}

void PatternRecognition::sort_by_station(const std::vector<SciFiSpacePoint*> &spnts,
                                 std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station) {
  for ( int station_num = 0;
        station_num < static_cast<int>(spnts_by_station.size()); ++station_num ) {
    for ( int i = 0; i < static_cast<int>(spnts.size()); ++i ) {
      if ( spnts[i]->get_station() == station_num + 1 ) {
        spnts_by_station[station_num].push_back(spnts[i]);
      }
    }
  }
}

int PatternRecognition::num_stations_with_unused_spnts(
    const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station) {

  int stations_hit = 0;

  std::cout << "Unused spacepoints per station:\t";

  for ( int i = 0; i < static_cast<int>(spnts_by_station.size()); ++i ) {
    int unused_sp = 0;
    for ( int j = 0; j < static_cast<int>(spnts_by_station[i].size()); ++j ) {
      if ( !spnts_by_station[i][j]->get_used() ) {
        ++unused_sp;
      }
    }
    if ( unused_sp > 0 ) {
      ++stations_hit;
    }
    std::cout << unused_sp << "  ";
  }
  std::cout << std::endl;
  return stations_hit;
}

void PatternRecognition::stations_with_unused_spnts(
                         const std::vector< std::vector<SciFiSpacePoint*> > &spnts_by_station,
                         std::vector<int> &stations_hit, std::vector<int> &stations_not_hit) {

  stations_hit.clear();
  stations_not_hit.clear();

  std::cout << "Unused spacepoints per station:\t";

  for ( int i = 0; i < static_cast<int>(spnts_by_station.size()); ++i ) {

    int unused_sp = 0;

    for ( int j = 0; j < static_cast<int>(spnts_by_station[i].size()); ++j ) {
      if ( !spnts_by_station[i][j]->get_used() ) {
        ++unused_sp;
      }
    }

    if ( unused_sp > 0 ) {
      stations_hit.push_back(i);
    } else if ( unused_sp == 0 ) {
      stations_not_hit.push_back(i);
    }
    std::cout << unused_sp << "  ";
  }
  std::cout << std::endl;
}

void PatternRecognition::set_ignore_stations(const std::vector<int> &ignore_stations,
                         int &ignore_station_1, int &ignore_station_2) {

  ignore_station_1 = -1, ignore_station_2 = -1;
  if ( ignore_stations.size() == 0 ) {
    // Leave ignore stations as -1
  } else if ( ignore_stations.size() == 1 ) {
    ignore_station_1 = ignore_stations[0];
  } else if ( ignore_stations.size() == 2 ) {
    ignore_station_1 = ignore_stations[0];
    ignore_station_2 = ignore_stations[1];
  } else if ( ignore_stations.size() > 2 ) {
    std::cout << "Error: Invalid ignore station argument." << std::endl;
  }
}

void PatternRecognition::draw_line(const SciFiSpacePoint *sp1, const SciFiSpacePoint *sp2,
                                   SimpleLine &line_x, SimpleLine &line_y) {

          Hep3Vector pos_outer = sp1->get_position();
          Hep3Vector pos_inner = sp2->get_position();

          line_x.set_m(( pos_inner.x() - pos_outer.x()) / (pos_inner.z() - pos_outer.z() ));
          line_x.set_c(pos_outer.x() - ( pos_outer.z() * line_x.get_m()) );

          line_y.set_m(( pos_inner.y() - pos_outer.y()) / (pos_inner.z() - pos_outer.z() ));
          line_y.set_c(pos_outer.y() - ( pos_outer.z() *  line_y.get_m() ));

          std::cout << "m_xi = " << line_x.get_m()  << "\tx_0i = " << line_x.get_c() << std::endl;
          std::cout << "m_yi = " << line_y.get_m() << "\ty_0i = " << line_y.get_c() << std::endl;
}

// } // ~namespace MAUS
