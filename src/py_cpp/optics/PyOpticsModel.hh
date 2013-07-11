/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

#ifndef _SRC_PY_CPP_PYOPTICSMODEL_HH_
#define _SRC_PY_CPP_PYOPTICSMODEL_HH_

// These ifdefs are required to avoid cpp compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

namespace MAUS {

class OpticsModel; // note this is just in MAUS namespace

namespace PyOpticsModel {

/** PyOpticsModel is the python implementation of the C++ OpticsModel class
 */
typedef struct {
    PyObject_HEAD;
    OpticsModel* model;
} PyOpticsModel;

/** _alloc allocates memory for PyOpticsModel
 */
PyObject *_alloc(PyTypeObject *self, Py_ssize_t nitems);

/** _new allocates and initialises memory for PyOpticsModel
 */
PyObject *_new(PyTypeObject *self, Py_ssize_t nitems);

/** _init initialises an allocated PyOpticsModel object
 */
int _init(PyObject* self, PyObject *args, PyObject *kwds);

/** deallocate memory */
void _dealloc(PyOpticsModel * self);

/** synonym for dealloc */
void _free(PyOpticsModel * self);

/** Initialise optics_model module
 *
 *  This is called by import optics_model; it initialises the OpticsModel type
 *  allowing user to construct and call methods on OpticsModel objects
 */
PyMODINIT_FUNC initoptics_model(void);

/** Return the C++ optics model associated with a PyOpticsModel 
 *  
 *  OpticsModel still owns the memory allocated to PyOpticsModel
 */
OpticsModel* get_optics_model(PyOpticsModel* py_model);

}  // namespace PyOpticsModel
}  // namespace MAUS

#endif  // _SRC_PY_CPP_PYFIELDS_HH_
