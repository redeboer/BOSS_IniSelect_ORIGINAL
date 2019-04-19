#ifndef Analysis_TrackSelector_CandidateTracks_H
#define Analysis_TrackSelector_CandidateTracks_H

#include "TString.h"
#include "TrackSelector/Particle/Particle.h"
#include "TrackSelector/TSGlobals/CombinationShuffler.h"
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
  CandidateTracks(const TString& pdtName, const size_t nparticles = 1) :
    fParticle(pdtName),
    fNParticles(nparticles)
  {}

  void Clear() { fTrackColl.clear(); }
  void AddTrack(T* track) { fTrackColl.push_back(track); }
  void SetNParticles(const size_t nparticles) { fNParticles = nparticles; }
  void SetParticle(const TString& pdtName) { fParticle = Particle(pdtName); }

  void SetMultCut_EqualTo() { fMultiplicityCut = EqualTo; }
  void SetMultCut_AtLeast() { fMultiplicityCut = AtLeast; };
  void SetMultCut_AtMost() { fMultiplicityCut = AtMost; };

  bool FailsMultiplicityCut() const;

  const std::vector<T*>& GetTracks() const { return fTrackColl; }

  const size_t   GetNTracks() const { return fTrackColl.size(); }
  const size_t&  GetNParticles() const { return fNParticles; }
  const char*    GetPdtName() const { return fParticle.GetPdtName(); }
  const Int_t&   GetPdgCode() const { return fParticle.GetPdgCode(); }
  const Float_t& GetMass() const { return fParticle.GetMass(); }
  T*             GetParticle(const size_t i = 0) const;

  bool IsCharged() const { fParticle.GetCharge(); }

  bool NextCombination();

private:
  enum EMultiplicityCut
  {
    NoCut,
    EqualTo,
    AtLeast,
    AtMost
  };

  Particle fParticle;
  size_t   fNParticles;

  std::vector<T*>  fTrackColl;
  EMultiplicityCut fMultiplicityCut;
};

/// @}

template <typename T>
T* CandidateTracks<T>::GetParticle(const size_t i) const
{
  if(i < fTrackColl.size()) return fTrackColl.at(i);
  return nullptr;
}

template <typename T>
bool CandidateTracks<T>::NextCombination()
{
  return TSGlobals::CombinationShuffler::NextCombination(fTrackColl, fNParticles);
}

template <typename T>
bool CandidateTracks<T>::FailsMultiplicityCut() const
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