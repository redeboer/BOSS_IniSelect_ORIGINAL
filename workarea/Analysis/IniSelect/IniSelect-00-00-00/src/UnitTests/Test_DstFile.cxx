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
  REQUIRE(fInputFile.RunNumber() == -10870);
  REQUIRE(fInputFile.EventNumber() == fCountEvent);
  REQUIRE(fInputFile.IsMonteCarlo() == true);

  /// * Check iterators for all events
  if(true)
  {
    int              countCharged = 0;
    ChargedTrackIter iCharged(fInputFile);
    while(EvtRecTrack* track = iCharged.Next())
    {
      REQUIRE(track->trackId() == countCharged);
      ++countCharged;
      ++fNChargedTracks;
    }
    REQUIRE(fInputFile.TotalChargedTracks() == countCharged);

    int              countNeutral = 0;
    NeutralTrackIter iNeutral(fInputFile);
    while(EvtRecTrack* track = iNeutral.Next())
    {
      REQUIRE(track->trackId() - fInputFile.TotalChargedTracks() == countNeutral);
      ++countNeutral;
      ++fNNeutralTracks;
    }
    REQUIRE(fInputFile.TotalNeutralTracks() == countNeutral);

    int         countMC = 0;
    McTrackIter iMC(fInputFile);
    while(Event::McParticle* track = iMC.Next())
    {
      REQUIRE(track->trackIndex() == countMC);
      ++countMC;
      ++fNMCTracks;
    }
    REQUIRE(fInputFile.TotalMcTracks() == countMC);
  }

  /// * Track iterator test in case of event `50007`
  if(fCountEvent != 50007) return;
  REQUIRE(fInputFile.TotalChargedTracks() == 6);
  REQUIRE(fInputFile.TotalNeutralTracks() == 7);
  REQUIRE(fInputFile.TotalTracks() == 13);

  /// * Charged track iterator test
  ChargedTrackIter iCharged(fInputFile);
  while(EvtRecTrack* track = iCharged.Next())
  {
    switch(track->trackId())
    {
      case 1:
        REQUIRE(track->trackId() == 1);
        REQUIRE(track->partId() == -1);
        REQUIRE(track->quality() == -1);
        REQUIRE(track->isElectron() == 1);
        REQUIRE(track->isMuon() == 1);
        REQUIRE(track->isPion() == 1);
        REQUIRE(track->isKaon() == 1);
        REQUIRE(track->isProton() == 1);
        REQUIRE(track->isMdcTrackValid() == 1);
        REQUIRE(track->isMdcKalTrackValid() == 1);
        REQUIRE(track->isMdcDedxValid() == 1);
        REQUIRE(track->isTofTrackValid() == 1);
        REQUIRE(track->isEmcShowerValid() == 1);
        REQUIRE(track->isMucTrackValid() == 1);
        REQUIRE(track->isExtTrackValid() == 1);
        REQUIRE(track->mdcTrack() != nullptr);
        REQUIRE(track->mdcKalTrack() != nullptr);
        REQUIRE(track->mdcDedx() != nullptr);
        REQUIRE(track->extTrack() != nullptr);
        REQUIRE(track->emcShower() != nullptr);
        REQUIRE(track->mucTrack() != nullptr);
        break;
      case 4:
        REQUIRE(track->trackId() == 4);
        REQUIRE(track->partId() == -1);
        REQUIRE(track->quality() == -1);
        REQUIRE(track->isElectron() == 1);
        REQUIRE(track->isMuon() == 1);
        REQUIRE(track->isPion() == 1);
        REQUIRE(track->isKaon() == 1);
        REQUIRE(track->isProton() == 1);
        REQUIRE(track->isMdcTrackValid() == 1);
        REQUIRE(track->isMdcKalTrackValid() == 1);
        REQUIRE(track->isMdcDedxValid() == 1);
        REQUIRE(track->isTofTrackValid() == 1);
        REQUIRE(track->isEmcShowerValid() == 0);
        REQUIRE(track->isMucTrackValid() == 0);
        REQUIRE(track->isExtTrackValid() == 1);
        REQUIRE(track->mdcTrack() != nullptr);
        REQUIRE(track->mdcKalTrack() != nullptr);
        REQUIRE(track->mdcDedx() != nullptr);
        REQUIRE(track->extTrack() != nullptr);
        REQUIRE(track->emcShower() == nullptr);
        REQUIRE(track->mucTrack() == nullptr);
      default: break;
    }
  }

  /// * Neutral track iterator test
  NeutralTrackIter iNeutral(fInputFile);
  while(EvtRecTrack* track = iNeutral.Next())
  {
    if(track->trackId() == 6)
    {
      REQUIRE(track->trackId() == 6);
      REQUIRE(track->partId() == -1);
      REQUIRE(track->quality() == -1);
      REQUIRE(track->isElectron() == 1);
      REQUIRE(track->isMuon() == 1);
      REQUIRE(track->isPion() == 1);
      REQUIRE(track->isKaon() == 1);
      REQUIRE(track->isProton() == 1);
      REQUIRE(track->isMdcTrackValid() == 0);
      REQUIRE(track->isMdcKalTrackValid() == 0);
      REQUIRE(track->isMdcDedxValid() == 0);
      REQUIRE(track->isTofTrackValid() == 0);
      REQUIRE(track->isEmcShowerValid() == 1);
      REQUIRE(track->isMucTrackValid() == 0);
      REQUIRE(track->isExtTrackValid() == 0);
      REQUIRE(track->mdcTrack() == nullptr);
      REQUIRE(track->mdcKalTrack() == nullptr);
      REQUIRE(track->mdcDedx() == nullptr);
      REQUIRE(track->extTrack() == nullptr);
      REQUIRE(track->emcShower() != nullptr);
      REQUIRE(track->mucTrack() == nullptr);
    }
  }

  /// * MC track iterator test
  McTrackIter iMC(fInputFile);
  while(Event::McParticle* track = iMC.Next())
  {
    if(track->trackIndex() == 6)
    {
      REQUIRE(track->particleProperty() == 223); // omega
      REQUIRE(track->statusFlags() == 1);
      REQUIRE(track->primaryParticle() == 0);
      REQUIRE(track->leafParticle() == 0);
      REQUIRE(track->decayFromGenerator() == 1);
      REQUIRE(track->decayInFlight() == 0);
      REQUIRE(track->vertexIndex0() == 5);
      REQUIRE(track->vertexIndex1() == 6);
      REQUIRE(CompareFloat(track->initialPosition().x(), 0.170947));
      REQUIRE(CompareFloat(track->initialPosition().y(), -0.189134));
      REQUIRE(CompareFloat(track->initialPosition().z(), -0.865456));
      REQUIRE(CompareFloat(track->initialPosition().t(), 647.937));
      REQUIRE(CompareFloat(track->initialPosition().px(), 0.170947));
      REQUIRE(CompareFloat(track->initialPosition().py(), -0.189134));
      REQUIRE(CompareFloat(track->initialPosition().pz(), -0.865456));
      REQUIRE(CompareFloat(track->initialPosition().e(), 647.937));
      REQUIRE(CompareFloat(track->finalPosition().x(), 0.170947));
      REQUIRE(CompareFloat(track->finalPosition().y(), -0.189134));
      REQUIRE(CompareFloat(track->finalPosition().z(), -0.865456));
      REQUIRE(CompareFloat(track->finalPosition().t(), 647.937));
      REQUIRE(CompareFloat(track->finalPosition().px(), 0.170947));
      REQUIRE(CompareFloat(track->finalPosition().py(), -0.189134));
      REQUIRE(CompareFloat(track->finalPosition().pz(), -0.865456));
      REQUIRE(CompareFloat(track->finalPosition().e(), 647.937));
      REQUIRE(CompareFloat(track->initialFourMomentum().x(), -0.341812));
      REQUIRE(CompareFloat(track->initialFourMomentum().y(), 0.728731));
      REQUIRE(CompareFloat(track->initialFourMomentum().z(), 0.0386919));
      REQUIRE(CompareFloat(track->initialFourMomentum().t(), 1.12435));
      REQUIRE(CompareFloat(track->initialFourMomentum().px(), -0.341812));
      REQUIRE(CompareFloat(track->initialFourMomentum().py(), 0.728731));
      REQUIRE(CompareFloat(track->initialFourMomentum().pz(), 0.0386919));
      REQUIRE(CompareFloat(track->initialFourMomentum().e(), 1.12435));
      REQUIRE(track->mother().particleProperty() == 443);
    }
  }
}

void Test_DstFile::TestFinalize()
{
  REQUIRE(fNMCTracks == 154);
  REQUIRE(fNChargedTracks == 25);
  REQUIRE(fNNeutralTracks == 70);
}