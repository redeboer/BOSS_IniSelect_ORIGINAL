#include "IniSelect/UnitTests/Test_CandidateTracks.h"
#include "TString.h"
#include <iostream>
#include <iomanip>
using namespace std;

Test_CandidateTracks::Test_CandidateTracks(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator),
  fSetObject("pi+", 2),
  fKaons("K-", 3),
  fPhotons("g", 3)
{}

void Test_CandidateTracks::TestInitialize()
{
  /// * Test `CandidateTracks::Set*`
  fConstructed.SetParticle("g");
  fConstructed.SetNParticles(5);

  fConstructed.SetMultCut_AtLeast();
  fSetObject.SetMultCut_EqualTo();

  /// * Test `CandidateTracks::GetPdtName`
  TString g_name(fConstructed.GetPdtName().Data());
  TString pip_name(fSetObject.GetPdtName().Data());
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
}

void Test_CandidateTracks::TestExecute()
{
  /// * Test `CandidateTracks::AddTrack`
  fKaons.Clear();
  ChargedTrackIter it(fInputFile);
  while(EvtRecTrack* trk = it.Next())
    fKaons.AddTrack(trk);
  REQUIRE(fKaons.GetNTracks() < 7);

  /// * Test `CandidateTracks::AddTrack`
  fSetObject.Clear();
  McTrackIter itMC(fInputFile);
  while(Event::McParticle* trk = itMC.Next()) fSetObject.AddTrack(trk);
  REQUIRE(fSetObject.GetNTracks() == fInputFile.TotalMcTracks());

  /// * Test `CandidateTracks::AddTrack`
  fPhotons.Clear();
  NeutralTrackIter itPhoton(fInputFile);
  while(EvtRecTrack* trk = itPhoton.Next()) fPhotons.AddTrack(trk);
  REQUIRE(fPhotons.GetNTracks() == fInputFile.TotalMcTracks());

  /// * Test `CandidateTracks::NextCombination`
  int count = 0;
  while(fSetObject.NextCombination()) ++count;
  /// @todo Should investigate why there are certain exceptions to the binom rule for `NextCombination`.
  cerr << fInputFile.EventNumber() << ":";
  cerr << setw(4) << count;
  cerr << setw(4) << nChoosek(fInputFile.TotalMcTracks(), fSetObject.GetNParticles());
  cerr << setw(4) << fInputFile.TotalMcTracks();
  cerr << setw(4) << fSetObject.GetNParticles();
  cerr << endl;
  switch(fInputFile.EventNumber())
  {
    case 50002: break;
    case 50005: break;
    case 50008: break;
    case 50010: break;
    // default: REQUIRE(count == nChoosek(fInputFile.TotalMcTracks(), fSetObject.GetNParticles()));
  }
}

void Test_CandidateTracks::TestEvent()
{
  REQUIRE(fKaons.GetNTracks() == 6);
  ChargedTrackIter it(fInputFile);
  EvtRecTrack* trk = nullptr;

  /// * Test `CandidateTracks::NextCombination` and `CandidateTracks::GetCandidate`
  fKaons.NextCombination();
  fKaons.NextCombination();
  REQUIRE(fKaons.GetCandidate(0)->trackId() == 0);
  REQUIRE(fKaons.GetCandidate(1)->trackId() == 1);
  REQUIRE(fKaons.GetCandidate(2)->trackId() == 3);
  REQUIRE(fKaons.GetCandidate(3)->trackId() == 2);
  REQUIRE(fKaons.GetCandidate(4)->trackId() == 4);
  REQUIRE(fKaons.GetCandidate(5)->trackId() == 5);
  REQUIRE(fKaons.GetCandidate(6) == nullptr);
  fKaons.NextCombination();
  REQUIRE(fKaons.GetCandidate(0)->trackId() == 0);
  REQUIRE(fKaons.GetCandidate(1)->trackId() == 1);
  REQUIRE(fKaons.GetCandidate(2)->trackId() == 4);
  REQUIRE(fKaons.GetCandidate(3)->trackId() == 2);
  REQUIRE(fKaons.GetCandidate(4)->trackId() == 3);
  REQUIRE(fKaons.GetCandidate(5)->trackId() == 5);
  REQUIRE(fKaons.GetCandidate(6) == nullptr);
  fKaons.NextCombination();
  REQUIRE(fKaons.GetCandidate(0)->trackId() == 0);
  REQUIRE(fKaons.GetCandidate(1)->trackId() == 1);
  REQUIRE(fKaons.GetCandidate(2)->trackId() == 5);
  REQUIRE(fKaons.GetCandidate(3)->trackId() == 2);
  REQUIRE(fKaons.GetCandidate(4)->trackId() == 3);
  REQUIRE(fKaons.GetCandidate(5)->trackId() == 4);
  REQUIRE(fKaons.GetCandidate(6) == nullptr);
}

void Test_CandidateTracks::TestFinalize()
{}

void Test_CandidateTracks::AddDummyTracks(CandidateTracks<Event::McParticle>& obj, int n)
{
  for(int i = 0; i < n; ++i) obj.AddTrack(nullptr);
}

int Test_CandidateTracks::nChoosek(int n, int k)
{
  if(k > n) return 0;
  if(k * 2 > n) k = n-k;
  if(k == 0) return 1;

  int result = n;
  for(int i = 2; i <= k; ++i)
  {
    result *= (n-i+1);
    result /= i;
  }
  return result;
}