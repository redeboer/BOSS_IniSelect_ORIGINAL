#include "TrackSelector/Fit/ParticleIdentifier.h"
#include "TString.h"
#include "TrackSelector/TSGlobals/TSException.h"
using namespace TSGlobals::Error;

#include <stdio.h>

ParticleID* ParticleIdentifier::gPID = ParticleID::instance();
int         ParticleIdentifier::fBestIndex;
std::string ParticleIdentifier::fBestName;
int         ParticleIdentifier::fParticleToIdentify = 0;

ParticleIdentifier::ParticleIdentifier()
{
  if(!gPID) throw Exception("ParticleID singleton has not been initialized");
};

void ParticleIdentifier::SetParticleToIdentify(int pdcCode)
{
  switch(pdcCode)
  {
    case -11:
    case 11: fParticleToIdentify |= gPID->onlyElectron(); break;
    case -13:
    case 13: fParticleToIdentify |= gPID->onlyMuon(); break;
    case -211:
    case 211: fParticleToIdentify |= gPID->onlyPion(); break;
    case -321:
    case 321: fParticleToIdentify |= gPID->onlyKaon(); break;
    case -2212:
    case 2212: fParticleToIdentify |= gPID->onlyProton(); break;
    default: throw Exception(Form("No PID case defined for PDG code %d", pdcCode));
  }
  gPID->identify(fParticleToIdentify);
}

std::string ParticleIdentifier::FindMostProbable(EvtRecTrack* trk, CutObject& probCut)
{
  if(!SetProbabilities(trk)) return "";
  SetMostProbable();
  if(FailsProbPidCut(probCut)) return "";
  AppendChargeSign(trk);
  return fBestName;
}

bool ParticleIdentifier::SetProbabilities(EvtRecTrack* trk)
{
  gPID->setRecTrack(trk);
  gPID->calculate();
  return gPID->IsPidInfoValid();
}

void ParticleIdentifier::SetMostProbable()
{
  fBestIndex = -1;
  double bestProb{0.};
  for(int i = 0; i < 5; ++i)
  {
    if(IsIdentified(i) && gPID->prob(i) > bestProb) {
      bestProb = gPID->prob(i);
      fBestIndex = i;
    }
  }
for(int i = 0; i < 5; ++i) printf("  %8.6f", gPID->prob(i)); printf("\n");
for(int i = 0; i < 5; ++i) if(IsIdentified(i)) printf("  %8.6f", gPID->prob(i)); else printf("  %8.6f", 0.); printf("\n");
printf("--> %3d\n", fBestIndex);
  fBestName = ConvertIndexToName(fBestIndex);
}

bool ParticleIdentifier::FailsProbPidCut(CutObject& probCut)
{
  return probCut.FailsMin(gPID->prob(fBestIndex));
}

void ParticleIdentifier::AppendChargeSign(EvtRecTrack* trk)
{
  SetPidType(fBestIndex);
  if(trk->mdcKalTrack()->charge() > 0)
    fBestName += "+";
  else
    fBestName += "-";
}

const char* ParticleIdentifier::ConvertIndexToName(int index)
{
  /// Mapping from the array index used in [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) to a particle name.
  /// See [here](http://bes3.to.infn.it/Boss/7.0.2/html/ParticleID_8h-source.html#l00121) for an example of what this mapping should be.
  switch(index)
  {
    case -1: return "";
    case 0: return "e";
    case 1: return "mu";
    case 2: return "pi";
    case 3: return "K";
    case 4: return "p";
    default: throw Exception(Form("No name case defined for index %d", index));
  }
}

bool ParticleIdentifier::IsIdentified(int index)
{
  /// Mapping from the array index used in [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) to a particle name.
  /// See [here](http://bes3.to.infn.it/Boss/7.0.2/html/ParticleID_8h-source.html#l00121) for an example of what this mapping should be.
  switch(index)
  {
    case 0: return fParticleToIdentify & gPID->onlyElectron();
    case 1: return fParticleToIdentify & gPID->onlyMuon();
    case 2: return fParticleToIdentify & gPID->onlyPion();
    case 3: return fParticleToIdentify & gPID->onlyKaon();
    case 4: return fParticleToIdentify & gPID->onlyProton();
    default: throw Exception(Form("No name case defined for index %d", index));
  }
}

void ParticleIdentifier::SetPidType(int index)
{
  switch(index)
  {
    case 0: RecMdcKalTrack::setPidType(RecMdcKalTrack::electron); return;
    case 1: RecMdcKalTrack::setPidType(RecMdcKalTrack::muon); return;
    case 2: RecMdcKalTrack::setPidType(RecMdcKalTrack::pion); return;
    case 3: RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon); return;
    case 4: RecMdcKalTrack::setPidType(RecMdcKalTrack::proton); return;
    default: throw Exception(Form("No name case defined for index %d", index));
  }
}