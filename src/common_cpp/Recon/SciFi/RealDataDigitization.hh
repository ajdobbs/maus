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

/** @class RealDataDigitization 
 *  D...
 *
 */

#ifndef REALDATADIGITIZATION_HH
#define REALDATADIGITIZATION_HH
// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>

// G4MICE from commonCpp
#include "Interface/dataCards.hh"
#include "Interface/MICEEvent.hh"

// from old file
#include "Config/MiceModule.hh"

#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"

namespace MAUS {

struct ChanMap {
  /* \struct ChanMap A struct to hold scifi channel map data */
  int UId;
  int tracker;
  int station;
  int plane;
  int channel;
  int board;
  int bank;
  int chan_ro;
  int extWG;
  int inWG;
  int WGfib;
  /* Default Constructor */
  ChanMap() : UId(-1),
              tracker(-1),
              station(-1),
              plane(-1),
              channel(-1),
              board(-1),
              bank(-1),
              chan_ro(-1),
              extWG(-1),
              inWG(-1),
              WGfib(-1) {
    // Do nothing
  };
};

// typedef std::map<int, ChanMap> ChanMapLookup;
typedef std::vector<ChanMap> ChanMapLookup;

class RealDataDigitization {
 public:
  typedef std::vector<int> IntChannelArray;

  RealDataDigitization();

  ~RealDataDigitization();

  void initialise(double npe_cut,
                  const std::string& map_file,
                  const std::string& calib_file,
                  const std::string& bad_channels_file);

  /** @brief Processes a spill from DAQ
   *  @params spill A SciFiSpill to be filled
   */
  void process(Spill *spill);

  /** @brief Reads the VLSB data
   *  @params input_event The JSON input vlsb event to be processed.
   *  @params event The SciFiEvent to be filled.
   *  @params TrackerDaq The TrackerDaq object to be created for T0.
   *  @params TrackerDaq The TrackerDaq object to be created for T1.
   */
  std::vector<SciFiDigit*> process_VLSB(int SpillNum, TrackerDaq* td);

  /** @brief Reads the VLSB data in the cosmic run format
   *  @params input_event The JSON input vlsb event to be processed.
   *  @params event The SciFiEvent to be filled.
   *  @params TrackerDaq The TrackerDaq object to be created for T0.
   *  @params TrackerDaq The TrackerDaq object to be created for T1.
   */
  void process_VLSB_c(Json::Value input_event,
                      SciFiEvent* event,
                      TrackerDaq *tracker0daq_event,
                      TrackerDaq *tracker1daq_event);

  /** @brief Reads in the calibration.
   */
  bool load_calibration(std::string filename);

  /** @brief Make a unique channel number across all banks and boards.
   */
  int calc_uid(int chan_ro, int bank, int board) const;

  /** @brief Loads the mapping.
   */
  bool load_mapping(std::string file);

  /** @brief Converts read-out map into SciFi Fibre map
   */
  bool get_StatPlaneChannel(int &board, int &bank, int &chan_ro,
                            int &tracker, int &station, int &plane, int &channel,
                            int &extWG, int &inWG, int &WGfib);

  /** @brief Reads the bad channel list from file.
   */
  bool load_bad_channels(std::string file);

  /** @brief Returns value depends on the goodness of the channel.
   */
  bool is_good_channel(int bank, int chan_ro) const;

 private:
  static const int _number_channels       = 128;
  static const int _number_banks          = 64;
  static const int _number_boards         = 16;
  static const int _total_number_channels = 6403;
  static const int _banks_per_board       = 4;
  static const int _n_entries         = 8192;
  double _npe_cut;
  // static const double _min       = 0.000000001;

  /// Arrays containing calibration values for every channel in the 4 banks of the 16 boards.
  Json::Value _calibration[_number_banks][_number_channels];
  /// This is an array storing the goodness of each channel.
  bool _good_chan[_number_banks][_number_channels];

  ChanMapLookup _chan_map; /// Mapping data storage
};  // Don't forget this trailing colon!!!!

} // ~namespace MAUS

#endif
