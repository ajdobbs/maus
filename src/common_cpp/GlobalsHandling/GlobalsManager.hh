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

#ifndef _SRC_COMMON_CPP_GLOBALSHANDLING_GLOBALSMANAGER_HH_
#define _SRC_COMMON_CPP_GLOBALSHANDLING_GLOBALSMANAGER_HH_

#include <string>

#include "json/json.h"

// legacy classes outside the MAUS namespace
class Squeak;
class dataCards;
class MICERun;
class MiceMaterials;
class MiceModule;

namespace MAUS {

typedef Squeak Logging;
class RunActionManager;
class CppErrorHandler;

class MAUSGeant4Manager;
class BTFieldConstructor;

// I decided not to make this a user interfaceable class. I can change that if
// we find we need lots of things set up at the beginning of the job...

/** @class GlobalsManager - Handler for global objects.
 *
 *  The GlobalsManager holds data associated with a particular execution of the
 *  MAUS code. The GlobalsManager is a singleton class. Note that in distributed
 *  computing mode there will be one MAUSProcess per processing node.
 *
 *  This is quite a low level object, so it just hands pointers around. For 
 *  example don't want to require GEANT4 or anything to interact with this class
 *  and don't want to risk circular dependencies. For this reason instantiation
 *  is handled by GlobalsManagerFactory class, which is high level and does need
 *  to know about all that stuff. Also has destructor there. I would rather keep
 *  construction and destruction in the class, but I think it makes circular
 *  references which can be unpleasant.
 *
 *  Note also that one feature is that (except GEANT4, which won't fix) it would
 *  be nice if we take out all of the global/static variables and initialise
 *  things here instead. So that we can in principle do a delete and
 *  reinitialise and get a completely clean set up.
 */
class GlobalsManager {
  public:
    /** Return a pointer to the singleton instance
     *
     *  This returns a pointer to the singleton instance or throws an exception
     *  if it is not initialised yet. Initialisation should have been done by
     *  GlobalsManagerFactory.
     */
    static GlobalsManager* GetInstance();

    /** Return true if the singleton has been initialised, else false
     */
    static bool HasInstance();

    /** Get RunActionManager. Data updated run-by-run.
     */
    static RunActionManager* GetRunActionManager();

    /** Set RunActionManager. Data updated run-by-run.
     */
    static void SetRunActionManager(RunActionManager* manager);

    /** Get CppErrorHandler. Controls what happens to exceptions.
     */
    static CppErrorHandler* GetErrorHandler();

    /** Set CppErrorHandler. Controls what happens to exceptions.
     */
    static void SetErrorHandler(CppErrorHandler* handler);

    /** Get Configuration datacards. Global run controls.
     */ 
    static Json::Value* GetConfigurationCards();

    /** Set Configuration datacards. Global run controls.
     */ 
    static void SetConfigurationCards(Json::Value* cards);

    /** Get legacy datacards. Old global run controls (please dont use)
     */
    static dataCards* GetLegacyCards();

    /** Set legacy datacards. Old global run controls (please dont use)
     */
    static void SetLegacyCards(dataCards* cards);

    /** Get the BTFieldConstructor (controls access to field maps)
     */
    static BTFieldConstructor* GetBTFieldConstructor();

    /** Set the BTFieldConstructor (controls access to field maps)
     */
    static void SetBTFieldConstructor(BTFieldConstructor* field);

    /** Get the Geant4Manager (controls access to G4 objects)
     */
    static MAUSGeant4Manager* GetGeant4Manager();

    /** Set the Geant4Manager (controls access to G4 objects)
     */
    static void SetGeant4Manager(MAUSGeant4Manager* manager);

    /** Get the MC Geometry MiceModules
     *
     *  MiceModules is a tree of geometry information - this returns the root of
     *  the tree
     *
     *  Concept is to have reconstruction geometry and separate MC geometry so
     *  that we can do MC vs Recon studies
     */
    static MiceModule* GetMonteCarloMiceModules();

    /** Set the MC Geometry MiceModules
     */
    static void SetMonteCarloMiceModules(MiceModule* root_module);

    /** Get the Reconstruction Geometry MiceModules
     *
     *  MiceModules is a tree of geometry information - this returns the root of
     *  the tree.
     *
     *  Concept is to have reconstruction geometry and separate MC geometry so
     *  that we can do MC vs Recon studies
     */
    static MiceModule* GetReconstructionMiceModules();

    /** Set the Reconstruction Geometry MiceModules
     */
    static void SetReconstructionMiceModules(MiceModule* root_module);

  private:
    // construction, 
    GlobalsManager();
    GlobalsManager(const GlobalsManager& process);
    GlobalsManager& operator=(const GlobalsManager& process);
    ~GlobalsManager();

    Json::Value* _configuration_cards;
    MICERun* _legacy_mice_run;
    CppErrorHandler* _error_handler;
    dataCards* _legacy_cards;
    RunActionManager* _run_action_manager;
    MiceModule* _mc_mods;
    MiceModule* _recon_mods;
    BTFieldConstructor* _field_constructor;
    MAUSGeant4Manager* _maus_geant4_manager;
    MiceMaterials* _mice_materials;
    static GlobalsManager* _process;
    friend class GlobalsManagerFactory;
};
}  // namespace MAUS

#endif  // _SRC_COMMON_CPP_GLOBALSHANDLING_GLOBALSMANAGER_HH_
