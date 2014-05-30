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

/** @class MapCppGlobalReconImport
 *  Import detector events from Recon Event into a Global Event and create
 *  global tracks.
 *
 */

#ifndef _SRC_MAP_MAPCPPGLOBALRECONIMPORT_H_
#define _SRC_MAP_MAPCPPGLOBALRECONIMPORT_H_

// Python / C API
#include <Python.h>

// C headers
#include <assert.h>
#include <json/json.h>

// C++ headers
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// Other headers
#include "Interface/Squeak.hh"
#include "Config/MiceModule.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/SpillProcessor.hh"

#include "src/common_cpp/DataStructure/GlobalEvent.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "Recon/Global/ImportTOFRecon.hh"
#include "Recon/Global/ImportSciFiRecon.hh"
#include "Recon/Global/TrackMatching.hh"

namespace MAUS {

  class MapCppGlobalReconImport {
  public:
    /** Constructor, setting the internal variable #_classname */
    MapCppGlobalReconImport();

    /** Sets up the worker
     *
     *  @param argJsonConfigDocument a JSON document with
     *         the configuration.
     */
    bool birth(std::string argJsonConfigDocument);

    /** Shutdowns the worker
     *
     *  This takes no arguments and does nothing
     */
    bool death();

    /** process JSON document
     *
     *  Receive a document with recon events and return a 
     *  document with populated global events and tracks.
     * @param document a line/spill from the JSON input
     */
    std::string process(std::string) const;

    /** Import the existing MAUS::ReconEvent, creating a new
     * MAUS::GlobalEvent and populating a
     * MAUS::recon::global::Track object. 
     * @param recon_event The ReconEvent to obtain the GlobalEvent from
     *                  
     * @return global_event The GlobalEvent with spacepoints and track(s)
     */
    MAUS::GlobalEvent* Import(MAUS::ReconEvent* recon_event) const;

  private:
    /// Check that a valid configuration is passed to the process
    bool _configCheck;
    /// This will contain the configuration
    Json::Value _configJSON;
    ///  JsonCpp setup
    Json::Reader _reader;

    // Mapper name, useful for tracking results...
    std::string _classname;
  }; // Don't forget this trailing colon!!!!
} // ~MAUS

#endif