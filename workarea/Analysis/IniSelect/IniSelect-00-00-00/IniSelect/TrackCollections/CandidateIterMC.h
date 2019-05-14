#ifndef Analysis_IniSelect_CandidateIterMC_H
#define Analysis_IniSelect_CandidateIterMC_H

#include "IniSelect/TrackCollections/CandidateSelection.h"

/// @addtogroup BOSS_objects
/// @{

/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 14th, 2018
class CandidateIter_baseMC
{
public:
  CandidateIter_baseMC(CandidateSelectionMC& candidates);
  virtual CandidateTracks<Event::McParticle>* Next() = 0;

protected:
  CandidateSelectionMC*                                            fSelection;
  std::map<std::string, CandidateTracks<Event::McParticle> >::iterator fIter;

  CandidateTracks<Event::McParticle>* UnpackIter();
};

class CandidateIterMC : public CandidateIter_baseMC
{
public:
  CandidateIterMC(CandidateSelectionMC&);
  CandidateTracks<Event::McParticle>* Next();
};

class ChargedCandidateIterMC : public CandidateIter_baseMC
{
public:
  ChargedCandidateIterMC(CandidateSelectionMC&);
  CandidateTracks<Event::McParticle>* Next();
};
/// @}

CandidateIter_baseMC::CandidateIter_baseMC(CandidateSelectionMC& candidates) : fSelection(&candidates)
{
  fIter = fSelection->fSelections.begin();
}

CandidateTracks<Event::McParticle>* CandidateIterMC::Next()
{
  ++fIter;
  return UnpackIter();
}

CandidateTracks<Event::McParticle>* ChargedCandidateIterMC::Next()
{
  while(fIter != fSelection->fSelections.end())
  {
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

#endif