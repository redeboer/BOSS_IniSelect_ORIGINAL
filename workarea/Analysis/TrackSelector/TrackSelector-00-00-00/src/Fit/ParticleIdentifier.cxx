#include "TrackSelector/Fit/ParticleIdentifier.h"
#include "TString.h"
#include "TrackSelector/TSGlobals/TSException.h"
using namespace TSGlobals::Error;

ParticleIdentifier::ParticleIdentifier()
{
  if(!gPID) throw Exception("ParticleID singleton has not been initialized");
};

ParticleID* ParticleIdentifier::gPID = ParticleID::instance();

void ParticleIdentifier::IdentifyParticle(const int pdgCode)
{
  switch(pdgCode)
  {
    case -11:
    case 11: IdentifyElectron(); return;
    case -13:
    case 13: IdentifyMuon(); return;
    case -211:
    case 211: IdentifyPion(); return;
    case -321:
    case 321: IdentifyKaon(); return;
    case -2212:
    case 2212: IdentifyProton(); return;
    default: throw Exception(Form("PID problem: case for %d has not been defined", pdgCode));
  }
}

double ParticleIdentifier::GetProbByPDGCode(const int pdgCode)
{
  switch(pdgCode)
  {
    case -11:
    case 11: return gPID->probElectron();
    case -13:
    case 13: return gPID->probMuon();
    case -211:
    case 211: return gPID->probPion();
    case -321:
    case 321: return gPID->probKaon();
    case -2212:
    case 2212: return gPID->probProton();
    default: throw Exception(Form("Cannot get PID prob for %d has not been defined", pdgCode));
  }
}

bool ParticleIdentifier::AttemptIdentification()
{
  gPID->calculate();
  return gPID->IsPidInfoValid();
}