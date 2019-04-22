#include "IniSelect/UnitTests/Test_DstFile.h"
#include <stdio.h>

Test_DstFile::Test_DstFile(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator), fInputFile(eventSvc())
{}

void Test_DstFile::TestInitialize()
{
}

void Test_DstFile::TestExecute()
{
  fInputFile.LoadNextEvent();
  printf("run %8d, event %4d\n", fInputFile.RunNumber(), fInputFile.EventNumber());
}

void Test_DstFile::TestFinalize()
{
}