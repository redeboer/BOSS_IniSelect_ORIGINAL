#include "IniSelect/UnitTests/Test_CandidateSelection.h"
#include "TString.h"
#include <iostream>

Test_CandidateSelection::Test_CandidateSelection(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator)
{
  /// * Test `CandidateSelection::SetCandidateToN`
  fSelection.SetCandidateToN("pi+", 2);
  fSelection.SetCandidateToN("K-", 3);
  fSelection.SetCandidateToN("g", 5);

  /// * Test `CandidateSelection::SetCandidateToN`
  REQUIRE(fSelection.HasCandidate("pi+") == true);
  REQUIRE(fSelection.HasCandidate("K-") == true);
  REQUIRE(fSelection.HasCandidate("g") == true);
  REQUIRE(fSelection.HasCandidate("K+") == false);

  /// * Test `CandidateSelection::GetNCharged`
  REQUIRE(fSelection.GetNCharged() == 5);
}

void Test_CandidateSelection::TestEvent()
{
  
}