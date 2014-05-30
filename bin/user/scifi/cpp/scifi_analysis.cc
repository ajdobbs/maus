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
 *
 */

// C / C++ headers
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>

// ROOT headers
#include "TCanvas.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGClient.h"

#include "src/common_cpp/JsonCppStreamer/IRStream.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiTrack.hh"
#include "src/common_cpp/DataStructure/SciFiTrackPoint.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiChannelId.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/Track.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/Recon/SciFi/SciFiTools.hh"

/** Access Tracker data using ROOT */

int main(int argc, char *argv[]) {
  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Set up ROOT app, input file, and MAUS data class
  TApplication theApp("App", &argc, argv);
  std::cout << "Opening file " << filename << std::endl;
  irstream infile(filename.c_str(), "Spill");
  MAUS::Data data;

  // Loop over all spills
  while ( infile >> readEvent != NULL ) {
    infile >> branchName("data") >> data;
    MAUS::Spill* spill = data.GetSpill();
    if (spill == NULL || spill->GetDaqEventType() != "physics_event") {
      std::cout << "Not a usable spill\n";
      continue;
    }

    std::cout << "Spill: " << spill->GetSpillNumber() << "\n";
    std::vector<MAUS::ReconEvent*>* revts = spill->GetReconEvents();
    std::vector<MAUS::MCEvent*>* mevts = spill->GetMCEvents();

    // Loop over MC events
    for ( size_t i = 0; i < mevts->size(); ++i ) {
      if ( !mevts->at(i) || !revts->at(i) ) {
        std::cerr << "Bad event, skipping\n";
        continue;
      }

      // Create the lookup providing a link between recon digits and MC hits
      MAUS::SciFiLookup lkup;
      lkup.make_hits_map(mevts->at(i));

      // Pull out the scifi event
      MAUS::SciFiEvent* sfevt = revts->at(i)->GetSciFiEvent();

      // Perform your analysis here, e.g.
      std::vector<MAUS::SciFiTrack*> trks = sfevt->scifitracks();
      std::vector<MAUS::SciFiTrack*>::iterator trk;

      // Loop over kalman tracks
      for ( trk = trks.begin(); trk != trks.end(); ++trk ) {
        std::vector<MAUS::SciFiTrackPoint*> tpnts = (*trk)->scifitrackpoints();
        std::vector<MAUS::SciFiTrackPoint*>::iterator tpnt;

        // Loop over trackpoints
        for ( tpnt = tpnts.begin(); tpnt != tpnts.end(); ++tpnt ) {
          MAUS::SciFiCluster* clus = (*tpnt)->cluster();
          if (!clus) {
            std::cout << "Empty cluster pointer, address " << clus  << std::endl;
            continue;
          }
          std::vector<MAUS::SciFiDigit*> digits = clus->get_digits();
          std::vector<MAUS::SciFiDigit*>::iterator dig;

          // Loop over digits
          for ( dig = digits.begin(); dig != digits.end(); ++dig ) {

            // Perform the digits to hits lookup
            std::vector<MAUS::SciFiHit*> hits;
            std::vector<MAUS::SciFiHit*>::iterator hit;

            if (!lkup.get_hits((*dig), hits)) {
              std::cerr << "Lookup failed\n";
              continue;
            }

            // Loop over MC hits
            for ( hit = hits.begin(); hit != hits.end(); ++hit ) {
              int track_id = (*hit)->GetTrackId();
              std::cout << "track_id: " << track_id << std::endl;
            } // ~Loop over MC hits
          } // ~Loop over digits
        } // ~Loop over track points
      } // ~Loop over kalman tracks
    } // ~Loop over MC events
  } // ~Loop over all spills
  theApp.Run();
}
