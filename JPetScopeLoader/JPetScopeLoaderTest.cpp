#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ScopeReader
#define BOOST_TEST_LOG_LEVEL message
#include <cstddef>
#include <functional>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "../JPetManager/JPetManager.h"

//#include "JPetScopeLoaderFixtures.h"
#include "../JPetScopeLoader/JPetScopeLoader.h"

char* convertStringToCharP(const std::string& s)
{
  char* pc = new char[s.size() + 1];
  std::strcpy(pc, s.c_str());
  return pc;
}

std::vector<char*> createArgs(const std::string& commandLine) 
{
  std::istringstream iss(commandLine);
  std::vector<std::string> args {std::istream_iterator<std::string>{iss},
                                 std::istream_iterator<std::string>{}
                                };
  std::vector<char*> args_char;
  std::transform(args.begin(), args.end(), std::back_inserter(args_char), convertStringToCharP);
  return args_char;
}


BOOST_AUTO_TEST_SUITE (JPetScopeLoaderTestSuite)

BOOST_AUTO_TEST_CASE (getFilePrefix) 
{
  JPetDBParamGetter::clearParamCache();
  JPetScopeParamGetter::clearParamCache();
  JPetScopeLoader reader(0);
  BOOST_REQUIRE(reader.getFilePrefix("").empty());
  BOOST_REQUIRE(reader.getFilePrefix("abkabd").empty());
  BOOST_REQUIRE(reader.getFilePrefix("jfsd808").empty());
  BOOST_REQUIRE_EQUAL(reader.getFilePrefix("c1_0554.txt"), "c1");
  BOOST_REQUIRE_EQUAL(reader.getFilePrefix("c098_0554.txt"), "c098");
  BOOST_REQUIRE_EQUAL(reader.getFilePrefix("cq_"), "cq");
  BOOST_REQUIRE_EQUAL(reader.getFilePrefix("a_ss_q"), "a");
}

BOOST_AUTO_TEST_CASE (createInputScopeFileNames) 
{
  JPetDBParamGetter::clearParamCache();
  JPetScopeParamGetter::clearParamCache();
  JPetScopeLoader reader(0);
  BOOST_REQUIRE(reader.createInputScopeFileNames("", {}).empty());
  BOOST_REQUIRE(reader.createInputScopeFileNames("non_existing", {}).empty());
  BOOST_REQUIRE(!reader.createInputScopeFileNames("unitTestData/JPetScopeLoaderTest/scope_files/0", {{"C1",0}, {"C2",1}, {"C3", 2}, {"C4",3}}).empty());
  std::map<std::string, int>  expectedRes0 {
      {"C1_00003.txt",0}, {"C1_00004.txt", 0},  {"C2_00003.txt",1}, {"C2_00004.txt",1},   
    {"C3_00003.txt",2},{"C3_00004.txt", 2}, {"C4_00003.txt",3},{"C4_00004.txt", 3}
  };
  std::map<std::string, int>  expectedRes;
  std::string pathToFiles = "unitTestData/JPetScopeLoaderTest/scope_files/0";
  for (const auto& el: expectedRes0) {
    expectedRes[pathToFiles + "/" + el.first] = el.second;
  }
  std::map<std::string, int> obtainedRes = reader.createInputScopeFileNames(pathToFiles, {{"C1",0}, {"C2",1}, {"C3", 2}, {"C4",3}});
  BOOST_REQUIRE(!obtainedRes.empty());
  BOOST_REQUIRE_EQUAL(obtainedRes.size(), expectedRes.size());
  auto it2 = obtainedRes.begin();
  for (auto it =expectedRes.begin(); it != expectedRes.end(); ++it) {
    BOOST_REQUIRE_EQUAL(it->first, it2->first);
    BOOST_REQUIRE_EQUAL(it->second, it2->second);
    ++it2;
  }
}

BOOST_AUTO_TEST_CASE (isCorrectScopeFileName) 
{
  JPetDBParamGetter::clearParamCache();
  JPetScopeParamGetter::clearParamCache();
  JPetScopeLoader reader(0);
  BOOST_REQUIRE(!reader.isCorrectScopeFileName(""));
  BOOST_REQUIRE(!reader.isCorrectScopeFileName("C1_004.gif"));
  BOOST_REQUIRE(!reader.isCorrectScopeFileName("C1004.txt"));
  BOOST_REQUIRE(!reader.isCorrectScopeFileName("004.txt"));
  BOOST_REQUIRE(!reader.isCorrectScopeFileName("C5_abc.txt"));
  BOOST_REQUIRE(reader.isCorrectScopeFileName("C1_023.txt"));
  BOOST_REQUIRE(reader.isCorrectScopeFileName("C5_000.txt"));
  BOOST_REQUIRE(reader.isCorrectScopeFileName("AA_004.txt"));
  BOOST_REQUIRE(reader.isCorrectScopeFileName("AA_004.txt"));
}

BOOST_AUTO_TEST_CASE (generate_root_file) {
  JPetDBParamGetter::clearParamCache();
  JPetScopeParamGetter::clearParamCache();
  const char* test_root_filename = "unitTestData/JPetScopeLoaderTest/test_file_test_0.reco.sig.root";
  boost::filesystem::remove(test_root_filename); 
  auto commandLine = "main.exe  -t scope -f unitTestData/JPetScopeLoaderTest/test_file.json";
  auto args_char = createArgs(commandLine);
  auto argc = args_char.size();
  auto argv = args_char.data();

  JPetManager& manager = JPetManager::getManager();
  manager.parseCmdLine(argc, argv);
  manager.run();
  BOOST_REQUIRE_MESSAGE(boost::filesystem::exists(test_root_filename), "File " << test_root_filename << " does not exist.");
}

BOOST_AUTO_TEST_CASE (position_does_not_exist) {
  JPetDBParamGetter::clearParamCache();
  JPetScopeParamGetter::clearParamCache();

  const char* test_root_filename = "unitTestData/JPetScopeLoaderTest/wrong_file_test_30.reco.sig.root";
  boost::filesystem::remove(test_root_filename); 
  auto commandLine = "main.exe  -t scope -f unitTestData/JPetScopeLoaderTest/wrong_file.json"; 
  //contains a single position 30 that does not exist
  auto args_char = createArgs(commandLine);
  auto argc = args_char.size();
  auto argv = args_char.data();

  JPetManager& manager = JPetManager::getManager();
  manager.parseCmdLine(argc, argv);
  manager.run();
  BOOST_REQUIRE_MESSAGE(!boost::filesystem::exists(test_root_filename), "File " << test_root_filename << " exists.");
}

BOOST_AUTO_TEST_CASE (folder_does_not_exist) 
{
  JPetDBParamGetter::clearParamCache();
  JPetScopeParamGetter::clearParamCache();
  const char* test_root_filename = "unitTestData/JPetScopeLoaderTest/wrong_file2_test_0.reco.sig.root";
  boost::filesystem::remove(test_root_filename); 
  auto commandLine = "main.exe  -t scope -f unitTestData/JPetScopeLoaderTest/wrong_file2.json"; 
  //contains a wrong data folder name
  auto args_char = createArgs(commandLine);
  auto argc = args_char.size();
  auto argv = args_char.data();

  JPetManager& manager = JPetManager::getManager();
  manager.parseCmdLine(argc, argv);
  manager.run();
  BOOST_REQUIRE_MESSAGE(!boost::filesystem::exists(test_root_filename), "File " << test_root_filename << " exists.");
}

BOOST_AUTO_TEST_CASE (generate_root_file2) {
  JPetDBParamGetter::clearParamCache();
  JPetScopeParamGetter::clearParamCache();

  const char* test_root_filename1 = "unitTestData/JPetScopeLoaderTest/test_file2_test_0.reco.sig.root";
  const char* test_root_filename2 = "unitTestData/JPetScopeLoaderTest/test_file2_test_1.reco.sig.root";
  boost::filesystem::remove(test_root_filename1); 
  boost::filesystem::remove(test_root_filename2); 
  auto commandLine = "main.exe  -t scope -f unitTestData/JPetScopeLoaderTest/test_file2.json";
  auto args_char = createArgs(commandLine);
  auto argc = args_char.size();
  auto argv = args_char.data();

  JPetManager& manager = JPetManager::getManager();
  manager.parseCmdLine(argc, argv);
  manager.run();
  BOOST_REQUIRE_MESSAGE(boost::filesystem::exists(test_root_filename1), "File " << test_root_filename1 << " does not exist.");
  BOOST_REQUIRE_MESSAGE(boost::filesystem::exists(test_root_filename2), "File " << test_root_filename2 << " does not exist.");
}

//BOOST_FIXTURE_TEST_CASE (signal_generation_test, signal_generation_fixture) {

  //check_header(
    //[] (int osc_file_size, int reco_signal_size) -> void {
      //BOOST_REQUIRE_EQUAL(osc_file_size, reco_signal_size);
    //}
  //);

  //check_data(
    //[] (float osc_file_time, float reco_signal_time, float osc_file_ampl, float reco_signal_ampl) -> void {
      //BOOST_CHECK_CLOSE_FRACTION(osc_file_time, reco_signal_time, 1.f / 131072.f);
      //BOOST_CHECK_CLOSE_FRACTION(osc_file_ampl, reco_signal_ampl, 1.f / 131072.f);
    //}
  //);
//}
//BOOST_FIXTURE_TEST_CASE (tref_correctness_test, tref_correctness_fixture) {
  
  //check_tref_simple(
    //[] (const void* tref_pointer) -> void {
      //BOOST_CHECK_PREDICATE(std::not_equal_to <size_t> (), ((size_t) tref_pointer)((size_t) nullptr));
    //}
  //);
  ////check_tref_simple(nullptr);
//}


BOOST_AUTO_TEST_SUITE_END()
