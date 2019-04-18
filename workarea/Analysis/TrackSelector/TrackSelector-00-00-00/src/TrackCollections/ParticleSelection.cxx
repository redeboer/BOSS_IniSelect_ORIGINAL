#include "TrackSelector/TrackCollections/ParticleSelection.h"

ParticleSelection::ParticleSelection()
{
  fSelectionsIter = fSelections.begin();
}

void ParticleSelection::SetN_Photons(size_t n)
{
  SetParticleToN("g", n);
}
void ParticleSelection::SetN_PionMin(size_t n)
{
  SetParticleToN("pi-", n);
}
void ParticleSelection::SetN_PionPlus(size_t n)
{
  SetParticleToN("pi+", n);
}
void ParticleSelection::SetN_KaonPlus(size_t n)
{
  SetParticleToN("K+", n);
}
void ParticleSelection::SetN_KaonMin(size_t n)
{
  SetParticleToN("K-", n);
}
void ParticleSelection::SetN_Electrons(size_t n)
{
  SetParticleToN("e-", n);
}
void ParticleSelection::SetN_Positrons(size_t n)
{
  SetParticleToN("e+", n);
}
void ParticleSelection::SetN_Muons(size_t n)
{
  SetParticleToN("mu-+", n);
}
void ParticleSelection::SetN_AntiMuons(size_t n)
{
  SetParticleToN("mu+", n);
}
void ParticleSelection::SetN_Protons(size_t n)
{
  SetParticleToN("p+", n);
}
void ParticleSelection::SetN_AntiProtons(size_t n)
{
  SetParticleToN("p-", n);
}

TrackCollection<EvtRecTrack>* ParticleSelection::ResetLooper()
{
  fSelectionsIter = fSelections.begin();
  return UnpackIter();
}

TrackCollection<EvtRecTrack>* ParticleSelection::Next()
{
  ++fSelectionsIter;
  return UnpackIter();
}

TrackCollection<EvtRecTrack>* ParticleSelection::NextCharged()
{
  TrackCollection<EvtRecTrack>* coll;
  while(coll = Next())
  {
    if(!coll) return nullptr;
    if(coll->IsCharged()) return coll;
  }
}

TrackCollection<EvtRecTrack>* ParticleSelection::UnpackIter()
{
  if(fSelectionsIter == fSelections.end()) return nullptr;
  return &fSelectionsIter->second;
}

void ParticleSelection::SetParticleToN(const TString& pdtName, const size_t& nInstances)
{
  fSelections[pdtName.Data()].SetNParticles(nInstances);
}

const TrackCollection<EvtRecTrack>& ParticleSelection::GetCollection(const std::string& pdtName)
{
  return fSelections[pdtName];
}

void ParticleSelection::ClearCharged()
{
  std::map<std::string, TrackCollection<EvtRecTrack> >::iterator it;
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
    if(it->second.IsCharged()) it->second.Clear();
}

void ParticleSelection::ClearCharged()
{
  std::map<std::string, TrackCollection<EvtRecTrack> >::iterator it;
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
    if(!it->second.IsCharged()) it->second.Clear();
}

bool ParticleSelection::NextPhotonCombination()
{
  return fSelections["g"].NextCombination();
}
