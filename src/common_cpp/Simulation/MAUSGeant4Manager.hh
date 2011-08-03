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

#ifndef _SRC_COMMON_CPP_SIMULATION_MAUSGEANT4MANAGER_HH_
#define _SRC_COMMON_CPP_SIMULATION_MAUSGEANT4MANAGER_HH_

#include <G4RunManager.hh>
#include <G4SDManager.hh>

// should all be forward declarations? yes - but be careful about namespace
#include "src/legacy/Simulation/MICERunAction.hh"
#include "src/legacy/Simulation/FillMaterials.hh"
#include "src/legacy/Simulation/MICEDetectorConstruction.hh"
#include "src/common_cpp/Simulation/MAUSPrimaryGeneratorAction.hh"
#include "src/common_cpp/Simulation/MAUSSteppingAction.hh"
#include "src/common_cpp/Simulation/MAUSStackingActionKillNonMuons.hh"
#include "src/common_cpp/Simulation/MAUSTrackingAction.hh"
#include "src/common_cpp/Simulation/MAUSEventAction.hh"
#include "src/common_cpp/Simulation/VirtualPlanes.hh"

class MICEPhysicsList;

namespace MAUS {

/** MAUSPhysicsList is a synonym for (legacy) MICEPhysicsList
 */
typedef MICEPhysicsList MAUSPhysicsList;

/** @class MAUSGeant4Manager
 *
 *  @brief Manage Geant4 setup - singleton class that controls the interface with
 *  geant4.
 *
 *  This has to be a singleton class so that we can't accidentally set
 *  up geant4 twice.
 *
 *  Geant4 setup:
 *  We have a few different classes 
 */
class MAUSGeant4Manager {
  public:
    /** @brief Get the singleton MAUSGeant4Manager
     *
     *  Get the instance of the MAUSGeant4Manager. This will construct the
     *  MAUSGeant4Manager if required.
     */
    static MAUSGeant4Manager* GetInstance();

    /** @brief Get the G4RunManager
     */
    G4RunManager* GetRunManager() const {return _runManager;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSSteppingAction* GetStepping() const {return _stepAct;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSTrackingAction* GetTracking() const {return _trackAct;}

    /** @brief Get the MAUSEventAction
     */
    MAUSEventAction* GetEventAction() const {return _eventAct;}

    /** @brief Get the MAUSSteppingAction
     */
    MAUSPrimaryGeneratorAction* GetPrimaryGenerator() const {return _primary;}

    /** @brief Get the MAUSPhysicsList
     */
    MAUSPhysicsList* GetPhysicsList() const {return _physList;}

    /** @brief Get the Geometry
     */
    MICEDetectorConstruction* GetGeometry() const {return _detector;}

    /** @brief Get the VirtualPlanes
     */
    VirtualPlaneManager* GetVirtualPlanes() const {return _virtPlanes;}

    /** @brief Set the VirtualPlanes
     *
     *  Nb: this loses the pointer to the original virtual planes - so if caller
     *  don't want to keep them, caller must delete the original virtual planes.
     */
    void SetVirtualPlanes(VirtualPlaneManager* virt) {_virtPlanes = virt;}

    /** @brief Phased fields in the geometry (e.g. RF cavities)
     *
     *  If there are unphased fields in the geometry, SetPhases will attempt to
     *  phase them using FieldPhaser. Throws an exception if the phasing fails.
     */
    void SetPhases();

    /** @brief Get the reference particle from json configuration
     */
    MAUSPrimaryGeneratorAction::PGParticle GetReferenceParticle();

    /** @brief Run a particle through the simulation
     *
     *  @returns a json object with tracking, virtual hits and real hits
     */
    Json::Value RunParticle(MAUSPrimaryGeneratorAction::PGParticle p);

    /** @brief Run a particle through the simulation
     *
     *  @returns copy of particle with any tracking output appended
     *           to the particle. Following branches will be overwritten with
     *           tracking output from this event:\n
     *             "tracks", "virtual_hits", "hits"
     */
    Json::Value RunParticle(Json::Value particle);

    /** @brief Run an array of particles through the simulation
     *
     *  @param particle_array should conform to spill schema for spill->mc
     *         branch (so array of Json values containing objects, each of which
     *         has a primary branch containing the primary particle data)
     *
     *  @returns an array of particles with any new hits, virtual_hits or tracks
     *  appended
     */
    Json::Value RunManyParticles(Json::Value particle_array);

  private:
    MAUSGeant4Manager();
    ~MAUSGeant4Manager();

    G4RunManager* _runManager;
    MICEPhysicsList* _physList;
    MAUSPrimaryGeneratorAction* _primary;
    MAUSSteppingAction*         _stepAct;
    MAUSTrackingAction*         _trackAct;
    MAUSEventAction*            _eventAct;
    MICEDetectorConstruction*   _detector;
    VirtualPlaneManager*        _virtPlanes;

    Json::Value Tracking(MAUSPrimaryGeneratorAction::PGParticle p);
};

}  // namespace MAUS

#endif  // _SRC_CPP_CORE_SIMULATION_MAUSGEANT4MANAGER_HH_

