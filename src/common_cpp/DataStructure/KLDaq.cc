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

#include "src/common_cpp/DataStructure/KLDaq.hh"


namespace MAUS {

KLDaq::KLDaq()
    : _V1724() {
}

KLDaq::KLDaq(const KLDaq& _kldaq)
    : _V1724() {
    *this = _kldaq;
}

KLDaq& KLDaq::operator=(const KLDaq& _kldaq) {
    if (this == &_kldaq) {
        return *this;
    }
    SetV1724Array(_kldaq._V1724);
    return *this;
}

KLDaq::~KLDaq() {
}

V1724Array KLDaq::GetV1724Array() const {
    return _V1724;
}

V1724 KLDaq::GetV1724ArrayElement(size_t index) const {
    return _V1724[index];
}

size_t KLDaq::GetV1724ArraySize() const {
    return _V1724.size();
}

void KLDaq::SetV1724Array(V1724Array V1724) {
    _V1724 = V1724;
}
}

