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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_KLCHANNELID_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_KLCHANNELID_HH_

#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

/** Identifier for the channel where a hit was recorded in the KL
 */
class KLChannelId {
  public:
    /** Constructor - allocate to 0 */
    KLChannelId();

    /** Copy Constructor - copies data from channel*/
    KLChannelId(const KLChannelId& channel);

    /** Equality operator - copies data from channel */
    KLChannelId& operator=(const KLChannelId& channel);

    /** Destructor (does nothing)*/
    virtual ~KLChannelId();

    /** Get the cell that was hit */
    int GetCell() const;

    /** Set the cell that was hit */
    void SetCell(int cell);


  private:
    int _cell;

    MAUS_VERSIONED_CLASS_DEF(KLChannelId)
};
}

#endif

