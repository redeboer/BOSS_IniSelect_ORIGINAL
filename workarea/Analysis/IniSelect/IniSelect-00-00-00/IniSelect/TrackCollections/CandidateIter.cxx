#include "IniSelect/TrackCollections/CandidateIter.h"

CandidateIter_base::CandidateIter_base(CandidateSelection& candidates) : fSelection(&candidates), fIsFirst(true)
{
  fIter = fSelection->fSelections.begin();
}

CandidateTracks<EvtRecTrack>* CandidateIter::Next()
{
  if(fIsFirst) fIsFirst = false;
  else ++fIter;
  return UnpackIter();
}

CandidateTracks<EvtRecTrack>* ChargedCandidateIter::Next()
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

CandidateTracks<EvtRecTrack>* CandidateIter_base::UnpackIter()
{
  if(fIter == fSelection->fSelections.end()) return nullptr;
  return &fIter->second;
}