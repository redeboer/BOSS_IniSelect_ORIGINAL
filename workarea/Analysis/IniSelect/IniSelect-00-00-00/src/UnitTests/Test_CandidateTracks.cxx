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
  TEST_CASE(fInputFile.RunNumber() == -10870);
  TEST_CASE(fInputFile.EventNumber() == fCountEvent);
  TEST_CASE(fInputFile.IsMonteCarlo() == true);
  ++fCountEvent;
}

void Test_CandidateTracks::TestFinalize()
{
  TEST_CASE(fInputFile.TotalChargedTracks() == 2);
  TEST_CASE(fInputFile.TotalNeutralTracks() == 11);
  TEST_CASE(fInputFile.TotalTracks() == 13);
}