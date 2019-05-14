#ifndef Analysis_IniSelect_CandidateSelection_H
#define Analysis_IniSelect_CandidateSelection_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/TrackCollections/CandidateTracks.h"
#include "McTruth/McParticle.h"
#include "TString.h"
#include <map>
#include <string>
#include <utility>

/// @addtogroup BOSS_objects
/// @{

/// @todo Consider converting `CandidateSelectionTempl` into "`ParticleSelectorTempl`", i.e., a singleton that only has static methods. This will, however, remove the option of creating separate sets of particle selections.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 18th, 2018
template <typename T>
class ParticleIter_base;
template <typename T>
class ChargedCandidateIterTempl;
template <typename T>
class CandidateIterTempl;

template <typename T>
class CandidateSelectionTempl
{
  friend class CandidateIter_base<T>;
  friend class ChargedCandidateIterTempl<T>;
  friend class CandidateIterTempl<T>;

public:
  CandidateSelectionTempl() : fNCharged(0) { fSelectionsIter = fSelections.begin(); }

  void SetCandidateToN(const TString& pdtName, Int_t nInstances);

  void ClearCharged();
  void ClearNeutral();

  Bool_t NextPhotonCombination();

  Bool_t HasCandidate(const std::string& name) const;
  Bool_t AddCandidate(T* trk, const std::string& name);

  CandidateTracks<T>& GetCandidates(const std::string& pdtName) { return fSelections.at(pdtName); }
  CandidateTracks<T>& GetPhotons() { return GetCandidates("g"); };
  Int_t               GetNCharged() { return fNCharged; }

  void Print();

private:
  typename std::map<std::string, CandidateTracks<T> > fSelections;

  Int_t fNCharged;
  Int_t CountOccurences(const TString& input, const TString& particle_name);
};

template <typename T>
class CandidateIter_base
{
public:
  CandidateIter_base(CandidateSelectionTempl<T>& candidates);
  virtual T* Next() = 0;

protected:
  CandidateSelectionTempl<T>*                          fSelection;
  std::map<std::string, CandidateTracks<T> >::iterator fIter;

  CandidateTracks<T>* UnpackIter();
};

template <typename T>
class ChargedCandidateIterTempl : public CandidateTracks<T>
{
public:
  ChargedCandidateIterTempl(DstFile& file);
  CandidateTracks<T>* Next();
};

template <typename T>
class CandidateIterTempl : public CandidateTracks<EvtRecTrack>
{
public:
  CandidateIterTempl(DstFile& file);
  CandidateTracks<T>* Next();
};

typedef CandidateSelectionTempl<EvtRecTrack>         CandidateSelection;
typedef CandidateSelectionTempl<Event::McParticle>   CandidateSelectionMC;
typedef ChargedCandidateIterTempl<EvtRecTrack>       ChargedCandidateIter;
typedef ChargedCandidateIterTempl<Event::McParticle> ChargedCandidateIterMC;
typedef CandidateIterTempl<EvtRecTrack>              CandidateIter;
typedef CandidateIterTempl<Event::McParticle>        CandidateIterMC;
/// @}

template <typename T>
void CandidateSelectionTempl<T>::SetCandidateToN(const TString& pdtName, Int_t nInstances)
{
  if(!nInstances) return;
  CandidateTracks<T>& coll = fSelections[pdtName.Data()];
  coll.SetParticle(pdtName);
  coll.SetNParticles(nInstances);
  if(coll.IsCharged()) fNCharged += nInstances;
}

template <typename T>
void CandidateSelectionTempl<T>::ClearCharged()
{
  typename std::map<std::string, CandidateTracks<T> >::iterator it;
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
    if(it->second.IsCharged()) it->second.Clear();
}

template <typename T>
void CandidateSelectionTempl<T>::ClearNeutral()
{
  typename std::map<std::string, CandidateTracks<T> >::iterator it;
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
    if(!it->second.IsCharged()) it->second.Clear();
}

template <typename T>
Bool_t CandidateSelectionTempl<T>::NextPhotonCombination()
{
  return fSelections.at("g").NextCombination();
}

template <typename T>
Bool_t CandidateSelectionTempl<T>::HasCandidate(const std::string& name) const
{
  return fSelections.find(name) != fSelections.end();
}

template <typename T>
Bool_t CandidateSelectionTempl<T>::AddCandidate(T* track, const std::string& name)
{
  if(!HasCandidate(name)) false;
  fSelections.at(name).AddTrack(track);
  return true;
}

template <typename T>
Int_t CandidateSelectionTempl<T>::CountOccurences(const TString& input,
                                                  const TString& particle_name)
{
  TString tok;
  Ssiz_t  from = 0;
  Int_t   n    = 0;
  while(input.Tokenize(tok, from))
    if(tok.EqualTo(particle_name)) ++n;
  return n;
}

template <typename T>
void CandidateSelectionTempl<T>::Print()
{
  CandidateTracks<T>* coll = FirstCandidate();
  if(!coll) return;
  std::cout << "Selected particles: ";
  while(coll)
  {
    std::cout << coll->GetNParticles() << " " << coll->GetPdtName();
    coll = NextParticle();
    if(coll) std::cout << ", ";
  }
  std::cout << std::endl;
}

template <typename T>
CandidateIter_base<T>::CandidateIter_base(CandidateSelectionTempl<T>& candidates) :
  fSelection(&candidates)
{
  fIter = fSelection->begin();
}

template <typename T>
CandidateTracks<T>* CandidateIterTempl<T>::Next()
{
  ++fSelectionsIter;
  return UnpackIter();
}

template <typename T>
CandidateTracks<T>* ChargedCandidateIterTempl<T>::Next()
{
  while(NextCandidate())
  {
    if(!UnpackIter()) return nullptr;
    if(UnpackIter()->IsCharged()) return UnpackIter();
  }
  return nullptr;
}

template <typename T>
CandidateTracks<T>* CandidateIter_base<T>::UnpackIter()
{
  if(fSelectionsIter == fSelections.end()) return nullptr;
  return &fSelectionsIter->second;
}

#endif