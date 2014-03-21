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

// Gean4
#include "Geant4/G4Step.hh"

// MAUS
#include "DetModel/EMR/EMRSD.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

/**
* SAVING DATA IN JSON FILES
**/

// EMRSD::EMRSD(MiceModule* mod) : MAUSSD(mod) {
// }
//
// void EMRSD::Initialize(G4HCofThisEvent* HCE)
// {
// }
//
//
// G4bool EMRSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) {
//
//   G4double edep = aStep->GetTotalEnergyDeposit();
//
//   int pid = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
//
//   if ( edep == 0. ) return false;
//   if ( fabs(pid) != 13 ) return false;
//
//   if (!_hits.isMember("emr_hits")) {
//     _hits["emr_hits"] = Json::Value(Json::arrayValue);
//   }
//   int hit_i = _hits["emr_hits"].size();
//   _hits["emr_hits"].append(Json::Value());

//   std::cout << "EMRSD : " << hit_i << std::endl ;

//   G4TouchableHandle theTouchable = aStep->GetPreStepPoint()->GetTouchableHandle();
//   G4int barNumber = theTouchable->GetCopyNumber();  // get the fiber copy number
//   G4ThreeVector Pos = aStep->GetPreStepPoint()->GetPosition();  // true MC position
//   G4ThreeVector Mom = aStep->GetTrack()->GetMomentum();  // true momentum

//   Json::Value channel_id;
//    channel_id["bar_number"] = barNumber;
//    //  channel_id["bar_number"] = _module->propertyInt("Bar");

//   _hits["emr_hits"][hit_i]["channel_id"] = channel_id;
//   _hits["emr_hits"][hit_i]["track_id"] = aStep->GetTrack()->GetTrackID();
//   _hits["emr_hits"][hit_i]["energy"] = aStep->GetTrack()->GetTotalEnergy();
//   _hits["emr_hits"][hit_i]["charge"] =
//                              aStep->GetTrack()->GetDefinition()->GetPDGCharge();
//   _hits["emr_hits"][hit_i]["particle_id"] = pid;
//   _hits["emr_hits"][hit_i]["time"] =
//                                      aStep->GetPreStepPoint()->GetGlobalTime();
//   _hits["emr_hits"][hit_i]["energy_deposited"] = edep;
//   _hits["emr_hits"][hit_i]["momentum"]["x"] = Mom.x();
//   _hits["emr_hits"][hit_i]["momentum"]["y"] = Mom.y();
//   _hits["emr_hits"][hit_i]["momentum"]["z"] = Mom.z();
//   _hits["emr_hits"][hit_i]["position"]["x"] = Pos.x();
//   _hits["emr_hits"][hit_i]["position"]["y"] = Pos.y();
//   _hits["emr_hits"][hit_i]["position"]["z"] = Pos.z();

//   return true;
// }

// void EMRSD::EndOfEvent(G4HCofThisEvent* HCE) {
//   // do nothing
//  }


/**
* SAVING DATA IN CPP FILES (TO BE CONVERTED LATER TO JSON OR ROOT) 
**/

EMRSD::EMRSD(MiceModule* mod) : MAUSSD(mod) {
  int bar = _module->propertyInt("Cell");
  int plane = _module->propertyInt("Layer");
  _ch_id.SetBar(bar);
  _ch_id.SetPlane(plane);
  _hit_cppdata.SetChannelId(&_ch_id);
}

void EMRSD::Initialize(G4HCofThisEvent* HCE) {

  _hit_cppdata.SetEnergyDeposited(0.);
  _hit_cppdata.SetPathLength(0.);

  _Edep   = 0.;
  _path   = 0.;
  _nSteps = 0;
}

G4bool EMRSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) {

  G4Track* track = aStep->GetTrack();


  if (_nSteps == 0) {
    MAUS::ThreeVector pos(aStep->GetPreStepPoint()->GetPosition().x(),
                          aStep->GetPreStepPoint()->GetPosition().y(),
                          aStep->GetPreStepPoint()->GetPosition().z());
    MAUS::ThreeVector mom(track->GetMomentum().x(),
                          track->GetMomentum().y(),
                          track->GetMomentum().z());

    _hit_cppdata.SetTrackId(track->GetTrackID());
    _hit_cppdata.SetParticleId(track->GetDefinition()->GetPDGEncoding());
    _hit_cppdata.SetEnergy(track->GetTotalEnergy());
    _hit_cppdata.SetCharge(track->GetDefinition()->GetPDGCharge());
    _hit_cppdata.SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    _hit_cppdata.SetPosition(pos);
    _hit_cppdata.SetMomentum(mom);
  }

  _Edep += aStep->GetTotalEnergyDeposit();
  _path += aStep->GetStepLength();
  _nSteps++;

  return true;
}

void EMRSD::EndOfEvent(G4HCofThisEvent* HCE) {

  if (_nSteps) {
    if (!_hits.isMember("emr_hits")) {
      _hits["emr_hits"] = Json::Value(Json::arrayValue);
    }

//     std::cerr << "hit in plane: " <<  _ch_id.GetPlane()
//     << "  bar: " <<  _ch_id.GetBar() << std::endl;
    _hit_cppdata.SetEnergyDeposited(_Edep);
    _hit_cppdata.SetPathLength(_path);
    _hits["emr_hits"].append(*_hit_proc.CppToJson(_hit_cppdata, ""));
//     std::cerr << _hits["emr_hits"] << std::end;
  }
}
