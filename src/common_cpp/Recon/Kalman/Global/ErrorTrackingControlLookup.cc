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
 * Copyright Chris Rogers, 2016-2017
 */

#include <iostream>

#include "gsl/gsl_errno.h"

#include "src/common_cpp/Utils/Squeak.hh"
#include "src/common_cpp/Utils/Exception.hh"
#include "src/common_cpp/Globals/GlobalsManager.hh"
#include "src/common_cpp/Simulation/GeometryNavigator.hh"

#include "src/common_cpp/Recon/Global/MaterialModelAxialLookup.hh"
#include "src/common_cpp/Recon/Kalman/Global/ErrorTrackingControlLookup.hh"

namespace MAUS {
namespace Kalman {
namespace Global {
namespace ErrorTrackingControlLookup {

typedef struct {
    double _min_step_size;
    double _max_step_size;
} et_control_lookup_state_t;

static void* et_control_lookup_alloc() {
    et_control_lookup_state_t* state =
        static_cast<et_control_lookup_state_t *>(malloc(sizeof(et_control_lookup_state_t)));
    if (state == NULL) {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "Failed to allocate memory for et_control",
                              "ErrorTrackingControlType::et_control_lookup_alloc");
    }
    return state;
}

static int et_control_lookup_init(void* vstate, double min_step_size,
                           double max_step_size, double dummy2, double dummy3) {
    if (min_step_size < 0) {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "global track fit must have positive step size",
                              "ErrorTrackingControlType::et_control_lookup_init");
    }
    if (min_step_size > max_step_size) {
        throw Exceptions::Exception(Exceptions::recoverable,
                              "global track fit must have min step size more than max step size",
                              "ErrorTrackingControlType::et_control_lookup_init");
    }
    et_control_lookup_state_t* state =
                               static_cast<et_control_lookup_state_t *>(vstate);
    state->_min_step_size = min_step_size;
    state->_max_step_size = max_step_size;
    return GSL_SUCCESS;
}

int et_control_lookup_hadjust(void * vstate, size_t dim, unsigned int ord,
                              const double y[], const double yerr[],
                              const double yp[], double * h) {
    et_control_lookup_state_t* state =
                                static_cast<et_control_lookup_state_t*>(vstate);
    if (!MaterialModelAxialLookup::IsReady()) {
        std::cerr << "Control geometry lookup was not ready in " <<
                      "Recon/Kalman/Global/ErrorTrackingControlAxialLookup.hh" << std::endl;
        return GSL_FAILURE;
    }

    double lower_bound = 0;
    double upper_bound = 0;

    double sign = *h/fabs(*h);
    if (sign != sign)
        sign = 1.;

    MaterialModelAxialLookup::GetBounds(y[3], lower_bound, upper_bound);
    double step_size = upper_bound - y[3];
    if (sign < 0) {
        step_size = y[3] - lower_bound;
    }
    if (step_size < state->_min_step_size) {
        step_size = state->_min_step_size;
    } else if (step_size > state->_max_step_size) {
        step_size = state->_max_step_size;
    }

    int return_value = GSL_ODEIV_HADJ_NIL;
    if (fabs(step_size) > fabs(*h)) {
        return_value = GSL_ODEIV_HADJ_INC;
    } else if (fabs(step_size) < fabs(*h)) {
        return_value = GSL_ODEIV_HADJ_DEC;
    }

    *h = sign*fabs(step_size);
    return return_value;
}

static void et_control_lookup_free(void * vstate) {
    et_control_lookup_state_t *state = reinterpret_cast<et_control_lookup_state_t *>(vstate);
    free(state);
}

static const gsl_odeiv_control_type et_control_lookup_type = {
    "maus_recon_kalman_global_error_tracking", /* name */
    &et_control_lookup_alloc,
    &et_control_lookup_init,
    &et_control_lookup_hadjust,
    &et_control_lookup_free
};
const gsl_odeiv_control_type *gsl_odeiv_control_lookup_et = &et_control_lookup_type;

gsl_odeiv_control* gsl_odeiv_control_lookup_et_new(double min_step_size, double max_step_size) {
    gsl_odeiv_control* control =  gsl_odeiv_control_alloc(gsl_odeiv_control_lookup_et);
    int status = et_control_lookup_init(control->state, min_step_size, max_step_size, 0., 0.);
    if (status != GSL_SUCCESS) {
        gsl_odeiv_control_free(control);
        throw Exceptions::Exception(Exceptions::recoverable,
                             "Failed to initialise error tracking control",
                             "ErrorTrackingControlType::gsl_odeiv_control_lookup_et_new");
    }
    return control;
}

}  // namespace ErrorTrackingControlLookup
}  // namespace Global
}  // namespace Kalman
}  // namespace MAUS

