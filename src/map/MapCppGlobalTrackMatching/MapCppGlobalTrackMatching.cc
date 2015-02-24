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

#include "src/map/MapCppGlobalTrackMatching/MapCppGlobalTrackMatching.hh"

#include "src/common_cpp/DataStructure/Data.hh"
#include "Interface/Squeak.hh"
#include "src/common_cpp/Converter/DataConverters/JsonCppSpillConverter.hh"
#include "src/common_cpp/Converter/DataConverters/CppJsonSpillConverter.hh"
#include "src/common_cpp/API/PyWrapMapBase.hh"


namespace MAUS {

  PyMODINIT_FUNC init_MapCppGlobalTrackMatching(void) {
    PyWrapMapBase<MAUS::MapCppGlobalTrackMatching>::PyWrapMapBaseModInit
                                    ("MapCppGlobalTrackMatching", "", "", "", "");
  }

  MapCppGlobalTrackMatching::MapCppGlobalTrackMatching()
    : MapBase<Data>("MapCppGlobalTrackMatching"), _configCheck(false) {
  }

  void MapCppGlobalTrackMatching::_birth(const std::string& argJsonConfigDocument) {
    // Check if the JSON document can be parsed, else return error only.
    _configCheck = false;
    bool parsingSuccessful = _reader.parse(argJsonConfigDocument, _configJSON);
    if (!parsingSuccessful) {
        throw MAUS::Exception(Exception::recoverable,
                              "Failed to parse configuration",
                              "MapCppGlobalTrackMatching::birth");
    }
    _configCheck = true;
    _classname = "MapCppGlobalTrackMatching";
  }

  void MapCppGlobalTrackMatching::_death() {
  }

  void MapCppGlobalTrackMatching::_process(MAUS::Data* data_cpp) const {
    // Read string and convert to a Json object
    if (!data_cpp) {
        throw MAUS::Exception(Exception::recoverable,
                              "data_cpp was NULL",
                              "MapCppGlobalTrackMatching::_process");
    }
    if (!_configCheck) {
        throw MAUS::Exception(Exception::recoverable,
                              "Birth has not been successfully called",
                              "MapCppGlobalTrackMatching::_process");
    }
    const MAUS::Spill* spill = data_cpp->GetSpill();

    MAUS::ReconEventPArray* recon_events = spill->GetReconEvents();
    //~ MAUS::ReconEventPArray::iterator recon_event_iter;
    //~ for (recon_event_iter = recon_events->begin();
         //~ recon_event_iter != recon_events->end(); ++recon_event_iter) {
      //~ MAUS::ReconEvent* recon_event = (*recon_event_iter);
      //~ MAUS::SciFiTrackPArray* scifi_hits = recon_event->GetVirtualHits();
      //~ MAUS::SciFiTrackPArray::iterator scifi_hits_iter;
      //~ for (scifi_hits_iter = scifi_hits->begin();
           //~ scifi_hits_iter != scifi_hits->end(); ++scifi_hits_iter) {
        //~ ThreeVector pos = (*scifi_hits_iter).GetPosition();
        //~ ThreeVector mom = (*scifi_hits_iter).GetMomentum();
        //~ if (floor(pos.Z()) == 2773 or floor(pos.Z()) == 10572) {
          //~ Squeak::mout(Squeak::error) << "TRUTH\n" << pos.X() << "\t" << pos.Y() << "\t" << pos.Z() << "\t" << mom.X() << "\t" << mom.Y() << "\t" << mom.Z() << "\n";
        //~ }
      //~ }
    //~ }

    //~ MAUS::MCEventPArray* mc_events = spill->GetMCEvents();
    //~ MAUS::MCEventPArray::iterator mc_event_iter;
    //~ for (mc_event_iter = mc_events->begin();
         //~ mc_event_iter != mc_events->end(); ++mc_event_iter) {
      //~ MAUS::MCEvent* mc_event = (*mc_event_iter);
      //~ MAUS::VirtualHitArray* virt_hits = mc_event->GetVirtualHits();
      //~ MAUS::VirtualHitArray::iterator virt_hits_iter;
      //~ for (virt_hits_iter = virt_hits->begin();
           //~ virt_hits_iter != virt_hits->end(); ++virt_hits_iter) {
        //~ ThreeVector pos = (*virt_hits_iter).GetPosition();
        //~ ThreeVector mom = (*virt_hits_iter).GetMomentum();
        //~ int id = (*virt_hits_iter).GetParticleId();
        //~ if (floor(pos.Z()) == 2773 or floor(pos.Z()) == 10572) {
        //~ if (floor(pos.Z()) == 11556) {
          //~ Squeak::mout(Squeak::error) << "TRUTH\n" << pos.X() << "\t" << pos.Y() << "\t" << pos.Z() << "\t" << mom.X() << "\t" << mom.Y() << "\t" << mom.Z() << " " << id << "\n";
        //~ }
      //~ }
    //~ }
    if (!recon_events) {
      return;
    }

    MAUS::GlobalEvent* global_event;
    MAUS::ReconEventPArray::iterator recon_event_iter;
    for (recon_event_iter = recon_events->begin();
   recon_event_iter != recon_events->end();
   ++recon_event_iter) {
      // Load the ReconEvent, and import it into the GlobalEvent
      MAUS::ReconEvent* recon_event = (*recon_event_iter);
      global_event = MakeTracks(recon_event);

    }
  }

  MAUS::GlobalEvent*
  MapCppGlobalTrackMatching::MakeTracks(MAUS::ReconEvent* recon_event) const {
    if (!recon_event) {
      throw(Exception(Exception::recoverable,
          "Trying to use an empty recon event.",
          "MapCppGlobalTrackMatching::MakeTracks"));
    }

    MAUS::GlobalEvent* global_event = recon_event->GetGlobalEvent();
    //~ Squeak::mout(Squeak::error) << global_event << " GE\n";

    MAUS::recon::global::TrackMatching track_matching;
    //~ Squeak::mout(Squeak::error) << "###FormTracks###\n";
    track_matching.USTrack(global_event, _classname);
    track_matching.DSTrack(global_event, _classname);
    track_matching.throughTrack(global_event, _classname);
    return global_event;
  }
} // ~MAUS
