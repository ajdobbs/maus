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

// ROOT headers
#include "TCanvas.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGClient.h"
#include "TCut.h"

// MAUS headers
#include "src/common_cpp/Plotting/SciFi/TrackerDataAnalyserMomentum.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/JsonCppStreamer/IRStream.hh"

/** Analyse Tracker momentum data using ROOT */


int main(int argc, char *argv[]) {

  // First argument to code should be the input ROOT file name
  std::string filename = std::string(argv[1]);

  // Analysis parameters - some may be overidden with command line arguments. All momenta in MeV/c.
  int n_bins_pz_resol = 0;    // No. of bins in histos used to find pz resols (0 = let ROOT decide)
  int n_pz_points = 9;        // No. of data points in each pz resolution plot
  double pz_fit_min = -50.0;    // Lower bound of the gaussian fit to histos used to find pz resols
  double pz_fit_max = 50.0;    // Upper bound of the gaussian fit to histos used to find pz resols
  double pz_lower_bound = 0;  // Lower bound on pt_mc of pz resolution plots
  double pz_upper_bound = 90; // Upper bound on pt_mc of pz resolution plots
  double pz_rec_cut = 500.0;  // Cut on reconstruction pz applied to histos used to find pz resols

  // Parse any extra arguments supplied from the command line
  //   -p -> pause between events
  //   -g -> enables saving xyz plots and gives output graphics file type
  bool bool_pause = false;
  bool bool_save = false;
  std::string save_type = "";

  for (int i = 2; i < argc; i++) {
    if ( std::strcmp(argv[i], "-p") == 0 ) {
      std::cout << "Will wait for user input between spills\n";
      bool_pause = true;
    } else if ( std::strcmp(argv[i], "-g") == 0 ) {
      if ( (i+1) < argc ) save_type = argv[i + 1];
      if ( (save_type == "eps") || (save_type == "pdf") || (save_type == "png") ) {
        std::cout << "Saving plots as " << save_type << " files.\n";
        bool_save = true;
      } else if ( save_type != "" ) {
        std::cerr << "Invalid graphics output type given\n";
      }
    } else if ( std::strcmp(argv[i], "-n_bins_pz_resol") == 0 ) {
      if ( (i+1) < argc ) {
        std::stringstream ss1(argv[i + 1]);
        ss1 >> n_bins_pz_resol;
      }
    }
  }
  if ( n_bins_pz_resol == 0 ) {
    std::cout << "Using ROOT auto binning for pz resolution histos" << std::endl;
  } else {
    std::cout << "Pz resolution histos set to have number of bins = " << n_bins_pz_resol << "\n";
  }

  // Set up analysis class
  MAUS::TrackerDataAnalyserMomentum analyser;
  analyser.set_n_bins(n_bins_pz_resol);
  analyser.set_n_pz_points(n_pz_points);
  analyser.set_pz_fit_min(pz_fit_min);
  analyser.set_pz_fit_max(pz_fit_max);
  analyser.set_pz_lower_bound(pz_lower_bound);
  analyser.set_pz_upper_bound(pz_upper_bound);
  analyser.set_cutPzRec(pz_rec_cut);
  std::cout << "Pz resolution histogram fit lower bound: " << analyser.get_pz_fit_min() << "\n";
  std::cout << "Pz resolution histogram fit upper bound: " << analyser.get_pz_fit_max() << "\n";
  std::cout << "Pz resolution graphs number of points:  " << analyser.get_n_pz_points() << "\n";
  std::cout << "Pz resolution graphs lower bound:  " << analyser.get_pz_lower_bound() << " MeV/c\n";
  std::cout << "Pz resolution graphs upper bound:  " << analyser.get_pz_upper_bound() << " MeV/c\n";
  std::cout << "Pz rec cut: " << analyser.get_cutPzRec() << " MeV/c\n";
  analyser.setUp();

  // Set up ROOT app, input file, and MAUS data class
  TApplication theApp("App", &argc, argv);
  std::cout << "Opening file " << filename << std::endl;
  irstream infile(filename.c_str(), "Spill");
  MAUS::Data data;

  // Loop over all spills
  int counter = 0;
  while ( infile >> readEvent != NULL ) {
    ++counter;
    infile >> branchName("data") >> data;
    MAUS::Spill* spill = data.GetSpill();
    if (spill != NULL && spill->GetDaqEventType() == "physics_event") {
      std::cout << "Total number of spills: " << counter << ".  ";
      std::cout << "Spill number: " << spill->GetSpillNumber() << std::endl;
      analyser.accumulate(spill);
      if (bool_pause) {
        std::cout << "Press Enter to Continue";
        std::cin.ignore();
      }
    } else {
      std::cout << "Not a usable spill\n";
    }
  }

  // Make the final plots
  analyser.make_residual_histograms();
  analyser.make_residual_graphs();
  analyser.make_pz_resolutions();
  analyser.make_resolution_graphs();
  analyser.save_graphics(save_type);
  analyser.save_root();

  // Tidy up
  infile.close();
  theApp.Run();
}
