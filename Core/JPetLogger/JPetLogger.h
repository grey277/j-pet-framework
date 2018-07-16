/**
 *  @copyright Copyright 2018 The J-PET Framework Authors. All rights reserved.
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
 *  @file JPetLogger.h
 */

#ifndef JPETLOGGER_H
#define JPETLOGGER_H

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

#ifndef __CINT__
#include <boost/log/core.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#endif

/**
 * @brief Simple logger class.
 *
 * Logger class for printing out varius statusses in the resulting .log file
 * Don't use directly, rather by including JPetLoggerInclude.h
 * and using macros from there. It is wrapper for Boost.Log
 * that is multithread safe and implements own formatter.
*/

class JPetLogger {
public:
#ifndef __CINT__
  inline static boost::log::sources::severity_logger<boost::log::trivial::severity_level>& getSeverity() {
    static bool isInitialized = false;
    if (!isInitialized) {
      init();
      isInitialized = true;
    }
    static boost::log::sources::severity_logger<boost::log::trivial::severity_level> sev;
    return sev;
  }

  inline static void formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& out_stream) {
    boost::log::value_ref<std::string> fullpath = boost::log::extract<std::string>("File", rec);
    boost::log::value_ref<std::string> fullfunction = boost::log::extract<std::string>("Function", rec);
    out_stream << boost::log::extract<unsigned int>("LineID", rec) << ": [";
    out_stream << boost::filesystem::path(fullpath.get()).filename().string() << ":";
    out_stream << fullfunction << "@";
    out_stream << boost::log::extract<int>("Line", rec) << "] ";
    out_stream << "ThreadID: " << boost::log::extract<boost::log::attributes::current_thread_id::value_type>("ThreadID", rec) << " ";
    out_stream << "<" << rec[boost::log::trivial::severity] << "> ";
    out_stream << rec[boost::log::expressions::smessage];
  }

  inline static void setLogLevel(boost::log::trivial::severity_level level) { JPetLogger::sink->set_filter(boost::log::trivial::severity >= level); }
#else
  static void getSeverity();
  static void formatter();
  static void setLogLevel();
#endif

private:
  JPetLogger();
  JPetLogger(const JPetLogger&);
  JPetLogger& operator=(const JPetLogger&);

#ifndef __CINT__

  typedef boost::log::sinks::text_ostream_backend backend;
  typedef boost::log::sinks::synchronous_sink<backend> text_sink;
  static boost::shared_ptr<text_sink> sink;

  inline static const std::string generateFilename() {
    return std::string("JPet_") + to_string(boost::uuids::random_generator()()) + std::string(".log");
  }

  inline static void init() {
    JPetLogger::sink.reset(new text_sink(boost::make_shared<backend>()));
    JPetLogger::sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(generateFilename()));
    JPetLogger::sink->set_formatter(&JPetLogger::formatter);
    boost::log::core::get()->add_sink(JPetLogger::sink);
    boost::log::add_common_attributes();
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
  }
#endif
};

#endif /* !JPETLOGGER_H */
