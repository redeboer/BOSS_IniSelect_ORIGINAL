#include "IniSelect/TrackCollections/CandidateIterMC.h"

CandidateIter_baseMC::CandidateIter_baseMC(CandidateSelectionMC& candidates) : fSelection(&candidates)
{
  fIter = fSelection->fSelections.begin();
}

CandidateTracks<Event::McParticle>* CandidateIterMC::Next()
{
  if(fIsFirst) fIsFirst = false;
  else ++fIter;
  return UnpackIter();
}

CandidateTracks<Event::McParticle>* ChargedCandidateIterMC::Next()
{
  while(fIter != fSelection->fSelections.end())
  {
    if(fIsFirst) fIsFirst = false;
    else ++fIter;
    if(!UnpackIter()) return nullptr;
    if(UnpackIter()->IsCharged()) return UnpackIter();
  }
  return nullptr;
}

CandidateTracks<Event::McParticle>* CandidateIter_baseMC::UnpackIter()
{
  if(fIter == fSelection->fSelections.end()) return nullptr;
  return &fIter->second;
}