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

#include "src/common_cpp/JsonCppProcessors/SciFiTrackProcessor.hh"

namespace MAUS {

SciFiTrackProcessor::SciFiTrackProcessor()
                         : _scifi_trackpoint_array_proc(new SciFiTrackPointProcessor) {
    RegisterValueBranch("tracker", &_int_proc,
                        &SciFiTrack::tracker,
                        &SciFiTrack::set_tracker, false);
    RegisterValueBranch("algorithm", &_int_proc,
                        &SciFiTrack::GetAlgorithmUsed,
                        &SciFiTrack::SetAlgorithmUsed, false);
    RegisterValueBranch("charge", &_int_proc,
                        &SciFiTrack::charge,
                        &SciFiTrack::set_charge, false);
    RegisterValueBranch("f_chi2", &_double_proc,
                        &SciFiTrack::f_chi2,
                        &SciFiTrack::set_f_chi2, false);
    RegisterValueBranch("s_chi2", &_double_proc,
                        &SciFiTrack::s_chi2,
                        &SciFiTrack::set_s_chi2, false);
    RegisterValueBranch("ndf", &_int_proc,
                        &SciFiTrack::ndf,
                        &SciFiTrack::set_ndf, false);
    RegisterValueBranch("P_value", &_double_proc,
                        &SciFiTrack::P_value,
                        &SciFiTrack::set_P_value, false);
    RegisterValueBranch("trackpoints", &_scifi_trackpoint_array_proc,
                        &SciFiTrack::scifitrackpoints,
                        &SciFiTrack::set_scifitrackpoints, false);
}
} // ~namespace MAUS