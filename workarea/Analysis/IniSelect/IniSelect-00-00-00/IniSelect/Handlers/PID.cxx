#include "IniSelect/Handlers/PID.h"
using namespace std;

ParticleID* PID::gPID = ParticleID::instance();

Int_t PID::fPIDbit = 0;

CutObject PID::fCut_PIDprob("PIDProb");

PID::PID ()
{
  if(!gPID) throw Exception("ParticleID singleton has not been initialized");
};

void PID::SetIdentifyElectron()
{
  fPIDbit |= gPID->onlyElectron();
  gPID->identify(fPIDbit);
}

void PID::SetIdentifyMuon()
{
  fPIDbit |= gPID->onlyMuon();
  gPID->identify(fPIDbit);
}

void PID::SetIdentifyPion()
{
  fPIDbit |= gPID->onlyPion();
  gPID->identify(fPIDbit);
}

void PID::SetIdentifyKaon()
{
  fPIDbit |= gPID->onlyKaon();
  gPID->identify(fPIDbit);
}

void PID::SetIdentifyProton()
{
  fPIDbit |= gPID->onlyProton();
  gPID->identify(fPIDbit);
}

void PID::SetParticleToIdentify(Int_t pdcCode)
{
  switch(pdcCode)
  {
    case -11:
    case 11: SetIdentifyElectron(); break;
    case -13:
    case 13: SetIdentifyMuon(); break;
    case -211:
    case 211: SetIdentifyPion(); break;
    case -321:
    case 321: SetIdentifyKaon(); break;
    case -2212:
    case 2212: SetIdentifyProton(); break;
    default: throw Exception("No PID case defined for PDG code %d", pdcCode);
  }
}

Int_t PID::FindMostProbable(EvtRecTrack* trk)
{
  SetProbabilities(trk);
  Int_t bestIndex = -1;
  Double_t bestProb = 0.;
  for(Int_t i = 0; i < 5; ++i)
    if(IsIdentified(i) && gPID->prob(i) > bestProb)
    {
      bestProb   = gPID->prob(i);
      bestIndex = i;
    }
  if(fCut_PIDprob.FailsMin(gPID->prob(bestIndex))) throw Exception("Probability cut failed");
  auto pdgCandidate = ConvertIndexToPdg(bestIndex, trk);
  return pdgCandidate;
}

void PID::SetProbabilities(EvtRecTrack* trk)
{
  gPID->setRecTrack(trk);
  gPID->calculate();
  if(!gPID->IsPidInfoValid()) throw NotPidInfoValid();
}

void PID::SetMdcKalPidType(Int_t index)
{
  switch(index)
  {
    case 0: RecMdcKalTrack::setPidType(RecMdcKalTrack::electron);
    case 1: RecMdcKalTrack::setPidType(RecMdcKalTrack::muon);
    case 2: RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
    case 3: RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);
    case 4: RecMdcKalTrack::setPidType(RecMdcKalTrack::proton);
    default: throw Exception("No name case defined for index %d", index);
  }
}

const char* PID::ConvertIndexToName(Int_t index)
{
  /// Mapping from the array index used in [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) to a particle name.
  /// See [here](http://bes3.to.infn.it/Boss/7.0.2/html/ParticleID_8h-source.html#l00121) for an example of what this mapping should be.
  switch(index)
  {
    case 0: return "e";
    case 1: return "mu";
    case 2: return "pi";
    case 3: return "K";
    case 4: return "p";
    default: throw UnknownPidIndex(index);
  }
}

Int_t PID::ConvertIndexToPdg(Int_t index, EvtRecTrack* trk)
{
  /// Mapping from the array index used in [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) to a particle PDG code.
  /// See [here](http://bes3.to.infn.it/Boss/7.0.2/html/ParticleID_8h-source.html#l00121) for an example of what this mapping should be.
  Int_t pdgCode = 0;
  switch(index)
  {
    case 0: pdgCode = -11;
    case 1: pdgCode = -13;
    case 2: pdgCode = 211;
    case 3: pdgCode = 321;
    case 4: pdgCode = 2212;
    default: throw UnknownPidIndex(index);
  }
  SetMdcKalPidType(index);
  if(trk->mdcKalTrack()->charge() < 0) pdgCode *= -1;
  return pdgCode;
}

Bool_t PID::IsIdentified(Int_t index)
{
  /// Mapping from the array index used in [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) to a particle name.
  /// See [here](http://bes3.to.infn.it/Boss/7.0.2/html/ParticleID_8h-source.html#l00121) for an example of what this mapping should be.
  switch(index)
  {
    case 0: return fPIDbit & gPID->onlyElectron();
    case 1: return fPIDbit & gPID->onlyMuon();
    case 2: return fPIDbit & gPID->onlyPion();
    case 3: return fPIDbit & gPID->onlyKaon();
    case 4: return fPIDbit & gPID->onlyProton();
    default: throw UnknownPidIndex(index);
  }
}