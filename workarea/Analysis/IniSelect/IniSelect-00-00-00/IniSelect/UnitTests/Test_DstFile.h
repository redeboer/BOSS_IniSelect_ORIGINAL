#ifndef Analysis_IniSelect_UnitTests_DstFile_H
#define Analysis_IniSelect_UnitTests_DstFile_H

#include "IniSelect/UnitTests/UnitTester.h"

class Test_DstFile : public UnitTester
{
public:
  Test_DstFile(const std::string& name, ISvcLocator* pSvcLocator);

  void TestInitialize();
  void TestExecute();
  void TestEvent();
  void TestFinalize();

private:
  int fCountEvent;
  int fNMCTracks;
  int fNChargedTracks;
  int fNNeutralTracks;
};

#endif