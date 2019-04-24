#include "IniSelect/UnitTests/Test_CandidateTracks.h"
#include "TString.h"

Test_CandidateTracks::Test_CandidateTracks(const std::string& name, ISvcLocator* pSvcLocator) :
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

  /// * Test `CandidateTracks::GetPdtName`
  TString g_name(fCandidateTracks_g.GetPdtName());
  TString pip_name(fCandidateTracks_pip.GetPdtName());
  REQUIRE(g_name.EqualTo("g"));
  REQUIRE(pip_name.EqualTo("pi+"));

  REQUIRE(fCandidateTracks_g.GetNParticles() == 5);
  REQUIRE(fCandidateTracks_pip.GetNParticles() == 2);

  /// * Test `CandidateTracks::FailsMultiplicityCut`
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == true);
  fCandidateTracks_g.AddTrack(nullptr);
  fCandidateTracks_g.AddTrack(nullptr);
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == true);
  fCandidateTracks_g.AddTrack(nullptr);
  fCandidateTracks_g.AddTrack(nullptr);
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == true);
  fCandidateTracks_g.AddTrack(nullptr);
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == false);

  REQUIRE(fCandidateTracks_pip.FailsMultiplicityCut() == true);
  fCandidateTracks_pip.AddTrack(nullptr);
  fCandidateTracks_pip.AddTrack(nullptr);
  REQUIRE(fCandidateTracks_pip.FailsMultiplicityCut() == false);
  fCandidateTracks_pip.AddTrack(nullptr);
  REQUIRE(fCandidateTracks_pip.FailsMultiplicityCut() == true);

  /// * Test `CandidateTracks::SetMultCut_AtMost`
  fCandidateTracks_g.SetMultCut_AtMost();
  fCandidateTracks_g.AddTrack(nullptr);
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == true);

  /// * Test `CandidateTracks::Clear`
  fCandidateTracks_pip.Clear();
  REQUIRE(fCandidateTracks_pip.FailsMultiplicityCut() == true);

  /// * Test `CandidateTracks::GetTracks`
  REQUIRE(fCandidateTracks_pip.GetTracks().size() == 0);
  fCandidateTracks_pip.AddTrack(nullptr);
  fCandidateTracks_pip.AddTrack(nullptr);
  REQUIRE(fCandidateTracks_pip.GetTracks().size() == 2);
}

void Test_CandidateTracks::TestExecute()
{
  fInputFile.LoadNextEvent();
}

void Test_CandidateTracks::TestFinalize()
{
}