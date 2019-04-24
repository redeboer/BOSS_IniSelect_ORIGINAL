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

  /// * Test `CandidateTracks::GetNParticles`
  REQUIRE(fCandidateTracks_g.GetNParticles() == 5);
  REQUIRE(fCandidateTracks_pip.GetNParticles() == 2);

  /// * Test `CandidateTracks::FailsMultiplicityCut` and `AddTrack`
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == true);
  AddDummyTracks(fCandidateTracks_g, 2);
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == true);
  AddDummyTracks(fCandidateTracks_g, 2);
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == true);
  AddDummyTracks(fCandidateTracks_g, 1);
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == false);

  REQUIRE(fCandidateTracks_pip.FailsMultiplicityCut() == true);
  AddDummyTracks(fCandidateTracks_pip, 2);
  REQUIRE(fCandidateTracks_pip.FailsMultiplicityCut() == false);
  AddDummyTracks(fCandidateTracks_pip, 1);
  REQUIRE(fCandidateTracks_pip.FailsMultiplicityCut() == true);

  /// * Test `CandidateTracks::SetMultCut_AtMost`
  fCandidateTracks_g.SetMultCut_AtMost();
  AddDummyTracks(fCandidateTracks_g, 1);
  REQUIRE(fCandidateTracks_g.FailsMultiplicityCut() == true);

  /// * Test `CandidateTracks::Clear`
  fCandidateTracks_pip.Clear();
  REQUIRE(fCandidateTracks_pip.FailsMultiplicityCut() == true);

  /// * Test `CandidateTracks::GetTracks`
  REQUIRE(fCandidateTracks_pip.GetTracks().size() == 0);
  AddDummyTracks(fCandidateTracks_pip, 2);
  REQUIRE(fCandidateTracks_pip.GetTracks().size() == 2);

  /// * Test `CandidateTracks::GetTracks`
}

void Test_CandidateTracks::TestExecute()
{
  fInputFile.LoadNextEvent();
}

void Test_CandidateTracks::TestFinalize()
{
}

void Test_CandidateTracks::AddDummyTracks(CandidateTracks<Event::McParticle>& obj, int n)
{
  for(int i = 0; i < n; ++i) obj.AddTrack(nullptr);
}