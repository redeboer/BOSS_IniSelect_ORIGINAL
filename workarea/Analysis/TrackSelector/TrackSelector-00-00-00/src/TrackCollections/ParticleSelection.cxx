#include "TrackSelector/TrackCollections/ParticleSelection.h"

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

void ParticleSelection::ClearCharged()
{
  std::map<std::string, TrackCollection<EvtRecTrack> >::iterator it;
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
    if(it->second.IsCharged()) it->second.Clear();
}

void ParticleSelection::ClearNeutral()
{
  std::map<std::string, TrackCollection<EvtRecTrack> >::iterator it;
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
    if(!it->second.IsCharged()) it->second.Clear();
}

bool ParticleSelection::NextPhotonCombination()
{
  return fSelections["g"].NextCombination();
}
