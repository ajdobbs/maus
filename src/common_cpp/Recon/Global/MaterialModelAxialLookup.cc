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

#include <math.h>

#include <utility>
#include <algorithm>
#include <limits>

#include "Geant4/G4Material.hh"

#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Utils/Globals.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"

#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"

namespace MAUS {

double MaterialModelAxialLookup::_z_tolerance = 1e-1;
std::vector<std::pair<double, G4Material*> > MaterialModelAxialLookup::_lookup;

MaterialModelAxialLookup::MaterialModelAxialLookup(double x, double y, double z) {
    SetMaterial(x, y, z);
}

MaterialModelAxialLookup::MaterialModelAxialLookup(const MaterialModelAxialLookup& mat) {
    *this = mat;
}

MaterialModelAxialLookup& MaterialModelAxialLookup::operator=(const MaterialModelAxialLookup& mat) {
    if (&mat == this) {
        return *this;
    }
    _material = mat._material;
    _n_e = mat._n_e;
    _I = mat._I;
    _x_0 = mat._x_0;
    _x_1 = mat._x_1;
    _C = mat._C;
    _a = mat._a;
    _k = mat._k;
    _rad_len_ratio = mat._rad_len_ratio;
    _density = mat._density;
    _z_over_a = mat._z_over_a;
    return *this;
}

MaterialModelAxialLookup* MaterialModelAxialLookup::Clone() {
     throw Exceptions::Exception(Exceptions::recoverable,
                              "Not implemented", "MaterialModelDynamic::Clone");
}

bool compare(std::pair<double, G4Material*> a_pair, double z) {
    return z > a_pair.first;
}

void MaterialModelAxialLookup::SetMaterial(double x, double y, double z) {
    if (_lookup.size() == 0)
      throw Exceptions::Exception(Exceptions::recoverable,
                              "Attempt to set material without building lookup table first",
                              "MaterialModelAxialLookup::Clone");
    typedef std::vector<std::pair<double, G4Material*> >::iterator iter;
    iter a_pair = std::lower_bound(_lookup.begin(), _lookup.end(), z, compare);
    if (a_pair != _lookup.begin())
        a_pair--;
    MaterialModel::_material = a_pair->second;
    if (!IsEnabledMaterial(_material->GetName())) { // URG... string comparison
        MaterialModel::_material = NULL;
    } else {
        MaterialModel::SetMaterial(_material);
    }
}

void MaterialModelAxialLookup::BuildLookupTable(double z_start, double z_end) {
    if (z_end <= z_start)
      throw Exceptions::Exception(Exceptions::recoverable,
                              "z_start must be less than z_end",
                              "MaterialModelAxialLookup::BuildLookupTable");
    _lookup = std::vector<std::pair<double, G4Material*> >();
    GeometryNavigator* navigator = Globals::GetMCGeometryNavigator();
    ThreeVector dir(0., 0., 1.);
    G4Material* old_mat = NULL;
    G4Material* new_mat = NULL;
    double z = z_start;
    while (z < z_end) {
        navigator->SetPoint(ThreeVector(0., 0., z));
        new_mat = navigator->GetMaterial();
        if (new_mat != old_mat) {
            std::pair<double, G4Material*> new_pair(z, new_mat);
            _lookup.push_back(new_pair);
        }
        old_mat = new_mat;
        double delta_z = navigator->ComputeStep(ThreeVector(0., 0., z), dir, 1.);
        if (fabs(delta_z) < _z_tolerance)
            delta_z  = _z_tolerance;
        z += delta_z;
    }
}

void MaterialModelAxialLookup::PrintLookupTable(std::ostream& out) {
    for (size_t i = 0; i < _lookup.size(); ++i) {
        out << _lookup[i].first << " " << _lookup[i].second->GetName() << std::endl;
    }
}

void MaterialModelAxialLookup::GetBounds(double z_pos, double& lower_bound, double& upper_bound) {
    typedef std::vector<std::pair<double, G4Material*> >::iterator iter;
    iter a_pair = std::lower_bound(_lookup.begin(), _lookup.end(), z_pos, compare);
    // std::cerr << "GetBounds at " << z_pos << " finds " << a_pair->first
    //           << " " << a_pair->second << std::endl;
    if (a_pair == _lookup.begin()) {
        lower_bound = std::numeric_limits<double>::lowest();
        upper_bound = a_pair->first;
    } else if (a_pair == _lookup.end()) {
        lower_bound = _lookup.back().first;
        upper_bound = std::numeric_limits<double>::max();
    } else {
        upper_bound = a_pair->first;
        lower_bound = (--a_pair)->first;
    }
}

} // namespace MAUS

