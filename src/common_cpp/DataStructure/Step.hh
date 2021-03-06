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

#ifndef _SRC_COMMON_CPP_DATASTRUCTURE_STEP_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_STEP_HH_

#include <string>
#include <vector>

#include "src/common_cpp/Utils/VersionNumber.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

/** @class Step holds data pertaining to a single step point in GEANT4
 *
 *  Each time a step is made, we can record the position and momentum of the
 *  step point, total energy, time, energy deposited, path length, proper
 *  time of the step.
 */
class Step {
  public:
    /** Initialise to 0
     */
    Step();

    /** Copy constructor */
    Step(const Step& step);

    /** Equality operator */
    Step& operator=(const Step& step);

    /** Destructor - does nothing */
    virtual ~Step();

    /** Returns the position of the step */
    ThreeVector GetPosition() const;
    /** Sets the position of the step */
    void SetPosition(ThreeVector pos);

    /** Returns the momentum of the step */
    ThreeVector GetMomentum() const;

    /** Sets the momentum of the step */
    void SetMomentum(ThreeVector mom);

    /** Returns the spin of the step */
    ThreeVector GetSpin() const;

    /** Sets the spin of the step */
    void SetSpin(ThreeVector spin);

    /** Returns the proper time of the step */
    double GetProperTime() const;

    /** Sets the proper time of the step */
    void SetProperTime(double tau);

    /** Returns the path length so far of the track */
    double GetPathLength() const;

    /** Sets the path length so far of the track */
    void SetPathLength(double length);

    /** Returns the time at which the step was made */
    double GetTime() const;

    /** Sets the time at which the step was made */
    void SetTime(double time);

    /** Returns the energy of the track making the step */
    double GetEnergy() const;

    /** Sets the energy of the track making the step */
    void SetEnergy(double energy);

    /** Returns the energy deposited during the step */
    double GetEnergyDeposited() const;

    /** Sets the energy deposited during the step */
    void SetEnergyDeposited(double edep);

    /** Returns the magnetic field at the step point */
    ThreeVector GetBField() const;

    /** Sets the magnetic field at the step point */
    void SetBField(ThreeVector field);

    /** Returns the electric field at the step point */
    ThreeVector GetEField() const;

    /** Sets the electric field at the step point */
    void SetEField(ThreeVector field);

    /** Returns the G4Material name at the step point */
    std::string GetMaterial() const;

    /** Sets the G4Material name at the step point */
    void SetMaterial(std::string material);

    /** Returns the G4Volume name at the step point */
    std::string GetVolume() const;

    /** Sets the G4Volume name at the step point */
    void SetVolume(std::string volume);

  private:
    ThreeVector _position;
    ThreeVector _momentum;
    ThreeVector _spin;
    ThreeVector _bfield;
    ThreeVector _efield;

    double _proper_time;
    double _path_length;
    double _time;
    double _energy;
    double _energy_deposited;

    std::string _volume;
    std::string _material;

    MAUS_VERSIONED_CLASS_DEF(Step)
};
}

#endif


