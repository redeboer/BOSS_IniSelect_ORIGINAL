#ifndef Analysis_IniSelect_CandidateTracks_H
#define Analysis_IniSelect_CandidateTracks_H

#include "IniSelect/Globals/CombinationShuffler.h"
#include "IniSelect/Particle/ParticleDatabase.h"
#include "TString.h"
#include <vector>

/// @addtogroup BOSS_objects
/// @{

/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 16th, 2018
template <typename T>
class CandidateTracks
{
public:
  CandidateTracks() {}
  CandidateTracks(const TString& pdtName, Int_t nparticles = 1) :
    fParticle(ParticleDatabase::GetParticle(pdtName.Data())),
    fNParticles(nparticles)
  {}

  void Clear() { fTrackColl.clear(); }
  void AddTrack(T* track) { fTrackColl.push_back(track); }
  void SetNParticles(Int_t nparticles) { fNParticles = nparticles; }
  void SetParticle(const TString& pdtName)
  {
    fParticle = ParticleDatabase::GetParticle(pdtName.Data());
  }

  void SetMultCut_EqualTo() { fMultiplicityCut = EqualTo; }
  void SetMultCut_AtLeast() { fMultiplicityCut = AtLeast; };
  void SetMultCut_AtMost() { fMultiplicityCut = AtMost; };

  Bool_t FailsMultiplicityCut() const;

  const std::vector<T*>& GetTracks() const { return fTrackColl; }
  T* GetTracks(size_t i) const { if(i < fTrackColl.size()) return fTrackColl.at(i); else return nullptr; }

  size_t      GetNTracks() const { return fTrackColl.size(); }
  Int_t       GetNParticles() const { return fNParticles; }
  const char* GetPdtName() const { return fParticle.GetPdtName(); }
  Int_t       GetPdgCode() const { return fParticle.GetPdgCode(); }
  Float_t     GetMass() const { return fParticle.GetMass(); }
  T*          GetCandidate(size_t i = 0) const;

  Bool_t IsCharged() const { return fParticle.GetCharge(); }

  Bool_t NextCombination();

private:
  enum EMultiplicityCut
  {
    NoCut,
    EqualTo,
    AtLeast,
    AtMost
  };

  Particle fParticle;
  Int_t    fNParticles;

  std::vector<T*>  fTrackColl;
  EMultiplicityCut fMultiplicityCut;
};

/// @}

template <typename T>
T* CandidateTracks<T>::GetCandidate(size_t i) const
{
  if(i < fTrackColl.size()) return fTrackColl.at(i);
  return nullptr;
}

template <typename T>
Bool_t CandidateTracks<T>::NextCombination()
{
  return IniSelect::CombinationShuffler::NextCombination(fTrackColl, fNParticles);
}

template <typename T>
Bool_t CandidateTracks<T>::FailsMultiplicityCut() const
{
  switch(fMultiplicityCut)
  {
    case EqualTo: return fTrackColl.size() != fNParticles;
    case AtLeast: return fTrackColl.size() < fNParticles;
    case AtMost: return fTrackColl.size() > fNParticles;
    default: return false;
  }
}

#endif