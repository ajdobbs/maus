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

/** @class SciFiSpacePoint
 *
 *
 */

#ifndef SCIFISPACEPOINT_HH
#define SCIFISPACEPOINT_HH

// C headers
#include <assert.h>
#include <json/json.h>
#include <CLHEP/Vector/ThreeVector.h>

#include <CLHEP/Random/RandPoisson.h>
#include <CLHEP/Random/RandGauss.h>
#include <CLHEP/Random/RandExponential.h>
#include <CLHEP/Units/PhysicalConstants.h>

// C++ headers
#include <string>
#include <vector>

#include "src/common_cpp/Recon/SciFi/SciFiCluster.hh"

// namespace MAUS {

class SciFiSpacePoint {
 public:

  /** Default constructor - initialises to 0/NULL */
  SciFiSpacePoint();

  /** Copy constructor - any pointers are deep copied */
  SciFiSpacePoint(const SciFiSpacePoint &_scifispacepoint);

  /** Two cluster constructor  */
  SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2, SciFiCluster *clust3);

  /** Three cluster constructor  */
  SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2);

  /** Destructor  */
  virtual ~SciFiSpacePoint();

  /** Assignment operator - any pointers are deep copied */
  SciFiSpacePoint& operator=(const SciFiSpacePoint &_scifispacepoint);

  // Getters and setters
  void set_spill(int spill) { _spill = spill; }

  int get_spill()   const { return _spill; }

  void set_event(int event) { _event = event; }

  int get_event() const { return _event; }

  void set_tracker(int tracker) { _tracker = tracker; }

  int get_tracker() const { return _tracker; }

  void set_station(int station) { _station = station; }

  int get_station() const { return _station; }

  void set_npe(double nPE) { _npe = nPE; }

  double get_npe()     const { return _npe; }

  // void set_time(double time) { _time = time; }

  // double get_time()    const { return _time; }

  void set_type(std::string type) { _type = type; }

  std::string get_type()  const { return _type; }

  void set_position(CLHEP::Hep3Vector position) { _position = position; }

  CLHEP::Hep3Vector get_position() const { return _position; }

  void set_chi2(double chi2) { _chi2 = chi2; }

  double get_chi2()    const { return _chi2; }

  void set_used() { _used = true; }

  void set_used(bool used) { _used = used; }

  bool is_used() const { return _used; }

  bool get_used() const { return _used; }

  void set_channels(std::vector<SciFiCluster*> channels) { _channels = channels; }

  std::vector<SciFiCluster*> get_channels()  const { return _channels; }

 private:
  bool _used;

  int _spill, _event, _tracker, _station;

  double _npe, _chi2;

  std::string _type;

  std::vector<SciFiCluster*>  _channels;

  CLHEP::Hep3Vector _position;

  // int _time, _time_error, _time_res;
};  // Don't forget this trailing colon!!!!
// } // ~namespace MAUS

#endif
