#ifndef Analysis_TrackSelector_ParticleSelection_H
#define Analysis_TrackSelector_ParticleSelection_H

#include "McTruth/McEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "TrackSelector/TrackCollections/CandidateTracks.h"
#include <map>
#include <string>
#include <utility>

/// @addtogroup BOSS_objects
/// @{

/// @todo Consider converting `ParticleSelectionTempl` into "`ParticleSelectorTempl`", i.e., a singleton that only has static methods. This will, however, remove the option of creating separate sets of particle selections.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 18th, 2018
template <typename T>
class ParticleSelectionTempl
{
public:
  ParticleSelectionTempl() { fSelectionsIter = fSelections.begin(); }

  void SetN_Photons(size_t n) { SetParticleToN("g", n); }
  void SetN_PionMin(size_t n) { SetParticleToN("pi-", n); }
  void SetN_PionPlus(size_t n) { SetParticleToN("pi+", n); }
  void SetN_KaonMin(size_t n) { SetParticleToN("K-", n); }
  void SetN_KaonPlus(size_t n) { SetParticleToN("K+", n); }
  void SetN_Electrons(size_t n) { SetParticleToN("e-", n); }
  void SetN_Positrons(size_t n) { SetParticleToN("e+", n); }
  void SetN_Muons(size_t n) { SetParticleToN("mu-", n); }
  void SetN_AntiMuons(size_t n) { SetParticleToN("mu+", n); }
  void SetN_Protons(size_t n) { SetParticleToN("p+", n); }
  void SetN_AntiProtons(size_t n) { SetParticleToN("p-", n); }

  void ClearCharged();
  void ClearNeutral();

  bool NextPhotonCombination();

  CandidateTracks<T>& GetCandidates(const std::string& pdtName) { fSelections.at(pdtName); }
  CandidateTracks<T>& GetPhotons() { return GetCandidates("g"); };

  CandidateTracks<T>* ResetLooper();
  CandidateTracks<T>* Next();
  CandidateTracks<T>* NextCharged();

private:
  typename std::map<std::string, CandidateTracks<T> >           fSelections;
  typename std::map<std::string, CandidateTracks<T> >::iterator fSelectionsIter;
  void SetParticleToN(const TString& pdtName, const size_t& nInstances);

  CandidateTracks<T>* UnpackIter();
};

typedef ParticleSelectionTempl<EvtRecTrack> ParticleSelection;
typedef ParticleSelectionTempl<Event::McParticle> ParticleSelectionMC;

/// @}

template <typename T>
CandidateTracks<T>* ParticleSelectionTempl<T>::ResetLooper()
{
  fSelectionsIter = fSelections.begin();
  return UnpackIter();
}

template <typename T>
CandidateTracks<T>* ParticleSelectionTempl<T>::Next()
{
  ++fSelectionsIter;
  return UnpackIter();
}

template <typename T>
CandidateTracks<T>* ParticleSelectionTempl<T>::NextCharged()
{
  CandidateTracks<T>* coll;
  while(coll = Next())
  {
    if(!coll) return nullptr;
    if(coll->IsCharged()) return coll;
  }
}

template <typename T>
CandidateTracks<T>* ParticleSelectionTempl<T>::UnpackIter()
{
  if(fSelectionsIter == fSelections.end()) return nullptr;
  return &fSelectionsIter->second;
}

template <typename T>
void ParticleSelectionTempl<T>::SetParticleToN(const TString& pdtName, const size_t& nInstances)
{
  fSelections[pdtName.Data()].SetParticle(pdtName);
  fSelections[pdtName.Data()].SetNParticles(nInstances);
}

template <typename T>
void ParticleSelectionTempl<T>::ClearCharged()
{
  typename std::map<std::string, CandidateTracks<T> >::iterator it;
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
    if(it->second.IsCharged()) it->second.Clear();
}

template <typename T>
void ParticleSelectionTempl<T>::ClearNeutral()
{
  typename std::map<std::string, CandidateTracks<T> >::iterator it;
  for(it = fSelections.begin(); it != fSelections.end(); ++it)
    if(!it->second.IsCharged()) it->second.Clear();
}

template <typename T>
bool ParticleSelectionTempl<T>::NextPhotonCombination()
{
  return fSelections.at("g").NextCombination();
}

#endif