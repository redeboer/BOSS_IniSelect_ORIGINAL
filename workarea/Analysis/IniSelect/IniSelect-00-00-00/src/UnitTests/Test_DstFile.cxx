#include "IniSelect/UnitTests/Test_DstFile.h"
#include <iostream>

Test_DstFile::Test_DstFile(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator), fInputFile(eventSvc()), fCountEvent(0)
{}

void Test_DstFile::TestInitialize()
{
  fCountEvent = 50001;
}

void Test_DstFile::TestExecute()
{
  fInputFile.LoadNextEvent();
  TEST_CASE(fInputFile.RunNumber() == -10870);
  TEST_CASE(fInputFile.EventNumber() == fCountEvent);
  TEST_CASE(fInputFile.IsMonteCarlo() == true);
  ++fCountEvent;
}

void Test_DstFile::TestFinalize()
{
  TEST_CASE(fInputFile.TotalChargedTracks() == 2);
  TEST_CASE(fInputFile.TotalNeutralTracks() == 11);
  TEST_CASE(fInputFile.TotalTracks() == 13);
}