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

#include "src/common_cpp/Recon/Kalman/MAUSTrackWrapper.hh"

#include <vector>

#include "TMatrixD.h"

namespace MAUS {

  template<class ELEMENT>
  bool SortByZ(const ELEMENT *a, const ELEMENT *b) {
      return (a->get_position().z() > b->get_position().z());
  }



  Kalman::Track BuildTrack(SciFiClusterPArray cluster_array,
                                                        const SciFiGeometryHelper* geom, int dim) {
    Kalman::Track new_track(dim);
    size_t numbclusters = cluster_array.size();

    if (numbclusters < 1) return new_track;
    int tracker = cluster_array[0]->get_tracker();

    const SciFiPlaneMap& geom_map = geom->GeometryMap().find(tracker)->second.Planes;
    int tracker_const = (tracker == 0 ? -1 : 1);

    for (SciFiPlaneMap::const_iterator iter = geom_map.begin(); iter != geom_map.end(); ++iter) {
      int id = iter->first * tracker_const;
      Kalman::TrackPoint new_trackpoint = Kalman::TrackPoint(dim, 1, iter->second.Position.z(), id);
      new_track.Append(new_trackpoint);
    }

    for (size_t j = 0; j < numbclusters; ++j) {
      SciFiCluster* cluster = cluster_array[j];

      int id = (cluster->get_station() - 1)*3 + cluster->get_plane(); // Actually (id - 1)

      TMatrixD state_vector(1, 1);
      TMatrixD covariance(1, 1);

      state_vector(0, 0) = - cluster->get_alpha(); // Alpha is defined backwards.
      covariance(0, 0) = geom->GetChannelWidth() * geom->GetChannelWidth() / 12.0;

      new_track[id].SetData(Kalman::State(state_vector, covariance));
    }
    return new_track;
  }



//  Kalman::State ComputeSeed(SciFiHelicalPRTrack* h_track, const SciFiGeometryHelper* geom,
//                                                     bool correct_energy_loss, double seed_cov) {
//    TMatrixD vector(5, 1);
//    TMatrixD covariance(5, 5);
//
//    int tracker = h_track->get_tracker();
//    double seed_pos = geom->GetPlanePosition(tracker, 5, 2);
//    double length =  seed_pos;
//    double c  = CLHEP::c_light;
//    double particle_charge = h_track->get_charge();
//    double Bz = geom->GetFieldValue(tracker);
//    double PR_correction = geom->GetPRCorrection(tracker);
//    double PR_bias = geom->GetPRBias();
//
//    // Get h_track values.
//    double r  = h_track->get_R();
//    double pt = - c*Bz*r*particle_charge;
//    double dsdz = - h_track->get_dsdz();
//    double x0 = h_track->get_circle_x0(); // Circle Center x
//    double y0 = h_track->get_circle_y0(); // Circle Center y
//    double s = (h_track->get_line_sz_c() - length*dsdz); // Path length at start plane
//    double phi_0 = s / r; // Phi at start plane
//    double phi = phi_0 + TMath::PiOver2(); // Direction of momentum
//
//    // TODO: Actually propagate the track parrameters and covariance matrix back to start plane.
//    //       This is an approximation.
//    ThreeVector patrec_momentum(-pt*sin(phi_0), pt*cos(phi_0), - pt/dsdz);
//
//    if (correct_energy_loss) {
//      double P = patrec_momentum.mag();
//      double patrec_bias; // Account for two(ish) planes of energy loss
// //      if (tracker == 0) {
// //        patrec_bias = (P + 1.3) / P;
// //      } else {
// //        patrec_bias = (P - 1.3) / P;
// //      }
//      patrec_bias = ((P + PR_correction) + PR_bias) / P;
//      patrec_momentum = patrec_bias * patrec_momentum;
//    }
//
//    double x = x0 + r*cos(phi_0);
//    double px = patrec_momentum.x();
//    double y = y0 + r*sin(phi_0);
//    double py = patrec_momentum.y();
//    double kappa = particle_charge / patrec_momentum.z();
//
//    vector(0, 0) = x;
//    vector(1, 0) = px;
//    vector(2, 0) = y;
//    vector(3, 0) = py;
//    vector(4, 0) = kappa;
//
// //
// // METHOD = ED SANTOS
// //
//    if (seed_cov > 0.0) {
//      covariance.Zero();
//      for (int i = 0; i < 5; ++i) {
//        covariance(i, i) = seed_cov;
//      }
//    } else {
// //
// // METHOD = CHRISTOPHER HUNT
// //
//      std::vector<double> cov = h_track->get_covariance();
//      TMatrixD patrec_covariance(5, 5);
//      if (cov.size() != 25) {
//        throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
//                              "Dimension of covariance matrix does not match the state vector",
//                              "KalmanSeed::ComputeInitalCovariance(Helical)");
//      }
//
//      double mc = particle_charge*c*Bz; // Magnetic constant
//      double sin = std::sin(phi_0);
//      double cos = std::cos(phi_0);
//      double sin_plus = std::sin(phi);
//      double cos_plus = std::cos(phi);
//
//      TMatrixD jacobian(5, 5);
//      jacobian(0, 0) = 1.0;
//      jacobian(0, 2) = cos + phi*sin;
//      jacobian(0, 3) = -sin;
//
//      jacobian(1, 2) = mc*cos_plus + mc*phi*sin_plus;
//      jacobian(1, 3) = -mc*sin_plus;
//
//      jacobian(2, 1) = 1.0;
//      jacobian(2, 2) = sin - phi*cos;
//      jacobian(2, 3) = cos;
//
//      jacobian(3, 2) = mc*sin_plus - mc*phi*cos_plus;
//      jacobian(1, 3) = mc*cos_plus;
//
//      jacobian(4, 3) = -dsdz / (mc*r*r);
//      jacobian(4, 4) = 1.0 / (mc*r);
//
//      TMatrixD jacobianT(5, 5);
//      jacobianT.Transpose(jacobian);
//
//      for (int i = 0; i < 5; ++i) {
//        for (int j = 0; j < 5; ++j) {
//          patrec_covariance(i, j) = cov.at(i*5 + j);
//        }
//      }
//      covariance = jacobian*patrec_covariance*jacobianT;
//    }
//
//    Kalman::State seed_state(vector, covariance);
//    return seed_state;
//  }


//  Kalman::State ComputeSeed(SciFiStraightPRTrack* s_track, const SciFiGeometryHelper* geom,
//                                                                               double seed_cov) {
//    TMatrixD vector(4, 1);
//    TMatrixD covariance(4, 4);
//
//    int tracker = s_track->get_tracker();
//    double seed_pos = geom->GetPlanePosition(tracker, 5, 2);
//    double length = seed_pos;
//
//    double x0 = s_track->get_x0();
//    double y0 = s_track->get_y0();
//    double mx = s_track->get_mx();
//    double my = s_track->get_my();
//
//    // Get position at the starting end of tracker
//    double x = x0 + mx*length;
//    double y = y0 + my*length;
//
//    vector(0, 0) = x;
//    vector(1, 0) = mx;
//    vector(2, 0) = y;
//    vector(3, 0) = my;
//
// //
// // METHOD = ED SANTOS
// //
//    if (seed_cov > 0.0) {
//      covariance.Zero();
//      covariance(0, 0) = seed_cov;
//      covariance(1, 1) = seed_cov/(200.0*200.0); // Rescale for gradients - not momenta!
//      covariance(2, 2) = seed_cov;
//      covariance(3, 3) = seed_cov/(200.0*200.0);
//    } else {
// //
// // METHOD = CHRISTOPHER HUNT
// //
//      std::vector<double> cov = s_track->get_covariance();
//      TMatrixD patrec_covariance(4, 4);
//
//      if (cov.size() != (unsigned int)16) {
//        throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
//                              "Dimension of covariance matrix does not match the state vector",
//                              "KalmanSeed::ComputeInitalCovariance(Straight)");
//      }
//
//      TMatrixD jacobian(4, 4);
//      jacobian(0, 0) = 1.0;
//      jacobian(1, 1) = 1.0;
//      jacobian(2, 2) = 1.0;
//      jacobian(3, 3) = 1.0;
//      jacobian(0, 1) = length;
//      jacobian(2, 3) = length;
//
//      TMatrixD jacobianT(4, 4);
//      jacobianT.Transpose(jacobian);
//
//      for (int i = 0; i < 4; ++i) {
//        for (int j = 0; j < 4; ++j) {
//          patrec_covariance(i, j) = cov.at(i*4 + j);
//        }
//      }
//
//      covariance = jacobian*patrec_covariance*jacobianT;
//    }
//
//    Kalman::State seed_state(vector, covariance);
//    return seed_state;
//  }


  SciFiTrack* ConvertToSciFiTrack(const Kalman::TrackFit* fitter,
                                     const SciFiGeometryHelper* geom, SciFiBasePRTrack* pr_track) {
    SciFiTrack* new_track = new SciFiTrack();
    const Kalman::Track& the_track = fitter->GetTrack();
    Kalman::State seed = fitter->GetSeed();
    double default_mom = geom->GetDefaultMomentum();

    if (the_track.GetLength() < 1)
      throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                            "Not enough points in Kalman Track",
                            "ConvertToSciFiTrack()");

    double chi_squared = fitter->CalculateSmoothedChiSquared();
    int NDF = fitter->GetNumberMeasurements(); // Smoothed ChiSquared NDF = No. Measurements
    double p_value = TMath::Prob(chi_squared, NDF);

    int tracker;
    if (the_track[0].GetId() > 0) {
      tracker = 1;
    } else {
      tracker = 0;
    }
    new_track->set_tracker(tracker);

    int dimension = the_track.GetDimension();
    if (dimension == 4) {
      new_track->SetAlgorithmUsed(SciFiTrack::kalman_straight);
    } else if (dimension == 5) {
      new_track->SetAlgorithmUsed(SciFiTrack::kalman_helical);
    } else {
      throw MAUS::Exceptions::Exception(MAUS::Exceptions::recoverable,
                            "Unexpected dimension of Kalman::Track",
                            "ConvertToSciFiTrack()");
    }

    ThreeVector reference_pos = geom->GetReferencePosition(tracker);
    HepRotation reference_rot = geom->GetReferenceRotation(tracker);
    int charge = pr_track->get_charge();

    for (unsigned int i = 0; i < the_track.GetLength(); ++i) {
      int ID = the_track[i].GetId();
      double position = the_track[i].GetPosition();
      const Kalman::State& smoothed_state = the_track[i].GetSmoothed();
//      const Kalman::State& smoothed_state = the_track[i].GetFiltered();
//      const Kalman::State& smoothed_state = the_track[i].GetPredicted();
      const Kalman::State& data_state = the_track[i].GetData();

      SciFiTrackPoint* new_point = new SciFiTrackPoint();

      new_point->set_tracker(tracker);
      new_point->set_has_data(data_state.HasValue());

      int id = abs(ID);
      new_point->set_station(((id-1)/3)+1);
      new_point->set_plane((id-1)%3);

      ThreeVector pos;
      ThreeVector mom;
      ThreeVector grad;
      ThreeVector error_pos;
      ThreeVector error_mom;
      ThreeVector error_grad;

      TMatrixD state_vector = smoothed_state.GetVector();
      TMatrixD state_covariance = smoothed_state.GetCovariance();

      if (dimension == 4) {
        pos.setZ(position);
        pos.setX(state_vector(0, 0));
        pos.setY(state_vector(2, 0));

        mom.setX(state_vector(1, 0)*default_mom);
        mom.setY(state_vector(3, 0)*default_mom);
        mom.setZ(default_mom);

        pos *= reference_rot;
        pos += reference_pos;
        mom *= reference_rot;

        if (tracker == 0) mom *= -1.0; // Direction of recon is reveresed upstream.
        mom.setZ(fabs(mom.z()));

        grad.SetX(mom.x()/mom.z());
        grad.SetY(mom.y()/mom.z());
        grad.SetZ(0.0);

        error_pos.SetX(std::sqrt(state_covariance(0, 0)));
        error_pos.SetY(std::sqrt(state_covariance(2, 2)));
        error_pos.SetZ(0.0);

        error_mom.SetX(std::sqrt(default_mom*state_covariance(1, 1)));
        error_mom.SetY(std::sqrt(default_mom*state_covariance(3, 3)));
        error_mom.SetZ(200.0);

        error_grad.SetX(std::sqrt(state_covariance(1, 1)));
        error_grad.SetY(std::sqrt(state_covariance(3, 3)));
        error_grad.SetZ(0.0);
      } else if (dimension == 5) {
        pos.setX(state_vector(0, 0));
        pos.setY(state_vector(2, 0));
        pos.setZ(position);

        mom.setX(state_vector(1, 0));
        mom.setY(state_vector(3, 0));
        mom.setZ(fabs(1.0/state_vector(4, 0)));

        pos *= reference_rot;
        pos += reference_pos;

        mom *= reference_rot;
        /*
        if (mom.z() < 0.0) {
          charge = -1;
        } else {
          charge = 1;
        }
        */
        mom.setZ(fabs(mom.z()));

        grad.SetX(mom.x() / mom.z());
        grad.SetY(mom.y() / mom.z());
        grad.SetZ(0.0);

        error_pos.SetX(std::sqrt(state_covariance(0, 0)));
        error_pos.SetY(std::sqrt(state_covariance(2, 2)));
        error_pos.SetZ(0.0);

        error_mom.SetX(std::sqrt(state_covariance(1, 1)));
        error_mom.SetY(std::sqrt(state_covariance(3, 3)));
        error_mom.SetZ(std::sqrt(state_covariance(4, 4)) / (state_vector(4, 0)*state_vector(4, 0)));

        error_grad.SetX(grad.x()*std::sqrt(((error_mom.x()*error_mom.x()) / (mom.x()*mom.x()))
                                              +(error_mom.z()*error_mom.z() / (mom.z()*mom.z()))));
        error_grad.SetY(grad.y()*std::sqrt(((error_mom.y()*error_mom.y()) / (mom.y()*mom.y()))
                                             + (error_mom.z()*error_mom.z() / (mom.z()*mom.z()))));
        error_grad.SetZ(0.0);
      }

      new_point->set_pos(pos);
      new_point->set_mom(mom);
      new_point->set_gradient(grad);
      new_point->set_pos_error(error_pos);
      new_point->set_mom_error(error_mom);
      new_point->set_gradient_error(error_grad);

      // TODO
      // CHARGE!
      if (data_state) {
        Kalman::State pull = fitter->CalculatePull(i);
        double weighted_pull = pull.GetVector()(0, 0) / std::sqrt(pull.GetCovariance()(0, 0));
        new_point->set_pull(weighted_pull);
        new_point->set_residual(fitter->CalculateFilteredResidual(i).GetVector()(0, 0));
        new_point->set_smoothed_residual(fitter->CalculateSmoothedResidual(i).GetVector()(0, 0));
      } else {
        new_point->set_pull(0.0);
        new_point->set_residual(0.0);
        new_point->set_smoothed_residual(0.0);
      }


      TMatrixD C = smoothed_state.GetCovariance();
      int size = C.GetNrows();
      int num_elements = size*size;

      double* matrix_elements = C.GetMatrixArray();
      std::vector<double> covariance(num_elements);
      for (int i = 0; i < num_elements; ++i) {
        covariance[i] = matrix_elements[i];
      }
      new_point->set_covariance(covariance);
      std::vector<double> errors(size);
      for (int i = 0; i < size; ++i) {
        errors[i] = std::sqrt(fabs(C(i, i)));
      }
      new_point->set_errors(errors);

      new_track->add_scifitrackpoint(new_point);
    }

    new_track->set_charge(charge);
    new_track->set_chi2(chi_squared);
    new_track->set_ndf(NDF);
    new_track->set_P_value(p_value);
    new_track->set_pr_track_pointer(pr_track);

    TMatrixD seed_vector = seed.GetVector();
    ThreeVector seed_pos;
    ThreeVector seed_mom;
    if (dimension == 4) {
      seed_pos.setX(seed_vector(0, 0));
      seed_mom.setX(seed_vector(1, 0)*default_mom);
      seed_pos.setY(seed_vector(2, 0));
      seed_mom.setY(seed_vector(3, 0)*default_mom);
      seed_pos.setZ(the_track.GetLastTrackPoint().GetPosition());
      seed_mom.setZ(default_mom);
    } else if (dimension == 5) {
      seed_pos.setX(seed_vector(0, 0));
      seed_mom.setX(seed_vector(1, 0));
      seed_pos.setY(seed_vector(2, 0));
      seed_mom.setY(seed_vector(3, 0));
      seed_pos.setZ(the_track.GetLastTrackPoint().GetPosition());
      seed_mom.setZ(fabs(1.0/seed_vector(4, 0)));
    }
    seed_pos *= reference_rot;
    seed_pos += reference_pos;

    seed_mom *= reference_rot;

//    if (tracker == 0) seed_mom *= -1.0; // Direction of recon is reveresed upstream.
    seed_mom.setZ(fabs(seed_mom.z()));

    new_track->SetSeedPosition(seed_pos);
    new_track->SetSeedMomentum(seed_mom);
    new_track->SetSeedCovariance(seed.GetCovariance().GetMatrixArray(), dimension*dimension);

    return new_track;
  }


  Kalman::Track BuildSpacepointTrack(SciFiSpacePointPArray spacepoints,
                           const SciFiGeometryHelper* geom, int dim, int plane_num, double smear) {
    Kalman::Track new_track(dim);
    int tracker = (*spacepoints.begin())->get_tracker();

    for (unsigned int i = 0; i < 5; ++i) {
      int id = (1 + i*3 + plane_num)*(tracker == 0 ? -1 : 1);
      Kalman::TrackPoint new_trackpoint(dim, geom->GetPlanePosition(tracker, i+1, plane_num), id);
      new_track.Append(new_trackpoint);
    }

    double variance = geom->GetChannelWidth() * geom->GetChannelWidth() / 12.0;

    for (SciFiSpacePointPArray::iterator it = spacepoints.begin(); it != spacepoints.end(); ++it) {
      int station = (*it)->get_station();
      TMatrixD vec(2, 1);
      TMatrixD cov(2, 2);
      cov.Zero();
      vec(0, 0) = (*it)->get_position().x();
      vec(1, 0) = (*it)->get_position().y();
      cov(0, 0) = variance;
      cov(1, 1) = variance;
      new_track[station-1].SetData(Kalman::State(vec, cov));
      new_track[station-1].SetPosition((*it)->get_position().z());
    }
    return new_track;
  }
}

