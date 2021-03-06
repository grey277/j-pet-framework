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
 *  @file JPetManager.h
 *  @brief Main manager of the analysis performed with the J-PET Framework.
 */

#ifndef JPETMANAGER_H
#define JPETMANAGER_H

#include "../JPetOptions/JPetOptions.h"
#include "../JPetTaskExecutor/JPetTaskExecutor.h"

/**
 * @brief Main manager of the analysis performed with the J-PET Framework.
 *
 * Each analysis program needs an instance of the JPetManager which is responsible for parsing the command line arguments
 * registering processing tasks, and
 * sending it to JPetExecutor which executes the registered tasks in threads.
 */

class JPetManager
{
public:
  static JPetManager& getManager();
  ~JPetManager();
  bool run();
  void registerTask(const TaskGenerator& taskGen);
  void parseCmdLine(int argc, char** argv);
  inline std::vector<JPetOptions> getOptions() const {
    return fOptions;
  }

private:
  JPetManager() {
    fTaskGeneratorChain = new TaskGeneratorChain;
  }
  JPetManager(const JPetManager&);
  void operator=(const JPetManager&);

  std::vector<JPetOptions> fOptions;
  TaskGeneratorChain* fTaskGeneratorChain;
};
#endif /*  !JPETMANAGER_H */
