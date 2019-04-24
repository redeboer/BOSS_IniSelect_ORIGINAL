#include "IniSelect/UnitTests/Test_CandidateTracks.h"
#include <iostream>

Test_DstFile::Test_DstFile(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator),
  fInputFile(eventSvc()),
  fCandidateTracks_pip("pi+", 2)
{}

void Test_CandidateTracks::TestInitialize()
{
  fCandidateTracks_g.SetParticle("g");
  fCandidateTracks_g.SetNParticles(5);

  fCandidateTracks_g.SetMultCut_AtLeast();
  fCandidateTracks_pip.SetMultCut_EqualTo();
}

void Test_CandidateTracks::TestExecute()
{
  fInputFile.LoadNextEvent();
  REQUIRE(fInputFile.RunNumber() == -10870);
  REQUIRE(fInputFile.EventNumber() == fCountEvent);
  REQUIRE(fInputFile.IsMonteCarlo() == true);
  ++fCountEvent;
}

void Test_CandidateTracks::TestFinalize()
{
  REQUIRE(fInputFile.TotalChargedTracks() == 2);
  REQUIRE(fInputFile.TotalNeutralTracks() == 11);
  REQUIRE(fInputFile.TotalTracks() == 13);
}