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
 */

#include "src/common_cpp/DataStructure/TOFSlabHit.hh"


namespace MAUS {

TOFSlabHit::TOFSlabHit()
    : _slab(0), _phys_event_number(0), _raw_time(0.), _charge(0), _plane(0),
      _charge_product(0), _time(0.), _station(0), _detector(""),
      _part_event_number(0), _pmt1(), _pmt0(),
      _global_x(0), _global_y(0), _global_z(0),
      _global_x_err(0), _global_y_err(0), _global_z_err(0),
      _vertical_slab(false), _horizontal_slab(false) {
}

TOFSlabHit::TOFSlabHit(const TOFSlabHit& _tofslabhit)
    : _slab(0), _phys_event_number(0), _raw_time(0.), _charge(0), _plane(0),
      _charge_product(0), _time(0.), _station(0), _detector(""),
      _part_event_number(0), _pmt1(), _pmt0(),
      _global_x(0), _global_y(0), _global_z(0),
      _global_x_err(0), _global_y_err(0), _global_z_err(0),
      _vertical_slab(false), _horizontal_slab(false) {
    *this = _tofslabhit;
}

TOFSlabHit& TOFSlabHit::operator=(const TOFSlabHit& _tofslabhit) {
    if (this == &_tofslabhit) {
        return *this;
    }
    SetSlab(_tofslabhit._slab);
    SetPhysEventNumber(_tofslabhit._phys_event_number);
    SetRawTime(_tofslabhit._raw_time);
    SetCharge(_tofslabhit._charge);
    SetPlane(_tofslabhit._plane);
    SetChargeProduct(_tofslabhit._charge_product);
    SetTime(_tofslabhit._time);
    SetStation(_tofslabhit._station);
    SetDetector(_tofslabhit._detector);
    SetPartEventNumber(_tofslabhit._part_event_number);
    SetPmt1(_tofslabhit._pmt1);
    SetPmt0(_tofslabhit._pmt0);
    SetGlobalPosX(_tofslabhit._global_x);
    SetGlobalPosY(_tofslabhit._global_y);
    SetGlobalPosZ(_tofslabhit._global_z);
    SetGlobalPosXErr(_tofslabhit._global_x_err);
    SetGlobalPosYErr(_tofslabhit._global_y_err);
    SetGlobalPosZErr(_tofslabhit._global_z_err);
    SetHorizontal(_tofslabhit._horizontal_slab);
    SetVertical(_tofslabhit._vertical_slab);
    return *this;
}

TOFSlabHit::~TOFSlabHit() {
}

int TOFSlabHit::GetSlab() const {
    return _slab;
}

void TOFSlabHit::SetSlab(int slab) {
    _slab = slab;
}

int TOFSlabHit::GetPhysEventNumber() const {
    return _phys_event_number;
}

void TOFSlabHit::SetPhysEventNumber(int phys_event_number) {
    _phys_event_number = phys_event_number;
}

double TOFSlabHit::GetRawTime() const {
    return _raw_time;
}

void TOFSlabHit::SetRawTime(double raw_time) {
    _raw_time = raw_time;
}

int TOFSlabHit::GetCharge() const {
    return _charge;
}

void TOFSlabHit::SetCharge(int charge) {
    _charge = charge;
}

int TOFSlabHit::GetPlane() const {
    return _plane;
}

void TOFSlabHit::SetPlane(int plane) {
    _plane = plane;
}

int TOFSlabHit::GetChargeProduct() const {
    return _charge_product;
}

void TOFSlabHit::SetChargeProduct(int charge_product) {
    _charge_product = charge_product;
}

double TOFSlabHit::GetTime() const {
    return _time;
}

void TOFSlabHit::SetTime(double time) {
    _time = time;
}

int TOFSlabHit::GetStation() const {
    return _station;
}

void TOFSlabHit::SetStation(int station) {
    _station = station;
}

std::string TOFSlabHit::GetDetector() const {
    return _detector;
}

void TOFSlabHit::SetDetector(std::string detector) {
    _detector = detector;
}

int TOFSlabHit::GetPartEventNumber() const {
    return _part_event_number;
}

void TOFSlabHit::SetPartEventNumber(int part_event_number) {
    _part_event_number = part_event_number;
}

Pmt1 TOFSlabHit::GetPmt1() const {
    return _pmt1;
}

Pmt1* TOFSlabHit::GetPmt1Ptr() {
    return &_pmt1;
}

void TOFSlabHit::SetPmt1(Pmt1 pmt1) {
    _pmt1 = pmt1;
}

Pmt0 TOFSlabHit::GetPmt0() const {
    return _pmt0;
}

Pmt0* TOFSlabHit::GetPmt0Ptr() {
    return &_pmt0;
}

void TOFSlabHit::SetPmt0(Pmt0 pmt0) {
    _pmt0 = pmt0;
}

double TOFSlabHit::GetGlobalPosX() const {
    return _global_x;
}

double TOFSlabHit::GetGlobalPosY() const {
    return _global_y;
}

double TOFSlabHit::GetGlobalPosZ() const {
    return _global_z;
}

double TOFSlabHit::GetGlobalPosXErr() const {
    return _global_x_err;
}

double TOFSlabHit::GetGlobalPosYErr() const {
    return _global_y_err;
}

double TOFSlabHit::GetGlobalPosZErr() const {
    return _global_z_err;
}

void TOFSlabHit::SetGlobalPosX(double global_x) {
    _global_x = global_x;
}

void TOFSlabHit::SetGlobalPosY(double global_y) {
    _global_y = global_y;
}

void TOFSlabHit::SetGlobalPosZ(double global_z) {
    _global_z = global_z;
}

void TOFSlabHit::SetGlobalPosXErr(double global_x_err) {
    _global_x_err = global_x_err;
}

void TOFSlabHit::SetGlobalPosYErr(double global_y_err) {
    _global_y_err = global_y_err;
}

void TOFSlabHit::SetGlobalPosZErr(double global_z_err) {
    _global_z_err = global_z_err;
}

void TOFSlabHit::SetHorizontal(bool hSlab) {
    _horizontal_slab = hSlab;
}

void TOFSlabHit::SetVertical(bool vSlab) {
    _vertical_slab = vSlab;
}
}

