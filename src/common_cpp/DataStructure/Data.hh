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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_MAUS_DATA_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_MAUS_DATA_HH_

#include "src/common_cpp/DataStructure/Spill.hh"

namespace MAUS {

/** Data is the root of the MAUS data structure
 *
 *  Data class is the root class for the MAUS data structure. At the moment it
 *  just holds a pointer to the spill. Note that the bottom of any ROOT data
 *  structure has to be outside the json data structure as we usually allocate 
 *  new memory whenever we load a new spill - which ROOT does not like.
 */
class Data {
  public:
    /** Default constructor initialises everything to NULL */
    Data();

    /** Copy constructor (deepcopy) */
    Data(const Data& data);

    /** Deepcopy from data to *this */
    Data& operator=(const Data& data);

    /** Deletes the spill data */
    virtual ~Data();

    /** Set the spill data
     *
     *  Data now takes ownership of memory allocated to spill
     */
    void SetSpill(Spill* spill);

    /** Get the spill data
     *
     *  Data still owns this memory
     */
    Spill* GetSpill() const;

    /** Returns sizeof the spill object
     *
     *  When setting up TTrees directly in ROOT, it is necessary to hand ROOT
     *  the size of the class member. This is difficult (impossible?) to access
     *  from python, so we provide a convenience function here.
     */
    int my_sizeof();

  private:
    Spill* _spill;
    ClassDef(Data, 1)
};
}

#endif
