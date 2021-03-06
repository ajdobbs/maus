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

#include "src/common_cpp/DataStructure/Global/BasePoint.hh"

namespace MAUS {
namespace DataStructure {
namespace Global {

// Default constructor
BasePoint::BasePoint()
    : _position(0., 0., 0., 0.),
      _position_error(0., 0., 0., 0.),
      _detector(MAUS::DataStructure::Global::kUndefined),
      _geometry_path(""),
      _num_photoelectrons(0.),
      _ADC_charge(0),
      _ADC_charge_product(0) {}

// Copy constructor
BasePoint::BasePoint(const BasePoint &base_point)
    : _position(base_point.get_position()),
      _position_error(base_point.get_position_error()),
      _detector(base_point.get_detector()),
      _geometry_path(base_point.get_geometry_path()),
      _num_photoelectrons(base_point.get_num_photoelectrons()),
      _ADC_charge(base_point.get_ADC_charge()),
      _ADC_charge_product(base_point.get_ADC_charge_product()) {}

// Destructor
BasePoint::~BasePoint() {}

// Assignment operator
BasePoint& BasePoint::operator=(const BasePoint &base_point) {
  if (this == &base_point) {
    return *this;
  }
  _position        = base_point.get_position();
  _position_error  = base_point.get_position_error();
  _detector        = base_point.get_detector();
  _geometry_path   = base_point.get_geometry_path();
  _num_photoelectrons = base_point.get_num_photoelectrons();
  _ADC_charge = base_point.get_ADC_charge();
  _ADC_charge_product = base_point.get_ADC_charge_product();

  return *this;
}

// Copy the member variables of 'this' into a new BasePoint
void BasePoint::Clone(BasePoint* bpNew) const {
  bpNew->set_position(this->get_position());
  bpNew->set_position_error(this->get_position_error());
  bpNew->set_detector(this->get_detector());
  bpNew->set_geometry_path(this->get_geometry_path());
  bpNew->set_num_photoelectrons(this->get_num_photoelectrons());
  bpNew->set_ADC_charge(this->get_ADC_charge());
  bpNew->set_ADC_charge_product(this->get_ADC_charge_product());
}

void BasePoint::set_position(TLorentzVector position) {
  _position = position;
}

TLorentzVector BasePoint::get_position() const {
  return _position;
}

void BasePoint::set_position_error(TLorentzVector position_error) {
  _position_error = position_error;
}

TLorentzVector BasePoint::get_position_error() const {
  return _position_error;
}

void BasePoint::set_detector(DetectorPoint detector) {
  _detector = detector;
}

DetectorPoint BasePoint::get_detector() const {
  return _detector;
}

void BasePoint::set_geometry_path(std::string geometry_path) {
  _geometry_path = geometry_path;
}

std::string BasePoint::get_geometry_path() const {
  return _geometry_path;
}

void BasePoint::set_num_photoelectrons(double num_photoelectrons) {
  _num_photoelectrons = num_photoelectrons;
}

double BasePoint::get_num_photoelectrons() const {
  return _num_photoelectrons;
}

void BasePoint::set_ADC_charge(int ADC_charge) {
  _ADC_charge = ADC_charge;
}

int BasePoint::get_ADC_charge() const {
  return _ADC_charge;
}

void BasePoint::set_ADC_charge_product(int ADC_charge_product) {
  _ADC_charge_product = ADC_charge_product;
}

int BasePoint::get_ADC_charge_product() const {
  return _ADC_charge_product;
}

void BasePoint::set_base(BasePoint value) {
  _position = value._position;
  _position_error = value._position_error;
  _detector = value._detector;
  _geometry_path = value._geometry_path;
  _num_photoelectrons = value._num_photoelectrons;
  _ADC_charge = value._ADC_charge;
  _ADC_charge_product = value._ADC_charge_product;
}

} // ~namespace Global
} // ~namespace DataStructure
} // ~namespace MAUS
