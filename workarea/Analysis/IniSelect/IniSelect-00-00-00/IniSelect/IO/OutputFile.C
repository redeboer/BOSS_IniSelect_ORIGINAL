#include "GaudiKernel/DeclareFactoryEntries.h"

#include "IniSelect/UnitTests/UnitTester.h"
#include "IniSelect/IO/OutputFile.h"
#include <iostream>
using namespace std;

class OutputFile_test : public UnitTester
{
public:
  OutputFile_test(const std::string& name, ISvcLocator* pSvcLocator) :
    UnitTester(name, pSvcLocator)
  {}

  void TestInitialize() {}
  void TestExecute() {}
  void TestEvent() {}
  void TestFinalize() {}
};

DECLARE_ALGORITHM_FACTORY(OutputFile_test)
DECLARE_FACTORY_ENTRIES(OutputFile_test)
{
  DECLARE_ALGORITHM(OutputFile_test);
}