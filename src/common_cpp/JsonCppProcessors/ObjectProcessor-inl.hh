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

#ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSOR_INL_HH_
#define _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSOR_INL_HH_

#include <string>

#include "src/common_cpp/Utils/JsonWrapper.hh"

namespace MAUS {

/** BaseItem is base representation of a particular data structure branch
 *
 *  The processor holds information on each of the branches for a particular C++
 *  type in a vector. Different sorts of branches (is it stored as a pointer? is
 *  it stored by a value) share this interface so they can be stored in the same
 *  vector.
 *
 *  @tparam ParentType C++ parent object on which branches are to be registered
 */
template <class ParentType>
class BaseItem {
  public:
    /** Destructor
     */
    virtual ~BaseItem() {}

    /** SetCppChild set the child data on the C++ representation
     */
    virtual void SetCppChild
                  (const Json::Value& parent_json, ParentType& parent_cpp) = 0;
    /** SetCppChild set the child data on the json representation
     */
    virtual void SetJsonChild
                  (const ParentType& parent_cpp, Json::Value& parent_json) = 0;

    /** Get the branch name */
    virtual std::string GetBranchName() const = 0;

  protected:
};

/** @class PointerItem pointer branch for a Cpp object
 *
 *  Probably don't want to use this class, instead use Register...Branch methods
 *  directly on ObjectProcessor
 *
 *  @tparam ParentType type of the parent object
 *  @tparam ChildType type of the child object referenced by the branch. Should
 *  use the actual type even if the target is a pointer
 */
template <class ParentType, class ChildType>
class PointerItem : public BaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType* value);
    typedef ChildType* (ParentType::*GetMethod)() const;

    /** Constructor
     *
     *  @param branch_name name used by json to reference the branch
     *  @param child_processor processor that will be used to convert the
     *  representation of the child types
     *  @param GetMethod callback that will return a pointer to the child data,
     *  where memory is still owned by the ObjectProcessor
     *  @param SetMethod callback that will set a pointer to the child data,
     *  where memory is given to the ObjectProcessor
     *  @param is_required if the branch doesnt exist in json, is null in json
     *  or is NULL in C++, throw Squeal if is_required is set to true when
     *  Set...Child methods are called
     */
    PointerItem(std::string branch_name, ProcessorBase<ChildType>* child_processor,
                GetMethod getter, SetMethod setter, bool is_required)
        : BaseItem<ParentType>(), _branch(branch_name),
          _processor(child_processor), _setter(setter),
          _getter(getter), _required(is_required) {
    }

    /** SetCppChild using data from parent_json
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object where C++ data is put
     */
    void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        if (!parent_json.isMember(_branch)) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Missing required branch "+_branch+" converting json->cpp",
                "PointerItem::SetCppChild");
            } else {
                return;
            }
        }
        if (parent_json[_branch].isNull()) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Null branch "+_branch+" converting json->cpp",
                "PointerItem::SetCppChild");
            } else {
                return;
            }
        }
        Json::Value child_json = parent_json[_branch];
        ChildType* child_cpp = _processor->JsonToCpp(child_json);
        // syntax is (_object.*_function)(args);
        (parent_cpp.*_setter)(child_cpp);
    }

    /** SetJsonChild using data from parent_cpp
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        ChildType* child_cpp = (parent_cpp.*_getter)();
        if (child_cpp == NULL) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Failed to find branch "+_branch+": class data was NULL",
                "PointerItem::GetCppChild");
            } else {
                return;
            }
        }
        Json::Value* child_json = _processor->CppToJson(*child_cpp);
        parent_json[_branch] = *child_json;
        delete child_json;
    }

    /** Get the branch name */
    std::string GetBranchName() const {return _branch;}

  private:
    std::string _branch;
    ProcessorBase<ChildType>* _processor;
    SetMethod _setter;
    GetMethod _getter;
    bool      _required;
};

/** @class ValueItem value branch for a Cpp object
 *
 *  Probably don't want to use this class, instead use Register...Branch methods
 *  directly on ObjectProcessor
 *
 *  @tparam ParentType type of the parent object
 *  @tparam ChildType type of the child object referenced by the branch. Should
 *  use the actual type even if the target is a pointer
 */
template <class ParentType, class ChildType>
class ValueItem : public BaseItem<ParentType> {
  public:
    typedef void (ParentType::*SetMethod)(ChildType value);
    typedef ChildType (ParentType::*GetMethod)() const;

    /** Constructor
     *
     *  @param branch_name name used by json to reference the branch
     *  @param child_processor processor that will be used to convert the
     *  representation of the child types
     *  @param GetMethod callback that will return the value of the child data
     *  associated with this branch
     *  @param SetMethod callback that will set the value of the child data
     *  associated with this branch
     *  @param is_required if the branch doesnt exist in json, throw Squeal if
     *  is_required is set to true
     */
    ValueItem(std::string branch_name, ProcessorBase<ChildType>* child_processor,
                GetMethod getter, SetMethod setter, bool is_required)
                          : BaseItem<ParentType>(), _branch(branch_name),
                            _processor(child_processor), _setter(setter),
      _getter(getter), _required(is_required) {
    }

    // Set the child in the ParentInstance
    /** SetCppChild using data from parent_json
     *
     *  @param parent_json json object whose child branch is used to make C++
     *  representation
     *  @param parent_cpp C++ object where C++ data is put
     */
    void SetCppChild(const Json::Value& parent_json, ParentType& parent_cpp) {
        if (!parent_json.isMember(_branch)) {
            if (_required) {
                throw Squeal(Squeal::recoverable,
                "Missing required branch "+_branch+" converting json->cpp",
                "PointerItem::SetCppChild");
            } else {
                return;
            }
        }
        Json::Value child_json = parent_json[_branch];
        ChildType* child_cpp = _processor->JsonToCpp(child_json);
        // syntax is (_object.*_function)(args);
        (parent_cpp.*_setter)(*child_cpp);
        delete child_cpp;
    }

    /** SetJsonChild using data from parent_cpp
     *
     *  @param parent_cpp C++ object from whence C++ data is got
     *  @param parent_json json object whose child branch is set using C++
     *  representation
     */
    void SetJsonChild(const ParentType& parent_cpp, Json::Value& parent_json) {
        ChildType child_cpp = (parent_cpp.*_getter)();
        Json::Value* child_json = _processor->CppToJson(child_cpp);
        parent_json[_branch] = *child_json;
        delete child_json;
    }

    /** Get the branch name */
    std::string GetBranchName() const {return _branch;}

  private:
    std::string _branch;
    ProcessorBase<ChildType>* _processor;
    SetMethod _setter;
    GetMethod _getter;
    bool      _required;
};


template <class ObjectType>
ObjectProcessor<ObjectType>::ObjectProcessor()
    : _throws_if_unknown_branches(true), _items() {
}

template <class ObjectType>
template <class ChildType>
void ObjectProcessor<ObjectType>::RegisterPointerBranch(
                std::string branch_name,
                ProcessorBase<ChildType>* child_processor,
                ChildType* (ObjectType::*GetMethod)() const,
                void (ObjectType::*SetMethod)(ChildType* value),
                bool is_required) {
    BaseItem<ObjectType>* item = new PointerItem<ObjectType, ChildType>
              (branch_name, child_processor, GetMethod, SetMethod, is_required);
    _items[branch_name] = item;
}

template <class ObjectType>
template <class ChildType>
void ObjectProcessor<ObjectType>::RegisterValueBranch(
                std::string branch_name,
                ProcessorBase<ChildType>* child_processor,
                ChildType (ObjectType::*GetMethod)() const,
                void (ObjectType::*SetMethod)(ChildType value),
                bool is_required) {
    BaseItem<ObjectType>* item = new ValueItem<ObjectType, ChildType>
              (branch_name, child_processor, GetMethod, SetMethod, is_required);
    _items[branch_name] = item;
}

template <class ObjectType>
ObjectType* ObjectProcessor<ObjectType>::JsonToCpp(const Json::Value& json_object) {
    if (json_object.type() != Json::objectValue) {
        std::string tp = JsonWrapper::ValueTypeToString(json_object.type());
        throw(Squeal(Squeal::recoverable,
                     "Attempt to pass a json "+tp+" type as an object",
                     "ObjectProcessor<ObjectType>::JsonToCpp"));
    }
    if (_throws_if_unknown_branches && HasUnknownBranches(json_object)) {
        throw(Squeal(Squeal::recoverable,
                     "Failed to recognise all json properties",
                     "ObjectProcessor<ObjectType>::JsonToCpp"));
    }
    ObjectType* cpp_object = new ObjectType();
    for (my_iter it = _items.begin(); it != _items.end(); ++it) {
        try {
            it->second->SetCppChild(json_object, *cpp_object);
        } catch(Squeal squee) {
            delete cpp_object;
            squee.SetMessage("In branch "+it->first+"\n"
                            +squee.GetMessage());
            throw squee;
        }
    }
    return cpp_object;
}

template <class ObjectType>
Json::Value* ObjectProcessor<ObjectType>::CppToJson(const ObjectType& cpp_object) {
    Json::Value* json_object = new Json::Value(Json::objectValue);
    for (my_iter it = _items.begin(); it != _items.end(); ++it) {
        try {
            it->second->SetJsonChild(cpp_object, *json_object);
        } catch(Squeal squee) {
            delete json_object;
            squee.SetMessage("In branch "+it->first+"\n"
                            +squee.GetMessage());
            throw squee;
        }
    }
    return json_object;
}

template <class ObjectType>
ObjectProcessor<ObjectType>::~ObjectProcessor() {
    for (my_iter it = _items.begin(); it != _items.end(); ++it) {
        delete it->second;
    }
}

template <class ObjectType>
bool ObjectProcessor<ObjectType>::HasUnknownBranches
                                              (const Json::Value& value) const {
    if (!value.isObject()) {
        std::string tp = JsonWrapper::ValueTypeToString(value.type());
        throw(Squeal(Squeal::recoverable,
                     "Comparison value must be a json object type - found "+tp,
                     "ObjectProcessor::HasUnknownBranches(...)"));
    }
    Json::Value::Members members = value.getMemberNames();
    for (Json::Value::Members::iterator it = members.begin();
                                                    it != members.end(); ++it) {
        if (_items.find(*it) == _items.end()) {
            return true;
        }
    }
    return false;
}

template <class ObjectType>
void ObjectProcessor<ObjectType>::SetThrowsIfUnknownBranches(bool will_throw) {
    _throws_if_unknown_branches = will_throw;
}

template <class ObjectType>
bool ObjectProcessor<ObjectType>::GetThrowsIfUnknownBranches() const {
    return _throws_if_unknown_branches;
}
}  // namespace MAUS
#endif  // #ifndef _SRC_COMMON_CPP_JSONCPPPROCESSORS_OBJECTPROCESSOR_INL_HH_
