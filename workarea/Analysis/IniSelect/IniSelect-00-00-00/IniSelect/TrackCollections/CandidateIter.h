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
  bool fIsFirst;
};

class CandidateIter : public CandidateIter_base
{
public:
  CandidateIter(CandidateSelection& candidates) : CandidateIter_base(candidates) {}
  CandidateTracks<EvtRecTrack>* Next();
};

class ChargedCandidateIter : public CandidateIter_base
{
public:
  ChargedCandidateIter(CandidateSelection& candidates) : CandidateIter_base(candidates) {}
  CandidateTracks<EvtRecTrack>* Next();
};

/// @}
#endif