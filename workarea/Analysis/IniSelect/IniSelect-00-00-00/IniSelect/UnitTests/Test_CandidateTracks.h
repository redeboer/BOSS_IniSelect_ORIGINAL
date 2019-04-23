#ifndef Analysis_IniSelect_UnitTests_CandidateTracks_H
#define Analysis_IniSelect_UnitTests_CandidateTracks_H

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

  CandidateTracks fCandidateTracks_g;   // this one is set using methods
  CandidateTracks fCandidateTracks_pip; // this one is set through the constructor
};

#endif