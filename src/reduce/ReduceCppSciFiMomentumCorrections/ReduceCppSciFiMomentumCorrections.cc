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

#include <string>
#include <sstream>

#include "src/reduce/ReduceCppSciFiMomentumCorrections/ReduceCppSciFiMomentumCorrections.hh"

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "Interface/dataCards.hh"
#include "Utils/Squeak.hh"

#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

#include "src/legacy/Config/MiceModule.hh"

namespace MAUS {

  ReduceCppSciFiMomentumCorrections::~ReduceCppSciFiMomentumCorrections() {
    }

  void ReduceCppSciFiMomentumCorrections::_birth(const std::string& argJsonConfigDocument) {
    _classname = "ReduceCppSciFiMomentumCorrections";
    _corrections_filename = "SciFiMomentumCorrections.root";

    // JsonCpp setup - check file parses correctly, if not return false
    Json::Value _configJSON;
    try {
      _configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    } catch (Exceptions::Exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleExceptionNoJson(exc, _classname);
    } catch (std::exception& exc) {
      MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
    }

    // Find the correct virtual planes
    MiceModule* module = Globals::GetReconstructionMiceModules();
    std::vector<const MiceModule*> modules =
      module->findModulesByPropertyString("SensitiveDetector", "SciFi");
    _geometry_helper = SciFiGeometryHelper(modules);
    _geometry_helper.Build();

    double upstream_z = _geometry_helper.FindPlane(0, 5, 2)->globalPosition().z();
    double downstream_z = _geometry_helper.FindPlane(1, 5, 2)->globalPosition().z();

    _upstream_virtual_plane = _findVirtualPlane(upstream_z);
    _downstream_virtual_plane = _findVirtualPlane(downstream_z);

    // Setup some histograms to fill
    _up_residual_p = TH2F( "SMC_up_residual_p", "Upstream p Residual vs p",
                                                           200, 100.0, 300.0, 200, -100.0, 100.0 );
    _up_residual_pz = TH2F( "SMC_up_residual_pz", "Upstream p_{z} Residual vs p_{z}", 
                                                           200, 100.0, 300.0, 200, -100.0, 100.0 );
    _down_residual_p = TH2F( "SMC_down_residual_p", "Downstream p Residual vs p",
                                                           200, 100.0, 300.0, 200, -100.0, 100.0 );
    _down_residual_pz = TH2F( "SMC_down_residual_pz", "Downstream p_{z} Residual vs p_{z}",
                                                           200, 100.0, 300.0, 200, -100.0, 100.0 );

    _up_p_residual_pt = TH2F( "SMC_up_p_residual_pt", "Upstream p Residual vs p_{#perp}",
                                                           200, 0.0, 200.0, 200, -100.0, 100.0 );
    _down_p_residual_pt = TH2F( "SMC_down_p_residual_pt", "Downstream p Residual vs p_{#perp}",
                                                           200, 0.0, 200.0, 200, -100.0, 100.0 );
    _up_p_residual_pz = TH2F( "SMC_up_p_residual_pz", "Upstream p Residual vs p_{z}",
                                                           200, 100.0, 300.0, 200, -100.0, 100.0 );
    _down_p_residual_pz = TH2F( "SMC_down_p_residual_pz", "Downstream p Residual vs p_{z}",
                                                           200, 100.0, 300.0, 200, -100.0, 100.0 );
  }


  void ReduceCppSciFiMomentumCorrections::_process(MAUS::Data* data_cpp) {
    if (data_cpp == NULL)
      throw Exceptions::Exception(Exceptions::recoverable, "Data was NULL",
		      "ReduceCppMCProp::_process");
    if (data_cpp->GetSpill() == NULL)
      throw Exceptions::Exception(Exceptions::recoverable, "Spill was NULL",
		      "ReduceCppMCProp::_process");
    if (data_cpp->GetSpill()->GetDaqEventType() != "physics_event") {
    }

    /*if (data_cpp->GetSpill()->GetReconEvents() == NULL)
        throw Exceptions::Exception(Exceptions::recoverable, "ReconEvents were NULL",
	"ReduceCppSciFiMomentumCorrections::_process");*/

    _spill = data_cpp->GetSpill();

    if (_spill) {
      if ( _spill->GetReconEvents() ) {
        for ( unsigned int event_i = 0;
              event_i < _spill->GetReconEvents()->size(); ++event_i) {

          SciFiEvent* scifi_event = _spill->GetAReconEvent(event_i).GetSciFiEvent();
          MCEvent* mc_event = _spill->GetMCEvents()->at(event_i);

          if ( (!scifi_event) || (!mc_event) ) {
            continue;
          }

          SciFiHelicalPRTrack* upstream_track = 0;
          SciFiHelicalPRTrack* downstream_track = 0;
          if (!_getHelicalTracks(scifi_event, upstream_track, downstream_track)) {
            std::cerr << "HERE1 : " << upstream_track << " | " << downstream_track << "\n";
            continue;
          }

          VirtualHit* upstream_truth = 0;
          VirtualHit* downstream_truth = 0;
          if (!_getMCTracks(mc_event, upstream_truth, downstream_truth)) {
            std::cerr << "HERE2 : " << upstream_truth << " | " << downstream_truth << "\n";
            continue;
          }

          double up_pr_px = upstream_track->get_seed_momentum().x(); // LOCAL TO TRACKER COORDINATES!!!
          double up_pr_py = upstream_track->get_seed_momentum().y(); // FIX THIS!
          double up_pr_pz = upstream_track->get_seed_momentum().z();
          double up_pr_pt = sqrt(up_pr_px*up_pr_px + up_pr_py*up_pr_py);
          double up_pr_p = sqrt(up_pr_pt*up_pr_pt + up_pr_pz*up_pr_pz);
          double up_true_px = upstream_truth->GetMomentum().x();
          double up_true_py = upstream_truth->GetMomentum().y();
          double up_true_pz = upstream_truth->GetMomentum().z();
          double up_true_pt = sqrt(up_true_px*up_true_px + up_true_py*up_true_py);
          double up_true_p = sqrt(up_true_pt*up_true_pt + up_true_pz*up_true_pz);

          _up_residual_p.Fill( up_true_p, up_pr_p-up_true_p);
          _up_residual_pz.Fill( up_true_pz, -up_pr_pz+up_true_pz);
          _up_p_residual_pt.Fill( up_true_pt, up_pr_p-up_true_p);
          _up_p_residual_pz.Fill( up_true_pz, up_pr_p-up_true_p);

          double down_pr_px = downstream_track->get_seed_momentum().x();
          double down_pr_py = downstream_track->get_seed_momentum().y();
          double down_pr_pz = downstream_track->get_seed_momentum().z();
          double down_pr_pt = sqrt(down_pr_px*down_pr_px + down_pr_py*down_pr_py);
          double down_pr_p = sqrt(down_pr_pt*down_pr_pt + down_pr_pz*down_pr_pz);
          double down_true_px = downstream_truth->GetMomentum().x();
          double down_true_py = downstream_truth->GetMomentum().y();
          double down_true_pz = downstream_truth->GetMomentum().z();
          double down_true_pt = sqrt(down_true_px*down_true_px + down_true_py*down_true_py);
          double down_true_p = sqrt(down_true_pt*down_true_pt + down_true_pz*down_true_pz);

          _down_residual_p.Fill( down_true_p, down_pr_p-down_true_p);
          _down_residual_pz.Fill( down_true_pz, down_pr_pz-down_true_pz);
          _down_p_residual_pt.Fill( down_true_pt, down_pr_p-down_true_p);
          _down_p_residual_pz.Fill( down_true_pz, down_pr_p-down_true_p);

          delete upstream_track;
          delete downstream_track;
          delete upstream_truth;
          delete downstream_truth;
      	}
      }
    } else {
      Squeak::mout(Squeak::error) << "Failed to import spill from data\n";
    } // check for spill
  }


  void ReduceCppSciFiMomentumCorrections::_death() {
    TFile outfile(_corrections_filename.c_str(), "RECREATE");
    _up_residual_p.Write();
    _up_residual_pz.Write();
    _down_residual_p.Write();
    _down_residual_pz.Write();

    _up_p_residual_pt.Write();
    _up_p_residual_pz.Write();
    _down_p_residual_pt.Write();
    _down_p_residual_pz.Write();
    outfile.Close();
  }


  bool ReduceCppSciFiMomentumCorrections::_getHelicalTracks(SciFiEvent* event, 
                                            SciFiHelicalPRTrack*& up, SciFiHelicalPRTrack*& down) {
    SciFiHelicalPRTrackPArray tracks = event->helicalprtracks();
    if (tracks.size() == 2 ) {
      for (unsigned int i = 0; i < 2; ++i) {
        if (tracks[i]->get_tracker() == 0) {
          up = new SciFiHelicalPRTrack(*tracks[i]);
        } else {
          down = new SciFiHelicalPRTrack(*tracks[i]);
        }
      }
      if (up && down) {
        return true;
      }
    }

    if (up != 0) {
      delete up;
      up = 0;
    }
    if (down != 0) {
      delete down;
      down = 0;
    }

    return false;
  }


  bool ReduceCppSciFiMomentumCorrections::_getMCTracks(MCEvent* event, 
                                                              VirtualHit*& up, VirtualHit*& down) {
    VirtualHitArray* virts = event->GetVirtualHits();

    for (VirtualHitArray::iterator it = virts->begin(); it != virts->end(); ++it ) {
      if (it->GetStationId() == _upstream_virtual_plane) {
        up = new VirtualHit((*it));
      }
      if (it->GetStationId() == _downstream_virtual_plane) {
        down = new VirtualHit((*it));
      }
      if (up && down) {
        return true;
      }
    }

    if (up != 0) {
      delete up;
      up = 0;
    }
    if (down != 0) {
      delete down;
      down = 0;
    }
    
    return false;
  }


  int ReduceCppSciFiMomentumCorrections::_findVirtualPlane(double z_pos) {
    int closest_station = 0;
    double separation = 1.0e300;
    int station_counter = 1;

    MAUSGeant4Manager* g4manager = Globals::GetGeant4Manager();
    VirtualPlaneManager* vpmanager = g4manager->GetVirtualPlanes();
    std::vector<VirtualPlane*> virtual_planes = vpmanager->GetPlanes();

    for (std::vector<VirtualPlane*>::const_iterator it = virtual_planes.begin();
                                                                it != virtual_planes.end(); ++it) {

      double test = fabs((*it)->GetPosition().z() - z_pos);

      if (test < separation) {
        separation = test;
        closest_station = station_counter;
      }

      station_counter += 1;
    }

    return closest_station;
  }


  PyMODINIT_FUNC init_ReduceCppSciFiMomentumCorrections(void) {
    PyWrapReduceBase<ReduceCppSciFiMomentumCorrections>::PyWrapReduceBaseModInit(
                                  "ReduceCppSciFiMomentumCorrections", "", "", "", "");
}
} // ~namespace MAUS
