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

#include <string>
#include "src/common_cpp/DataStructure/Step.hh"

namespace MAUS {

Step::Step() : _position(0, 0, 0), _momentum(0, 0, 0), _spin(0, 0, 0),
               _bfield(0, 0, 0), _efield(0, 0, 0), _proper_time(0),
               _path_length(0), _time(0), _energy(0), _energy_deposited(0),
               _volume(""), _material("") {
}

Step::Step(const Step& step) {
    *this = step;
}

Step& Step::operator=(const Step& step) {
    if (&step == this) {
        return *this;
    }
    _position = step._position;
    _momentum = step._momentum;
    _spin = step._spin;
    _bfield = step._bfield;
    _efield = step._efield;
    _proper_time = step._proper_time;
    _path_length = step._path_length;
    _time = step._time;
    _energy = step._energy;
    _energy_deposited = step._energy_deposited;
    _volume = step._volume;
    _material = step._material;

    return *this;
}

Step::~Step() {
}

ThreeVector Step::GetPosition() const {
    return _position;
}

void Step::SetPosition(ThreeVector pos) {
    _position = pos;
}

ThreeVector Step::GetMomentum() const {
    return _momentum;
}

void Step::SetMomentum(ThreeVector mom) {
    _momentum = mom;
}
ThreeVector Step::GetSpin() const {
    return _spin;
}

void Step::SetSpin(ThreeVector spin) {
    _spin = spin;
}

double Step::GetProperTime() const {
    return _proper_time;
}

void Step::SetProperTime(double tau) {
    _proper_time = tau;
}

double Step::GetPathLength() const {
    return _path_length;
}

void Step::SetPathLength(double length) {
    _path_length = length;
}

double Step::GetTime() const {
    return _time;
}

void Step::SetTime(double time) {
    _time = time;
}

double Step::GetEnergy() const {
    return _energy;
}

void Step::SetEnergy(double energy) {
    _energy = energy;
}

double Step::GetEnergyDeposited() const {
    return _energy_deposited;
}

void Step::SetEnergyDeposited(double edep) {
    _energy_deposited = edep;
}

ThreeVector Step::GetBField() const {
    return _bfield;
}

void Step::SetBField(ThreeVector bfield) {
    _bfield = bfield;
}

ThreeVector Step::GetEField() const {
    return _efield;
}

void Step::SetEField(ThreeVector efield) {
    _efield = efield;
}

std::string Step::GetMaterial() const {
    return _material;
}

void Step::SetMaterial(std::string material) {
    _material = material;
}

std::string Step::GetVolume() const {
    return _volume;
}

void Step::SetVolume(std::string volume) {
    _volume = volume;
}
}

