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

#include "src/common_cpp/Converter/DataConverters/CppJsonRunHeaderConverter.hh"

#include "src/common_cpp/DataStructure/RunHeaderData.hh"
#include "src/common_cpp/JsonCppProcessors/RunHeaderProcessor.hh"

namespace MAUS {

Json::Value* CppJsonRunHeaderConverter::_convert(const RunHeaderData* data) const {
  if (data == NULL || data->GetRunHeader() == NULL)
      return new Json::Value();
  Json::Value* my_json = RunHeaderProcessor().CppToJson(*data->GetRunHeader(), "");
  return my_json;
}
}
