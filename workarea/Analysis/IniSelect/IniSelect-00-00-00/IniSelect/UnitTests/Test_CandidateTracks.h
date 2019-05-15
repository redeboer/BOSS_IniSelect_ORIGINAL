#ifndef Analysis_IniSelect_UnitTests_CandidateTracks_H
#define Analysis_IniSelect_UnitTests_CandidateTracks_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/TrackCollections/CandidateTracks.h"
#include "IniSelect/UnitTests/UnitTester.h"

class Test_CandidateTracks : public UnitTester
{
public:
  Test_CandidateTracks(const std::string& name, ISvcLocator* pSvcLocator);

  void TestInitialize();
  void TestExecute();
  void TestEvent();
  void TestFinalize();

private:
  CandidateTracks<EvtRecTrack> fKaons;
  CandidateTracks<Event::McParticle> fConstructed;
  CandidateTracks<Event::McParticle> fSetObject;

  void AddDummyTracks(CandidateTracks<Event::McParticle>& obj, int n);
  int nChoosek(int n, int k);
};

#endif