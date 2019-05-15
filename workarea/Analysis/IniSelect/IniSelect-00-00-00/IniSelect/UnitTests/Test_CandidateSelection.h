#ifndef Analysis_IniSelect_UnitTests_CandidateSelection_H
#define Analysis_IniSelect_UnitTests_CandidateSelection_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/TrackCollections/CandidateSelection.h"
#include "IniSelect/UnitTests/UnitTester.h"

class Test_CandidateSelection : public UnitTester
{
public:
  Test_CandidateSelection(const std::string& name, ISvcLocator* pSvcLocator);

  void TestInitialize() {}
  void TestExecute() {}
  void TestEvent();
  void TestFinalize() {}

private:
  CandidateSelection   fSelection;
  CandidateSelectionMC fSelectionMC;
};

#endif