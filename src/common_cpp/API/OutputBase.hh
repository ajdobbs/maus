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
#ifndef _MAUS_API_OUTPUT_BASE_H
#define _MAUS_API_OUTPUT_BASE_H
#include <string>
#include <exception>
#include "API/IOutput.hh"
#include "API/ModuleBase.hh"
#include "API/APIExceptions.hh"
#include "Interface/Squeal.hh"
#include "Utils/CppErrorHandler.hh"

namespace MAUS {

  template <typename T>
  class OutputBase : public virtual IOutput<T>, public ModuleBase {

  public:
    explicit OutputBase(const std::string&);
    OutputBase(const OutputBase&);
    virtual ~OutputBase();

  public:
    bool save(T* t);

  private:
    virtual bool _save(T* t) = 0;
  };

  template <typename T>
  OutputBase<T>::OutputBase(const std::string& s) : IOutput<T>(), ModuleBase(s) {}

  template <typename T>
  OutputBase<T>::OutputBase(const OutputBase& ob) : IOutput<T>(), ModuleBase(ob._classname) {}

  template <typename T>
  OutputBase<T>::~OutputBase() {}

  template <typename T>
  bool OutputBase<T>::save(T* t) {
    if (!t) { throw NullInputException(_classname); }
    bool ret = false;
    try {
      ret = _save(t);
    }
    catch(Squeal& s) {
      CppErrorHandler::getInstance()->HandleSquealNoJson(s, _classname);
    }
    catch(std::exception& e) {
      CppErrorHandler::getInstance()->HandleStdExcNoJson(e, _classname);
    }
    catch(...) {
      throw UnhandledException(_classname);
    }
    return ret;
  }

}// end of namespace
#endif
