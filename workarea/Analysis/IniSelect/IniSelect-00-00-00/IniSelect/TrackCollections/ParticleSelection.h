#ifndef Analysis_IniSelect_ParticleSelection_H
#define Analysis_IniSelect_ParticleSelection_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/TrackCollections/CandidateTracks.h"
#include "McTruth/McParticle.h"
#include <map>
#include <string>
#include "TString.h"
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
  ParticleSelectionTempl() : fNCharged(0) { fSelectionsIter = fSelections.begin(); }

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
  void SetN_AntiProtons(size_t n) { SetParticleToN("anti-p-", n); }
  void SetFinalState(const TString& input);

  void ClearCharged();
  void ClearNeutral();

  bool NextPhotonCombination();

  bool HasParticle(const std::string& name) const;
  bool AddTrackToParticle(T* trk, const std::string& name);

  CandidateTracks<T>& GetCandidates(const std::string& pdtName) { fSelections.at(pdtName); }
  CandidateTracks<T>& GetPhotons() { return GetCandidates("g"); };
  int                 GetNCharged() { return fNCharged; }

  CandidateTracks<T>* FirstParticle();
  CandidateTracks<T>* NextParticle();
  CandidateTracks<T>* NextCharged();
  bool                ReachedEnd() { return fSelectionsIter == fSelections.end(); }

private:
  typename std::map<std::string, CandidateTracks<T> >           fSelections;
  typename std::map<std::string, CandidateTracks<T> >::iterator fSelectionsIter;
  void SetParticleToN(const TString& pdtName, const size_t& nInstances);

  int fNCharged;

  CandidateTracks<T>* UnpackIter();
  Int_t CountOccurences(const TString& input, const TString& particle_name);
};

typedef ParticleSelectionTempl<EvtRecTrack>       ParticleSelection;
typedef ParticleSelectionTempl<Event::McParticle> ParticleSelectionMC;

/// @}

template <typename T>
CandidateTracks<T>* ParticleSelectionTempl<T>::FirstParticle()
{
  fSelectionsIter = fSelections.begin();
  return UnpackIter();
}

template <typename T>
CandidateTracks<T>* ParticleSelectionTempl<T>::NextParticle()
{
  ++fSelectionsIter;
  return UnpackIter();
}

template <typename T>
CandidateTracks<T>* ParticleSelectionTempl<T>::NextCharged()
{
  while(NextParticle())
  {
    if(!UnpackIter()) return nullptr;
    if(UnpackIter()->IsCharged()) return UnpackIter();
  }
  return nullptr;
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
  CandidateTracks<T>& coll = fSelections[pdtName.Data()];
  coll.SetParticle(pdtName);
  coll.SetNParticles(nInstances);
  if(coll.IsCharged()) fNCharged += nInstances;
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

template <typename T>
bool ParticleSelectionTempl<T>::HasParticle(const std::string& name) const
{
  return fSelections.find(name) == fSelections.end();
}

template <typename T>
bool ParticleSelectionTempl<T>::AddTrackToParticle(T* track, const std::string& name)
{
  if(HasParticle(name)) false;
  fSelections.at(name).AddTrack(track);
  return true;
}

template <typename T>
void ParticleSelectionTempl<T>::SetFinalState(const TString& input)
{
  SetParticleToN("g", CountOccurences(input, "g"));
  SetParticleToN("pi-", CountOccurences(input, "pi-"));
  SetParticleToN("pi+", CountOccurences(input, "pi+"));
  SetParticleToN("K-", CountOccurences(input, "K-"));
  SetParticleToN("K+", CountOccurences(input, "K+"));
  SetParticleToN("e-", CountOccurences(input, "e-"));
  SetParticleToN("e+", CountOccurences(input, "e+"));
  SetParticleToN("mu-", CountOccurences(input, "mu-"));
  SetParticleToN("mu+", CountOccurences(input, "mu+"));
  SetParticleToN("p+", CountOccurences(input, "p+"));
  SetParticleToN("anti-p-", CountOccurences(input, "anti-p-"));
}

template <typename T>
Int_t ParticleSelectionTempl<T>::CountOccurences(const TString& input, const TString& particle_name)
{
  TString tok;
  Ssiz_t from = 0;
  Int_t n = 0;
  while(input.Tokenize(tok, from)) if(tok.EqualTo(particle_name)) ++n;
  return n;
}

#endif