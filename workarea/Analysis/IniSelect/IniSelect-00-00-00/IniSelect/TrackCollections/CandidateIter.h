#ifndef Analysis_IniSelect_CandidateIter_H
#define Analysis_IniSelect_CandidateIter_H

#include "IniSelect/TrackCollections/CandidateSelection.h"

/// @addtogroup BOSS_objects
/// @{

/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 14th, 2018
class CandidateIter_base
{
public:
  CandidateIter_base(CandidateSelection& candidates);
  virtual CandidateTracks<EvtRecTrack>* Next() = 0;

protected:
  CandidateSelection*                                            fSelection;
  std::map<std::string, CandidateTracks<EvtRecTrack> >::iterator fIter;

  CandidateTracks<EvtRecTrack>* UnpackIter();
};

class ChargedCandidateIter : public CandidateIter_base
{
public:
  ChargedCandidateIter(CandidateSelection&);
  CandidateTracks<EvtRecTrack>* Next();
};

class CandidateIter : public CandidateIter_base
{
public:
  CandidateIter(CandidateSelection&);
  CandidateTracks<EvtRecTrack>* Next();
};
/// @}

CandidateIter_base::CandidateIter_base(CandidateSelection& candidates) : fSelection(&candidates)
{
  fIter = fSelection->fSelections.begin();
}

CandidateTracks<EvtRecTrack>* CandidateIter::Next()
{
  ++fIter;
  return UnpackIter();
}

CandidateTracks<EvtRecTrack>* ChargedCandidateIter::Next()
{
  while(fIter != fSelection->fSelections.end())
  {
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

#endif