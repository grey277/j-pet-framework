/**
 *  @copyright Copyright 2019 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @file JPetAnyMap.h
 */

#ifndef _JPET_ANYMAP_H_
#define _JPET_ANYMAP_H_

#include "JPetLoggerInclude.h"
#include "JPetWriter/JPetWriter.h"
#include <TString.h>
#include <boost/any.hpp>
#include <string>

class JPetAnyMap : public TObject
{
public:
  explicit JPetAnyMap(std::string name) : fName(name) {}
  ~JPetAnyMap() {}

  void saveMapToFile(JPetWriter* writer)
  {
    assert(writer);
    if (!writer->isOpen())
    {
      ERROR("Could not write anymap(" + name + ") to file. The provided JPetWriter is closed.");
      return false;
    }
    writer->writeObject(this, name.c_str());
    return true;
  }

  /* @brief Warning! This method to not check if returned variable is valid.
   *
   *
   */
  static JPetAnyMap* readMapFromFile(std::string filename, std::string name)
  {
    TFile file(filename.c_str(), "READ");
    if (!file.IsOpen())
    {
      ERROR("Could not read from file.");
      return false;
    }
    JPetAnyMap* map = static_cast<JPetAnyMap*>(file.Get(name));
    return map;
  }

  void addVariableToMap(TString name, boost::any object) { fAnyMap[name] = object; }

  template <typename T>
  T getVariableFromMap(TString name)
  {
    try
    {
      return boost::any_cast<T>(fAnyMap[name]);
    }
    catch (const std::exception& excep)
    {
      T emptyT;
      ERROR("Could not find: " + name + " in map table, returning empty object, exception: " + std::string(excep.what()));
      return emptyT;
    }
  }

  ClassDef(JPetAnyMap, 1);

private:
  std::map<TString, boost::any> fAnyMap; // for passing any type of variables between tasks and save them to root file.
  std::string fName;                     // name to save in root file.
};

#endif /* !_JPET_ANYMAP_H_ */
