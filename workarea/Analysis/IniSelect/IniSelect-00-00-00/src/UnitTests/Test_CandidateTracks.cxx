#include "IniSelect/UnitTests/Test_CandidateTracks.h"
#include "TString.h"
#include <iostream>

Test_CandidateTracks::Test_CandidateTracks(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator),
  fInputFile(eventSvc()),
  fSetObject("pi+", 2),
  fKaons("K-", 3)
{}

void Test_CandidateTracks::TestInitialize()
{
  /// * Test `CandidateTracks::Set*`
  fConstructed.SetParticle("g");
  fConstructed.SetNParticles(5);

  fConstructed.SetMultCut_AtLeast();
  fSetObject.SetMultCut_EqualTo();

  /// * Test `CandidateTracks::GetPdtName`
  TString g_name(fConstructed.GetPdtName());
  TString pip_name(fSetObject.GetPdtName());
  REQUIRE(g_name.EqualTo("g"));
  REQUIRE(pip_name.EqualTo("pi+"));

  /// * Test other particle properties
  REQUIRE(fConstructed.GetPdgCode() == 21);
  REQUIRE(fSetObject.GetPdgCode() == 211);
  REQUIRE(fKaons.GetPdgCode() == -321);
  REQUIRE(fConstructed.IsCharged() == false);
  REQUIRE(fSetObject.IsCharged() == true);
  REQUIRE(fKaons.IsCharged() == true);

  /// * Test `CandidateTracks::GetNParticles`
  REQUIRE(fConstructed.GetNParticles() == 5);
  REQUIRE(fSetObject.GetNParticles() == 2);

  /// * Test `CandidateTracks::FailsMultiplicityCut`
  REQUIRE(fConstructed.FailsMultiplicityCut() == true);
  AddDummyTracks(fConstructed, 2);
  REQUIRE(fConstructed.FailsMultiplicityCut() == true);
  AddDummyTracks(fConstructed, 2);
  REQUIRE(fConstructed.FailsMultiplicityCut() == true);
  AddDummyTracks(fConstructed, 1);
  REQUIRE(fConstructed.FailsMultiplicityCut() == false);

  REQUIRE(fSetObject.FailsMultiplicityCut() == true);
  AddDummyTracks(fSetObject, 2);
  REQUIRE(fSetObject.FailsMultiplicityCut() == false);
  AddDummyTracks(fSetObject, 1);
  REQUIRE(fSetObject.FailsMultiplicityCut() == true);

  /// * Test `CandidateTracks::SetMultCut_AtMost`
  fConstructed.SetMultCut_AtMost();
  AddDummyTracks(fConstructed, 1);
  REQUIRE(fConstructed.FailsMultiplicityCut() == true);

  /// * Test `CandidateTracks::Clear`
  fSetObject.Clear();
  REQUIRE(fSetObject.FailsMultiplicityCut() == true);

  /// * Test size of `CandidateTracks::GetTracks`
  REQUIRE(fSetObject.GetTracks().size() == 0);
  AddDummyTracks(fSetObject, 2);
  REQUIRE(fSetObject.GetTracks().size() == 2);
}

void Test_CandidateTracks::TestExecute()
{
  fInputFile.LoadNextEvent();
  fKaons.Clear();
  /// * Test `CandidateTracks::AddTrack`
  ChargedTrackIter it(fInputFile);
  while(EvtRecTrack* trk = it.Next())
    fKaons.AddTrack(trk);
  REQUIRE(fKaons.GetTracks().size() < 7);
  /// * Test `CandidateTracks::GetTracks`
  if(fInputFile.EventNumber() != 50007) return;
  const std::vector<EvtRecTrack*>& vec = fKaons.GetTracks();
  int count = 0;
  for(std::vector<EvtRecTrack*>::const_iterator trk = vec.begin(); trk != vec.end(); ++trk)
  {
    REQUIRE((*trk)->trackId() == count);
    ++count;
  }
  REQUIRE(vec.size() == 6);
  /// * Test `CandidateTracks::NextCombination`
  fKaons.NextCombination();
  REQUIRE(vec[0]->trackId() == 0);
  REQUIRE(vec[1]->trackId() == 1);
  REQUIRE(vec[2]->trackId() == 3);
  REQUIRE(vec[3]->trackId() == 2);
  REQUIRE(vec[4]->trackId() == 4);
  REQUIRE(vec[5]->trackId() == 5);
  /// * Test `CandidateTracks::NextCombination`
  fKaons.NextCombination();
  REQUIRE(vec[0]->trackId() == 0);
  REQUIRE(vec[1]->trackId() == 1);
  REQUIRE(vec[2]->trackId() == 4);
  REQUIRE(vec[3]->trackId() == 2);
  REQUIRE(vec[4]->trackId() == 3);
  REQUIRE(vec[5]->trackId() == 5);
}

void Test_CandidateTracks::TestFinalize()
{}

void Test_CandidateTracks::AddDummyTracks(CandidateTracks<Event::McParticle>& obj, int n)
{
  for(int i = 0; i < n; ++i) obj.AddTrack(nullptr);
}