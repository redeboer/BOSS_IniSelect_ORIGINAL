#include "IniSelect/UnitTests/Test_CandidateSelection.h"
#include "TString.h"

Test_CandidateSelection::Test_CandidateSelection(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator),
  fInputFile(eventSvc())
{}

void Test_CandidateSelection::TestInitialize()
{}

void Test_CandidateSelection::TestExecute()
{
  fInputFile.LoadNextEvent();
}

void Test_CandidateSelection::TestFinalize()
{}