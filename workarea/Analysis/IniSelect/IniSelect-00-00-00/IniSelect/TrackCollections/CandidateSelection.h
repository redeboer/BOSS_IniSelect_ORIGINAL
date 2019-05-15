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

/// @todo Consider converting `CandidateSelectionTempl` into "`CandidateSelectorTempl`", i.e., a singleton that only has static methods. This will, however, remove the option of creating separate sets of particle selections.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 18th, 2018
class CandidateIter_base;
class CandidateIter_baseMC;
class CandidateIter;
class CandidateIterMC;
class ChargedCandidateIter;
class ChargedCandidateIterMC;

template <typename T>
class CandidateSelectionTempl
{
  friend class CandidateIter_base;
  friend class CandidateIter_baseMC;
  friend class CandidateIter;
  friend class CandidateIterMC;
  friend class ChargedCandidateIter;
  friend class ChargedCandidateIterMC;

public:
  CandidateSelectionTempl() : fNCharged(0) {}

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

typedef CandidateSelectionTempl<EvtRecTrack>         CandidateSelection;
typedef CandidateSelectionTempl<Event::McParticle>   CandidateSelectionMC;
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
  typename std::map<std::string, CandidateTracks<T> >::iterator it;
  std::cout << "Selected particles: ";
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
  {
    std::cout << it->GetNParticles() << " " << it->GetPdtName();
    if(it != fSelections.end()) std::cout << ", ";
  }
  std::cout << std::endl;
}

#endif