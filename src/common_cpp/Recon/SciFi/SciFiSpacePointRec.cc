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

#include "src/common_cpp/Recon/SciFi/SciFiSpacePointRec.hh"

// namespace MAUS {

SciFiSpacePointRec::SciFiSpacePointRec() {}

SciFiSpacePointRec::~SciFiSpacePointRec() {}

void SciFiSpacePointRec::process(SciFiEvent &evt) {
  int tracker, station, plane;
  int clusters_size = evt.clusters().size();
  // Store clusters in a vector.
  std::vector<SciFiCluster*> clusters[2][6][3];
  for ( int cl = 0; cl < clusters_size; cl++ ) {
    SciFiCluster* a_cluster = evt.clusters()[cl];
    tracker = a_cluster->get_tracker();
    station = a_cluster->get_station();
    plane   = a_cluster->get_plane();
    clusters[tracker][station][plane].push_back(a_cluster);
  }

  // For each tracker,
  for ( int Tracker = 0; Tracker < 2; Tracker++ ) {
    // For each station,
    for ( int Station = 0; Station < 6; Station++ ) {
      // Make all possible combinations of doublet
      // clusters from each of the 3 views...
      // looping over all clusters in plane 0 (view v)
      int numb_clusters_in_v = clusters[Tracker][Station][0].size();
      int numb_clusters_in_w = clusters[Tracker][Station][1].size();
      int numb_clusters_in_u = clusters[Tracker][Station][2].size();
      for ( int cla = 0; cla < numb_clusters_in_v; cla++ ) {
        SciFiCluster* candidate_A = (clusters[Tracker][Station][0])[cla];

        // Looping over all clusters in plane 1 (view w)
        for ( int clb = 0; clb < numb_clusters_in_w; clb++ ) {
          SciFiCluster* candidate_B = (clusters[Tracker][Station][1])[clb];

          // Looping over all clusters in plane 2 (view u)
          for ( int clc = 0; clc < numb_clusters_in_u; clc++ ) {
            SciFiCluster* candidate_C = (clusters[Tracker][Station][2])[clc];

            if ( kuno_accepts(candidate_A, candidate_B, candidate_C) &&
                 clusters_are_not_used(candidate_A, candidate_B, candidate_C) ) {
              SciFiSpacePoint* triplet = new SciFiSpacePoint(candidate_A, candidate_B, candidate_C);
              build_triplet(triplet);
              evt.add_spacepoint(triplet);
            }
          }  // ends plane 2
        }  // ends plane 1
      }  // ends plane 0
    }  // end loop over stations
  }  // end loop over trackers

  // Run over left-overs and make duplets without any selection criteria
  for ( int a_plane = 0; a_plane < 2; a_plane++ ) {
    for ( int another_plane = a_plane+1; another_plane < 3; another_plane++ ) {
      for ( int Tracker = 0; Tracker < 2; Tracker++ ) {  // for each tracker
        for ( int Station = 0; Station < 6; Station++ ) {  // for each station
          // Make all possible combinations of doublet clusters from views 0 & 1
          // looping over all clusters in view 0, then 1
          for ( unsigned int cla = 0;
                cla < clusters[Tracker][Station][a_plane].size(); cla++ ) {
          SciFiCluster* candidate_A =
                          (clusters[Tracker][Station][a_plane])[cla];

            // Looping over all clusters in view 1,2, then 2
            for ( unsigned int clb = 0;
                  clb < clusters[Tracker][Station][another_plane].size();
                  clb++ ) {
              SciFiCluster* candidate_B =
                           (clusters[Tracker][Station][another_plane])[clb];

              if ( clusters_are_not_used(candidate_A, candidate_B) &&
                   candidate_A->get_plane() != candidate_B->get_plane() &&
                   duplet_within_radius(candidate_A, candidate_B) ) {
                SciFiSpacePoint* duplet = new SciFiSpacePoint(candidate_A, candidate_B);
                build_duplet(duplet);
                evt.add_spacepoint(duplet);
              }
            }
          }
        }
      }
    }
  }
}

bool SciFiSpacePointRec::duplet_within_radius(SciFiCluster* candidate_A,
                                              SciFiCluster* candidate_B) {
  Hep3Vector pos = crossing_pos(candidate_A, candidate_B);
  double radius = pow(pow(pos.x(), 2.0)+pow(pos.y(), 2.0), 0.5);
  if ( radius < _acceptable_radius ) {
    return true;
  } else {
    return false;
  }
}

bool SciFiSpacePointRec::kuno_accepts(SciFiCluster* cluster1,
                                      SciFiCluster* cluster2,
                                      SciFiCluster* cluster3) {
  // The 3 clusters passed to the kuno_accepts function belong
  // to the same station, only the planes are different
  int tracker = cluster1->get_tracker();
  int station = cluster1->get_station();

  double uvwSum = cluster1->get_channel() +
                  cluster2->get_channel() +
                  cluster3->get_channel();

  if ( (tracker == 0 && station == 5 && (uvwSum < (_kuno_0_5+_kuno_toler))
                                     && (uvwSum > (_kuno_0_5-_kuno_toler))) ||
     (!(tracker == 0 && station == 5)&& (uvwSum < (_kuno_else+_kuno_toler))
                                     && (uvwSum > (_kuno_else-_kuno_toler))) ) {
    return true;
  } else {
    return false;
  }
}

bool SciFiSpacePointRec::clusters_are_not_used(SciFiCluster* candidate_A,
                                               SciFiCluster* candidate_B) {
  if ( candidate_A->is_used() || candidate_B->is_used() ) {
    return false;
  } else {
    return true;
  }
}

bool SciFiSpacePointRec::clusters_are_not_used(SciFiCluster* candidate_A,
                                               SciFiCluster* candidate_B,
                                               SciFiCluster* candidate_C) {
  if ( candidate_A->is_used() || candidate_B->is_used() || candidate_C->is_used() ) {
    return false;
  } else {
    return true;
  }
}

// Given 3 input clusters, this function computes all the triplet variables,
// like position and respective standard deviation
void SciFiSpacePointRec::build_triplet(SciFiSpacePoint* triplet) {
  std::vector<SciFiCluster*> channels = triplet->get_channels();
  SciFiCluster *xcluster = channels[0];
  SciFiCluster *wcluster = channels[1];
  SciFiCluster *vcluster = channels[2];

  // This is the position of the space-point
  Hep3Vector p1 = crossing_pos(vcluster, xcluster);
  Hep3Vector p2 = crossing_pos(vcluster, wcluster);
  Hep3Vector p3 = crossing_pos(xcluster, wcluster);
  Hep3Vector position = (p1+p2+p3)/3.0;
  triplet->set_position(position);

  // Vector p stores the crossing position of views v and w.
  Hep3Vector p(p2);

  // Now, determine the perpendicular distance from the hit on the X view
  // to the intersection of the V and W views
  Hep3Vector x_dir(xcluster->get_direction());
  Hep3Vector x_pos(xcluster->get_position());

  // Assume that the station is perpendicular to the Z axis
  // get_chi_squared(x_pos,p);
  double x1 = x_pos.x();
  double y1 = x_pos.y();
  double x2 = x_pos.x() + 10.0*x_dir.x();
  double y2 = x_pos.y() + 10.0*x_dir.y();
  double x0 = p.x();
  double y0 = p.y();

  double dist = ((x2-x1) * (y1-y0) - (x1-x0) * (y2-y1)) /
                 sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));

  double chi2 = (dist*dist)/0.064;
  triplet->set_chi2(chi2);
/*
  // Determine time
  // get_time(vcluster, xcluster, wcluster);
  double time_A = vcluster->get_time();
  double time_B = xcluster->get_time();
  double time_C = wcluster->get_time();
  _time = (time_A+time_B+time_C)/3.0;
  _time_error = 0.0;
  _time_error += (time_A-_time)*time_A;
  _time_error += (time_B-_time)*time_B;
  _time_error += (time_C-_time)*time_C;
  _time_error = sqrt(_time_error);
  _time_res = time_A - _time;*/
}

void SciFiSpacePointRec::build_duplet(SciFiSpacePoint* duplet) {
  std::vector<SciFiCluster*> channels = duplet->get_channels();
  SciFiCluster *clusterA = channels[0];
  SciFiCluster *clusterB = channels[1];

  Hep3Vector p1 = crossing_pos(clusterA, clusterB);

  // This is the position of the space-point.
  Hep3Vector position(p1);
  duplet->set_position(position);

/*
  // Determine time
  double time_A = clusterA->get_time();
  double time_B = clusterB->get_time();
  _time = (time_A+time_B)/2.0;
  _time_error = 0.0;
  _time_error += (time_A-_time)*time_A;
  _time_error += (time_B-_time)*time_B;
  _time_error = sqrt(_time_error);
  _time_res = time_A - _time;*/
}

// This function calculates the intersection position of two clusters.
// The position of a space-point will be the mean
// of all 3 possible intersections.
Hep3Vector SciFiSpacePointRec::crossing_pos(SciFiCluster* c1,
                                         SciFiCluster* c2) {
    Hep3Vector d1 = c1->get_direction();

    Hep3Vector d2 = c2->get_direction();

    Hep3Vector c1_pos(c1->get_position());

    Hep3Vector c2_pos(c2->get_position());

    CLHEP::HepMatrix m1(3, 3, 0);

    m1[0][0] = (c2_pos-c1_pos).x();
    m1[0][1] = (c2_pos-c1_pos).y();
    m1[0][2] = (c2_pos-c1_pos).z();

    m1[1][0] = d2.x();
    m1[1][1] = d2.y();
    m1[1][2] = d2.z();

    m1[2][0] = (d1.cross(d2)).x();
    m1[2][1] = (d1.cross(d2)).y();
    m1[2][2] = (d1.cross(d2)).z();

    CLHEP::HepMatrix m2(3, 3, 0);

    m2[0][0] = (c2_pos-c1_pos).x();
    m2[0][1] = (c2_pos-c1_pos).y();
    m2[0][2] = (c2_pos-c1_pos).z();

    m2[1][0] = d1.x();
    m2[1][1] = d1.y();
    m2[1][2] = d1.z();

    m2[2][0] = (d1.cross(d2)).x();
    m2[2][1] = (d1.cross(d2)).y();
    m2[2][2] = (d1.cross(d2)).z();

    double t1 = m1.determinant() / pow((d1.cross(d2)).mag(), 2.);
    double t2 = m2.determinant() / pow((d1.cross(d2)).mag(), 2.);

    Hep3Vector p1 = c1_pos+t1*d1;
    Hep3Vector p2 = c2_pos+t2*d2;

    Hep3Vector an_intersection = (p1+p2)/2.;

    return an_intersection;
}
// } // ~namespace MAUS