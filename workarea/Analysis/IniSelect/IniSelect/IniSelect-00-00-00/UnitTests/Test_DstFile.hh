#ifndef Analysis_IniSelect_UnitTests_DstFile_H
#define Analysis_IniSelect_UnitTests_DstFile_H

#include "IniSelect/UnitTester.h"
#include "IniSelect/Containers/DstFile.h"

class Test_DstFile : public UnitTester
{
public:
  Test_DstFile(const std::string& name, ISvcLocator* pSvcLocator) :
    UnitTester(name, pSvcLocator),
    fInputFile(eventSvc())
  {}

  StatusCode TestInitialize();
  StatusCode TestExecute();
  StatusCode TestFinalize();

private:
  DstFile fInputFile;

};

StatusCode Test_DstFile::TestInitialize()
{
  return StatusCode::SUCCESS;
}

StatusCode Test_DstFile::TestExecute()
{
  fInputFile.IncrementCounters();
  return StatusCode::SUCCESS;
}

StatusCode Test_DstFile::TestFinalize()
{
  return StatusCode::SUCCESS;
}

#endif