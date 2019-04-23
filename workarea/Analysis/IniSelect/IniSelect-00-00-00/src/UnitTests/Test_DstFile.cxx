#include "IniSelect/UnitTests/Test_DstFile.h"
#include "IniSelect/Particle/ParticleDatabase.h"
#include <iostream>

Test_DstFile::Test_DstFile(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator),
  fInputFile(eventSvc()),
  fCountEvent(0),
  fNMCTracks(0),
  fNChargedTracks(0),
  fNNeutralTracks(0)
{}

void Test_DstFile::TestInitialize()
{
  fCountEvent = 50000;
}

void Test_DstFile::TestExecute()
{
  fInputFile.LoadNextEvent();
  ++fCountEvent;

  /// * Test run and event numbers
  TEST_CASE(fInputFile.RunNumber() == -10870);
  TEST_CASE(fInputFile.EventNumber() == fCountEvent);
  TEST_CASE(fInputFile.IsMonteCarlo() == true);

  /// * Check iterators for all events
  if(true)
  {
    int countCharged = 0;
    ChargedTrackIter iCharged(fInputFile);
    while(EvtRecTrack* track = iCharged.Next())
    {
      TEST_CASE(track->trackId() == countCharged);
      ++countCharged;
      ++fNChargedTracks;
    }
    TEST_CASE(fInputFile.TotalChargedTracks() == countCharged);

    int countNeutral = 0;
    NeutralTrackIter iNeutral(fInputFile);
    while(EvtRecTrack* track = iNeutral.Next())
    {
      TEST_CASE(track->trackId() - fInputFile.TotalChargedTracks() == countNeutral);
      ++countNeutral;
      ++fNNeutralTracks;
    }
    TEST_CASE(fInputFile.TotalNeutralTracks() == countNeutral);

    int countMC = 0;
    McTrackIter iMC(fInputFile);
    while(Event::McParticle* track = iMC.Next())
    {
      TEST_CASE(track->trackIndex() == countMC);
      ++countMC;
      ++fNMCTracks;
    }
    TEST_CASE(fInputFile.TotalMcTracks() == countMC);
  }

  /// * Track iterator test in case of event `50007`
  if(fCountEvent != 50007) return;
  TEST_CASE(fInputFile.TotalChargedTracks() == 6);
  TEST_CASE(fInputFile.TotalNeutralTracks() == 7);
  TEST_CASE(fInputFile.TotalTracks() == 13);

  /// * Charged track iterator test
  ChargedTrackIter iCharged(fInputFile);
  while(EvtRecTrack* track = iCharged.Next())
  {
    switch(track->trackId())
    {
      case 1:
        TEST_CASE(track->trackId() == 1);
        TEST_CASE(track->partId() == -1);
        TEST_CASE(track->quality() == -1);
        TEST_CASE(track->isElectron() == 1);
        TEST_CASE(track->isMuon() == 1);
        TEST_CASE(track->isPion() == 1);
        TEST_CASE(track->isKaon() == 1);
        TEST_CASE(track->isProton() == 1);
        TEST_CASE(track->isMdcTrackValid() == 1);
        TEST_CASE(track->isMdcKalTrackValid() == 1);
        TEST_CASE(track->isMdcDedxValid() == 1);
        TEST_CASE(track->isTofTrackValid() == 1);
        TEST_CASE(track->isEmcShowerValid() == 1);
        TEST_CASE(track->isMucTrackValid() == 1);
        TEST_CASE(track->isExtTrackValid() == 1);
        TEST_CASE(track->mdcTrack() != nullptr);
        TEST_CASE(track->mdcKalTrack() != nullptr);
        TEST_CASE(track->mdcDedx() != nullptr);
        TEST_CASE(track->extTrack() != nullptr);
        TEST_CASE(track->emcShower() != nullptr);
        TEST_CASE(track->mucTrack() != nullptr);
        break;
      case 4:
        TEST_CASE(track->trackId() == 4);
        TEST_CASE(track->partId() == -1);
        TEST_CASE(track->quality() == -1);
        TEST_CASE(track->isElectron() == 1);
        TEST_CASE(track->isMuon() == 1);
        TEST_CASE(track->isPion() == 1);
        TEST_CASE(track->isKaon() == 1);
        TEST_CASE(track->isProton() == 1);
        TEST_CASE(track->isMdcTrackValid() == 1);
        TEST_CASE(track->isMdcKalTrackValid() == 1);
        TEST_CASE(track->isMdcDedxValid() == 1);
        TEST_CASE(track->isTofTrackValid() == 1);
        TEST_CASE(track->isEmcShowerValid() == 0);
        TEST_CASE(track->isMucTrackValid() == 0);
        TEST_CASE(track->isExtTrackValid() == 1);
        TEST_CASE(track->mdcTrack() != nullptr);
        TEST_CASE(track->mdcKalTrack() != nullptr);
        TEST_CASE(track->mdcDedx() != nullptr);
        TEST_CASE(track->extTrack() != nullptr);
        TEST_CASE(track->emcShower() == nullptr);
        TEST_CASE(track->mucTrack() == nullptr);
      default: break;
    }
  }

  /// * Neutral track iterator test
  NeutralTrackIter iNeutral(fInputFile);
  while(EvtRecTrack* track = iNeutral.Next())
  {
    if(track->trackId() == 6)
    {
      TEST_CASE(track->trackId() == 6);
      TEST_CASE(track->partId() == -1);
      TEST_CASE(track->quality() == -1);
      TEST_CASE(track->isElectron() == 1);
      TEST_CASE(track->isMuon() == 1);
      TEST_CASE(track->isPion() == 1);
      TEST_CASE(track->isKaon() == 1);
      TEST_CASE(track->isProton() == 1);
      TEST_CASE(track->isMdcTrackValid() == 0);
      TEST_CASE(track->isMdcKalTrackValid() == 0);
      TEST_CASE(track->isMdcDedxValid() == 0);
      TEST_CASE(track->isTofTrackValid() == 0);
      TEST_CASE(track->isEmcShowerValid() == 1);
      TEST_CASE(track->isMucTrackValid() == 0);
      TEST_CASE(track->isExtTrackValid() == 0);
      TEST_CASE(track->mdcTrack() == nullptr);
      TEST_CASE(track->mdcKalTrack() == nullptr);
      TEST_CASE(track->mdcDedx() == nullptr);
      TEST_CASE(track->extTrack() == nullptr);
      TEST_CASE(track->emcShower() != nullptr);
      TEST_CASE(track->mucTrack() == nullptr);
    }
  }

  /// * MC track iterator test
  McTrackIter iMC(fInputFile);
  while(Event::McParticle* track = iMC.Next())
  {
    if(track->trackIndex() == 6)
    {
      TEST_CASE(track->particleProperty() == 223); // omega
      TEST_CASE(track->statusFlags() == 1);
      TEST_CASE(track->primaryParticle() == 0);
      TEST_CASE(track->leafParticle() == 0);
      TEST_CASE(track->decayFromGenerator() == 1);
      TEST_CASE(track->decayInFlight() == 0);
      TEST_CASE(track->vertexIndex0() == 5);
      TEST_CASE(track->vertexIndex1() == 6);
      TEST_CASE(CompareFloat(track->initialPosition().x(), 0.170947));
      TEST_CASE(CompareFloat(track->initialPosition().y(), -0.189134));
      TEST_CASE(CompareFloat(track->initialPosition().z(), -0.865456));
      TEST_CASE(CompareFloat(track->initialPosition().t(), 647.937));
      TEST_CASE(CompareFloat(track->initialPosition().px(), 0.170947));
      TEST_CASE(CompareFloat(track->initialPosition().py(), -0.189134));
      TEST_CASE(CompareFloat(track->initialPosition().pz(), -0.865456));
      TEST_CASE(CompareFloat(track->initialPosition().e(), 647.937));
      TEST_CASE(CompareFloat(track->finalPosition().x(), 0.170947));
      TEST_CASE(CompareFloat(track->finalPosition().y(), -0.189134));
      TEST_CASE(CompareFloat(track->finalPosition().z(), -0.865456));
      TEST_CASE(CompareFloat(track->finalPosition().t(), 647.937));
      TEST_CASE(CompareFloat(track->finalPosition().px(), 0.170947));
      TEST_CASE(CompareFloat(track->finalPosition().py(), -0.189134));
      TEST_CASE(CompareFloat(track->finalPosition().pz(), -0.865456));
      TEST_CASE(CompareFloat(track->finalPosition().e(), 647.937));
      TEST_CASE(CompareFloat(track->initialFourMomentum().x(), -0.341812));
      TEST_CASE(CompareFloat(track->initialFourMomentum().y(), 0.728731));
      TEST_CASE(CompareFloat(track->initialFourMomentum().z(), 0.0386919));
      TEST_CASE(CompareFloat(track->initialFourMomentum().t(), 1.12435));
      TEST_CASE(CompareFloat(track->initialFourMomentum().px(), -0.341812));
      TEST_CASE(CompareFloat(track->initialFourMomentum().py(), 0.728731));
      TEST_CASE(CompareFloat(track->initialFourMomentum().pz(), 0.0386919));
      TEST_CASE(CompareFloat(track->initialFourMomentum().e(), 1.12435));
      TEST_CASE(track->mother().particleProperty() == 443);
    }
  }
}

void Test_DstFile::TestFinalize()
{
  TEST_CASE(fNMCTracks == 154);
  TEST_CASE(fNChargedTracks == 25);
  TEST_CASE(fNNeutralTracks == 70);
}