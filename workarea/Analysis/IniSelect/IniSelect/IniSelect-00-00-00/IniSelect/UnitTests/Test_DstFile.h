#ifndef Analysis_IniSelect_UnitTests_DstFile_H
#define Analysis_IniSelect_UnitTests_DstFile_H

#include "IniSelect/UnitTests/UnitTester.h"
#include "IniSelect/Containers/DstFile.h"

class Test_DstFile : public UnitTester
{
public:
  Test_DstFile(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode TestInitialize();
  StatusCode TestExecute();
  StatusCode TestFinalize();

private:
  DstFile fInputFile;

};

#endif