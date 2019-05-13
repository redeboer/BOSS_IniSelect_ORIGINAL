#include "IniSelect/Handlers/FinalStateHandler.h"
#include "IniSelect/Globals.h"
using namespace std;
using namespace IniSelect::Physics;

ParticleTrack& FinalStateHandler::GetParticle(const string& name, size_t i)
{
  return fCollection.at(name).at(i);
}

void FinalStateHandler::SetFinalState(const TString& finalState)
{
  fFinalState.Set(finalState);
  for(size_t i = 0; i < BESIIIparticles.size(); ++i)
  {
    const TString& pdtName = BESIIIparticles[i];
    Int_t n = fFinalState.CountOccurences(pdtName);
    fParticleSel.SetParticleToN(pdtName, n);
    fParticleSelMC.SetParticleToN(pdtName, n);
    AddParticle(pdtName.Data(), n);
  }
}

void FinalStateHandler::AddParticle(const std::string& pdtName, size_t n)
{
  vector<ParticleTrack> vec(n);
  for(size_t i = 0; i < n; ++i) vec[i] = ParticleTrack(pdtName);
  fCollection[pdtName] = vec;
}