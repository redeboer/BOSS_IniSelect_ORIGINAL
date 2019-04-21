#include "IniSelect/UnitTests/Test_DstFile.h"
#include <stdio.h>

Test_DstFile::Test_DstFile(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator), fInputFile(eventSvc())
{}

StatusCode Test_DstFile::TestInitialize()
{
  return StatusCode::SUCCESS;
}

StatusCode Test_DstFile::TestExecute()
{
  fInputFile.LoadHeaders();
  printf("run %10d, event %10d\n", fInputFile.RunNumber(), fInputFile.EventNumber());
  fInputFile.IncrementCounters();
  return StatusCode::SUCCESS;
}

StatusCode Test_DstFile::TestFinalize()
{
  return StatusCode::SUCCESS;
}