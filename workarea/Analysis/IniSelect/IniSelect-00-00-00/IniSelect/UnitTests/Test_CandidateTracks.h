#ifndef Analysis_IniSelect_UnitTests_CandidateTracks_H
#define Analysis_IniSelect_UnitTests_CandidateTracks_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/UnitTests/UnitTester.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/Containers/DstFile.h"
#include "IniSelect/TrackCollections/CandidateTracks.h"

class Test_CandidateTracks : public UnitTester
{
public:
  Test_CandidateTracks(const std::string& name, ISvcLocator* pSvcLocator);

  void TestInitialize();
  void TestExecute();
  void TestFinalize();

private:
  DstFile fInputFile;

  CandidateTracks<Event::McParticle> fCandidateTracks_g;   // this one is set using methods
  CandidateTracks<Event::McParticle> fCandidateTracks_pip; // this one is set through the constructor
};

#endif