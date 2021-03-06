/**
 *  @copyright Copyright 2016 The J-PET Framework Authors. All rights reserved.
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
 *  @file JPetTaskLoader.cpp
 *  @brief Class loads user task and execute in a loop of events
 */


#include "JPetTaskLoader.h"
#include <iostream>
#include "../JPetTask/JPetTask.h"
//ClassImp(JPetTaskLoader);

#include <boost/filesystem.hpp>

JPetTaskLoader::JPetTaskLoader(const char* in_file_type,
                               const char* out_file_type,
                               JPetTask* taskToExecute):
  JPetTaskIO(),
  fInFileType(in_file_type),
  fOutFileType(out_file_type)
{
  addSubTask(taskToExecute);
}



void JPetTaskLoader::init(const JPetOptions::Options& opts)
{
  auto newOpts(opts);
  auto inFile = newOpts.at("inputFile");
  auto outFile = inFile; /// @todo This line is potentially dangerous if the output directory is different than the input one.
  inFile = generateProperNameFile(inFile, fInFileType);
  outFile = generateProperNameFile(outFile, fOutFileType);
  newOpts.at("inputFile") = inFile;
  newOpts.at("inputFileType") = fInFileType;
  newOpts.at("outputFile") = outFile;
  newOpts.at("outputFileType") = fOutFileType;
  setOptions(JPetOptions(newOpts));
  
  //here we should call some function to parse options
  std::string inputFilename(fOptions.getInputFile());
  std::string outputPath(fOptions.getOutputPath());
  auto outputFilename = outputPath + std::string(fOptions.getOutputFile());
  createInputObjects(inputFilename.c_str());
  createOutputObjects(outputFilename.c_str());
}

std::string JPetTaskLoader::generateProperNameFile(const std::string& srcFilename, const std::string& fileType) const
{
  auto baseFileName = getBaseFilePath(srcFilename); 
  if (!fileType.empty()) {
    baseFileName = baseFileName + "." + fileType;
  }
  return baseFileName + ".root";
}

std::string JPetTaskLoader::getBaseFilePath(const std::string& srcName) const
{
  boost::filesystem::path p(srcName);
  // the file name and path are treated separately not to strip dots from the path
  std::string name = p.filename().native();
  boost::filesystem::path dir = p.parent_path().native();
  //strip the "extension" starting from the first dot in the file name
  auto pos = name.find(".");
  if ( pos != std::string::npos ) {
    name.erase( pos );
  }
  boost::filesystem::path bare_name(name);
  
  return (dir / bare_name).native();
}

JPetTaskLoader::~JPetTaskLoader()
{
}

