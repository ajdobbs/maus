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

/** @class JsonWrapper
 *  \brief Wrap the Json function calls with additional error handling
 *
 *  Convenience wrapper for Json functionality. Use of Json is somewhat verbose
 *  and requires lots of error checking etc. I wrap the Json function calls here
 *  in order to reduce verbosity.
 *
 *  Nb jargon:
 *  * Json "object" is like a map<std::string, Json::Value>.
 *  * Json "array" is a dynamic length array
 */

#ifndef _SRC_CPP_CORE_UTILS_JSONWRAPPER_HH
#define _SRC_CPP_CORE_UTILS_JSONWRAPPER_HH

#include <ostream>
#include <string>

#include "json/json.h"

#include "CLHEP/Vector/ThreeVector.h"

#include "src/legacy/Interface/STLUtils.hh"
#include "src/legacy/Interface/Squeal.hh"

namespace JsonWrapper {
  /** @brief List of data types allowed by json
   *
   *  List of data types allowed by json, with additional type "anyValue" for
   *  when we don't care.
   *
   *  Note Json always assumes integers are signed
   */
  enum JsonType {
    nullValue,     // 'null' value
    uintValue,     // unsigned integer value;
    intValue,      // signed integer value
    realValue,     // double value
    stringValue,   // UTF-8 string value.
    booleanValue,  // bool value
    arrayValue,    // array value (ordered list)
    objectValue,   // object value (collection of name/value pairs).
    anyValue       // object value (collection of name/value pairs).
  };

  /** @brief Convert a string to a Json::Value tree 
   *
   *  @param json_in raw string holding the configuration information
   *
   *  Read in a string and return as a Json value. Throw a Squeal if the reader
   *  fails to parse the string. The configuration is a dict of Json::Value,
   *  which is in itself a Json::Value.
   */
  Json::Value StringToJson(std::string json_in) throw(Squeal);

  /** @brief Convert a Json::Value tree to a std::string
   *
   *  @param json_in Json::Value to convert to a string
   */
  std::string JsonToString(const Json::Value& val);

  /** @brief Get an item from a Json array (variable length array)
   *
   *  @param array array of values from which we want to get a value
   *  @param value_index index of the value in the array
   *  @param value_type type of the value we want to get
   *
   *  Returns the Json::Value on success. Throws an exception of type Squeal on
   *  failure
   */
  Json::Value GetItem(Json::Value array, size_t value_index,
                                            JsonType value_type) throw(Squeal);

  /** @brief Get a property from a Json object (hash)
   *
   *  @param object the Json data tree
   *  @param value_name name of the branch we want to get
   *  @param value_type type of the value we want to get
   *
   *  Attempt to access a branch from Json. If the branch is not found, throw a
   *  Squeal.
   */
  Json::Value GetProperty
     (Json::Value object, std::string value_name,
                          JsonType value_type) throw(Squeal);


  /** @brief Convert from a json three vector to a CLHEP three vector
   *
   *  @param json_vec objectValue with realValue children "x", "y", "z". Throws
   *         an exception if the conversion fails.
   */
  CLHEP::Hep3Vector JsonToThreeVector
                                       (Json::Value json_vec) throw(Squeal);

  /** @brief Convert from Json::ValueType to JsonType
   */
  JsonType ValueTypeToJsonType(Json::ValueType tp);

  /** @brief Convert from Json::Value type to string
   */
  std::string ValueTypeToString(Json::ValueType tp);

  /** @brief Convert from JsonType to Json::ValueType
   */
  Json::ValueType JsonTypeToValueType(JsonType tp) throw(Squeal);

  /** @brief Return true if types are equal or anyValue
   */
  bool SimilarType(JsonType jt1, JsonType jt2);

  /** @brief Print the Json value to an ostream
   *
   *  Prints in json format to ostream out; so if passed to a stringstream, then
   *  StringToJson should read back in with no change
   */
  void Print(std::ostream& out, const Json::Value& val);

  /** @brief Check for equality between json values
   *
   *  Check that value_1 == value_2; when comparing reals, we allow a float
   *  tolerance given by tolerance. Recursively search through arrays and
   *  objects.
   *
   *  @param tolerance float tolerance - requirement is that
   *         fabs(float_1-float_2) < tolerance
   */
  bool AlmostEqual
                   (Json::Value value_1, Json::Value value_2, double tolerance);

  /** @brief Check for equality between json arrays
   *
   *  Check that value_1 == value_2, within float tolerance. Note that there is
   *  no type checking done here.
   */
  bool ArrayEqual
                   (Json::Value value_1, Json::Value value_2, double tolerance);

  /** @brief Check for equality between json objects
   *
   *  Check that value_1 == value_2, within float tolerance. Note that there is
   *  no type checking done here.
   */
  bool ObjectEqual
                   (Json::Value value_1, Json::Value value_2, double tolerance);


  /** @brief Merge two json objects
   *
   *  Merge two json objects. For each property in the object; if the property
   *  exists in object_1 or object_2 and is an array, append items from object_2
   *  array onto the back of the object_1 array; if the property exists in one
   *  object but not the other put it in the merged object; if the property
   *  exists in both objects but is not an array, throw an exception.
   */
  Json::Value ObjectMerge(Json::Value object_1, Json::Value object_2);

  /** @brief Merge two json arrays
   *
   *  Put items from array_2 onto the back of array_1
   */
  Json::Value ArrayMerge(Json::Value array_1, Json::Value array_2);

namespace Path {
  /** @brief Get the path from a json value
   *
   *  Path is stored in the comment field
   */
  std::string GetPath(Json::Value json);

  /** @brief Set the path to a json value
   *
   *  Path is stored in the comment field
   */
  void SetPath(Json::Value& json, std::string path);

  /** @brief Append the path to a json value
   *
   *  Path is stored in the comment field
   *  \param json Json::Value to which the path should be appended
   *  \param branch_name name of a branch in a Json object
   */
  void AppendPath(Json::Value& json, std::string branch_name);

  /** @brief Append the path to a json value
   *
   *  Path is stored in the comment field
   *  \param json Json::Value to which the path should be appended
   *  \param array_index index of a branch in a Json array
   */
  void AppendPath(Json::Value& json, size_t array_index);

  /** @brief Return the json value corresponding to a given path
   *
   *  Return the json value located at a given path. If the path cannot be
   *  accessed, throw a Squeal.
   */
  Json::Value& DereferencePath(Json::Value& json, std::string path);
}  // namespace Path
}  // namespace JsonWrapper

#endif


