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
#ifndef _SRC_COMMON_CPP_API_REDUCEBASE_INL_
#define _SRC_COMMON_CPP_API_REDUCEBASE_INL_

#include <string>
#include "src/common_cpp/API/APIExceptions.hh"
#include "Utils/Exception.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "src/common_cpp/Utils/PyObjectWrapper.hh"
#include "Utils/Squeak.hh"

namespace MAUS {

  template <typename T_IN, typename T_OUT>
  ReduceBase<T_IN, T_OUT>::ReduceBase(const std::string& s)
  : IReduce<T_IN, T_OUT>(), ModuleBase(s), _output(new T_OUT), _own_output(true) {}

  template <typename T_IN, typename T_OUT>
  ReduceBase<T_IN, T_OUT>::ReduceBase(const ReduceBase& rb)
  : IReduce<T_IN, T_OUT>(), ModuleBase(rb._classname) {
    if (rb._output)
      _output = new T_OUT(*(rb._output));
  }

  template <typename T_IN, typename T_OUT>
  ReduceBase<T_IN, T_OUT>::~ReduceBase() {
    if (_output && _own_output)
      delete _output;
  }

  template <typename T_IN, typename T_OUT>
  void ReduceBase<T_IN, T_OUT>::SetOutput(T_OUT* out) {
    if (_output && _own_output)
      delete _output;

    _output = out;
    _own_output = false;
  }

  template <typename T_IN, typename T_OUT>
  void ReduceBase<T_IN, T_OUT>::process(T_IN* t_in) {
    if (!t_in) { throw NullInputException(_classname); }
    this->_process(t_in);
  }

  template <typename T_IN, typename T_OUT>
  PyObject* ReduceBase<T_IN, T_OUT>::process_pyobj(PyObject* py_input) {
    T_OUT* cpp_out;
    try {
        // this function owns cpp_in; py_input is still owned by caller
        // (unwrap performed a deep copy and did any necessary type conversion)
        T_IN* cpp_in = PyObjectWrapper::unwrap<T_IN>(py_input);
        _process(cpp_in);
	cpp_out = new T_OUT(*_output);
        delete cpp_in;
    } catch (std::exception& e) {
      PyErr_SetString(PyExc_RuntimeError, e.what());
      return NULL; // let the python error handler do its thing
    } catch (...) {
      throw UnhandledException(_classname);
    }

    PyObject* py_output = PyObjectWrapper::wrap(cpp_out);
    // py_output owns cpp_out
    return py_output;
  }

}// end of namespace
#endif

