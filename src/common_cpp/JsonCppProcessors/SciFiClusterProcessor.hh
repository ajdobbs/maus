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

#include "src/common_cpp/JsonCppProcessors/PrimitivesProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/ObjectProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ThreeVectorProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/ArrayProcessors.hh"
#include "src/common_cpp/JsonCppProcessors/TRefArrayProcessor.hh"
#include "src/common_cpp/JsonCppProcessors/SciFiDigitProcessor.hh"

#include "src/common_cpp/DataStructure/SciFiCluster.hh"

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFICLUSTERPROCESSOR_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_SCIFICLUSTERPROCESSOR_HH_

namespace MAUS {

/** @class SciFiClusterProcessor processor for SciFiCluster */
class SciFiClusterProcessor : public ObjectProcessor<SciFiCluster> {
 public:
    /** Constructor - registers the branch structure */
    SciFiClusterProcessor();

 private:
    IntProcessor _int_proc;
    BoolProcessor _bool_proc;
    DoubleProcessor _double_proc;
    ThreeVectorProcessor _three_vec_proc;
    TRefArrayProcessor _digit_tref_proc;
};
}

#endif
