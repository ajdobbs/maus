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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENT_
#define _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENT_

#include "Rtypes.h"  // ROOT

namespace MAUS {

/** @class KLEvent comment
 *
 */

class KLEvent {
  public:
    /** Default constructor - initialises to 0/NULL */
    KLEvent();

    /** Copy constructor - any pointers are deep copied */
    KLEvent(const KLEvent& _klevent);

    /** Equality operator - any pointers are deep copied */
    KLEvent& operator=(const KLEvent& _klevent);

    /** Destructor - any member pointers are deleted */
    virtual ~KLEvent();


  private:

    ClassDef(KLEvent, 1)
};
}

#endif  // _SRC_COMMON_CPP_DATASTRUCTURE_KLEVENT_
