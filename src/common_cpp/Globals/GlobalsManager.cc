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

#include "src/legacy/Interface/Squeal.hh"
#include "src/legacy/Interface/Squeak.hh"
#include "src/legacy/Interface/MICERun.hh"
#include "src/legacy/Config/MiceModule.hh"

#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/RunActionManager.hh"
#include "src/common_cpp/Utils/Globals.hh"

#include "src/common_cpp/Simulation/MAUSGeant4Manager.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"

namespace MAUS {

// initialisation order is important here!
// first we need datacards
// then legacy mice_run (what this replaces, but some stuff still calls that)
// then logging (uses datacards on legacy_mice_run to set output levels)
// then error handler (uses logging)
// then legacy cards and run data
void GlobalsManager::InitialiseGlobals(std::string json_datacards) {
    if (Globals::_process != NULL) {
        throw(Squeal(Squeal::recoverable,
      "Attempt to initialise Globals when it was already initialised",
                      "GlobalsManager::InitialiseGlobals"));
    }
    Globals* process = new Globals();
    // Putting this here makes partially initialised Globals available for
    // initialisation. Beware!
    Globals::_process = process;
    try {
        std::cerr << "Globals manager" << std::endl;
        Json::Value config = JsonWrapper::StringToJson(json_datacards);
        process->_configuration_cards = new Json::Value(config);
        process->_legacy_mice_run = MICERun::getInstance();
        // used by logging
        process->_legacy_mice_run->jsonConfiguration =
                                                 process->_configuration_cards;
        int verbose_level = JsonWrapper::GetProperty
                       (config, "verbose_level", JsonWrapper::intValue).asInt();
        // we set up logging but for now leave singleton-like access
        // meaning that we can't reinitialise the logging
        Logging::setStandardOutputs(verbose_level);
        Logging::setOutputs(verbose_level);
        // we set up CppErrorHandler but for now leave singleton-like access
        // meaning that we can't reinitialise the error handler
        process->_error_handler = CppErrorHandler::getInstance();
        process->_legacy_cards = new dataCards(1);
        process->_legacy_mice_run->DataCards = process->_legacy_cards;
        process->_run_action_manager = new RunActionManager();
        std::string rec_file = JsonWrapper::GetProperty
           (*process->_configuration_cards, "reconstruction_geometry_filename",
            JsonWrapper::stringValue).asString();
        std::string sim_file = JsonWrapper::GetProperty
           (*process->_configuration_cards, "simulation_geometry_filename",
            JsonWrapper::stringValue).asString();
        process->_mc_mods = new MiceModule(sim_file);
        if (sim_file == rec_file) {
            process->_recon_mods = process->_mc_mods;
        } else {
            process->_recon_mods = new MiceModule(rec_file);
        }
        process->_legacy_mice_run->miceModule = process->_mc_mods;
        process->_maus_geant4_manager = MAUSGeant4Manager::GetInstance();
        process->_mc_field_constructor = process->_maus_geant4_manager->GetField();
        process->_maus_geant4_manager->SetPhases();
        process->_mc_field_constructor->Print(Squeak::mout(Squeak::info));
        if (process->_recon_mods == process->_mc_mods) {
            process->_recon_field_constructor = process->_mc_field_constructor;
        } else {
            process->_recon_field_constructor =
                                 new BTFieldConstructor(process->_recon_mods);
        }
    } catch(Squeal squee) {
        Globals::_process = NULL;
        delete process;
        throw squee;
    }
}

void GlobalsManager::DeleteGlobals() {
    // we don't delete the MICERun as this isn't really meant to be deleted
    // (it's legacy anyway)
    if (Globals::_process == NULL) {
        throw(Squeal(Squeal::recoverable,
             "Attempt to delete Globals when it was not initialised",
                      "GlobalsManager::DeleteGlobals"));
    }
    if (Globals::_process->_maus_geant4_manager != NULL) {
        // delete Globals::_process->_maus_geant4_manager;
    }
    if (Globals::_process->_mc_field_constructor != NULL) {
        // delete Globals::_process->_field_constructor;
    }
    if (Globals::GetInstance()->_recon_mods != NULL) {
        delete Globals::_process->_recon_mods;
        if (Globals::_process->_recon_mods == Globals::_process->_mc_mods) {
            Globals::_process->_mc_mods = NULL;
        }
        Globals::_process->_recon_mods = NULL;
    }
    if (Globals::GetInstance()->_mc_mods != NULL) {
        delete Globals::_process->_mc_mods;
        Globals::_process->_mc_mods = NULL;
    }
    if (Globals::_process->_run_action_manager != NULL) {
        delete Globals::_process->_run_action_manager;
    }
    if (Globals::_process->_legacy_cards != NULL) {
        delete Globals::_process->_legacy_cards;
    }
    if (Globals::_process->_error_handler != NULL) {
        // won't delete
        // delete Globals::_process->_error_handler;
    }
    if (Globals::_process->_legacy_mice_run != NULL) {
        // won't delete (messes up loads of stuff)
        // delete Globals::_process->_legacy_mice_run;
    }
    if (Globals::_process->_configuration_cards != NULL) {
        // won't delete (messes up Squeak)
        // delete Globals::_process->_configuration_cards;
    }
    delete Globals::_process;
    Globals::_process = NULL;
};

void GlobalsManager::SetReconstructionMiceModules
                                                      (MiceModule* recon_mods) {
    if (Globals::GetInstance()->_recon_mods != NULL &&
      Globals::GetInstance()->_mc_mods != Globals::GetInstance()->_recon_mods) {
        delete Globals::_process->_recon_mods;
    }
    Globals:: _process->_recon_mods = recon_mods;
}

void GlobalsManager::SetMonteCarloMiceModules(MiceModule* mc_mods) {
    if (Globals::GetInstance()->_mc_mods != NULL &&
      Globals::GetInstance()->_mc_mods != Globals::GetInstance()->_recon_mods) {
        delete Globals::_process->_mc_mods;
    }
    Globals::_process->_mc_mods = mc_mods;
}

void GlobalsManager::SetLegacyCards(dataCards* legacy_cards) {
    if (Globals::GetInstance()->_legacy_cards != NULL) {
        delete Globals::_process->_legacy_cards;
    }
    Globals::_process->_legacy_cards = legacy_cards;
}

void GlobalsManager::SetRunActionManager(RunActionManager* run_action) {
    if (Globals::GetInstance()->_run_action_manager != NULL) {
        delete Globals::_process->_run_action_manager;
    }
    Globals::_process->_run_action_manager = run_action;
}
/*
void GlobalsManager::SetGeant4Manager
                                           (MAUSGeant4Manager* geant4_manager) {
    if (Globals::GetInstance()->_maus_geant4_manager != NULL) {
        delete Globals::_process->_maus_geant4_manager;
    }
    Globals::_process->_maus_geant4_manager = geant4_manager;
}

void GlobalsManager::SetBTFieldConstructor(BTFieldConstructor* field) {
    if (Globals::GetInstance()->_field_constructor != NULL) {
        delete Globals::_process->_field_constructor;
    }
    Globals::_process->_field_constructor = field;
}

void GlobalsManager::SetErrorHandler(CppErrorHandler* error_handler) {
    if (Globals::GetInstance()->_error_handler != NULL) {
        delete Globals::_process->_error_handler;
    }
    Globals::_process->_error_handler = error_handler;
}

void GlobalsManager::SetConfigurationCards(Json::Value* configuration_cards) {
    if (Globals::GetInstance()->_configuration_cards != NULL) {
        delete Globals::_process->_configuration_cards;
    }
    Globals::_process->_configuration_cards = configuration_cards;
}
*/
}
