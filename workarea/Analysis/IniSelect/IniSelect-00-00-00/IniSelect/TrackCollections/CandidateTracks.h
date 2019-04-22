#ifndef Analysis_IniSelect_CandidateTracks_H
#define Analysis_IniSelect_CandidateTracks_H

#include "IniSelect/Globals/CombinationShuffler.h"
#include "IniSelect/Particle/Particle.h"
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
  CandidateTracks(const TString& pdtName, const short nparticles = 1) :
    fParticle(pdtName),
    fNParticles(nparticles)
  {}

  void Clear() { fTrackColl.clear(); }
  void AddTrack(T* track) { 
std::cout << "hiero1!" << std::endl;
    fTrackColl.push_back(track);
std::cout << "hiero2!" << std::endl;
 }
  void SetNParticles(const short nparticles) { fNParticles = nparticles; }
  void SetParticle(const TString& pdtName) { fParticle = Particle(pdtName); }

  void SetMultCut_EqualTo() { fMultiplicityCut = EqualTo; }
  void SetMultCut_AtLeast() { fMultiplicityCut = AtLeast; };
  void SetMultCut_AtMost() { fMultiplicityCut = AtMost; };

  bool FailsMultiplicityCut() const;

  const std::vector<T*>& GetTracks() const { return fTrackColl; }

  size_t      GetNTracks() const { return fTrackColl.size(); }
  short       GetNParticles() const { return fNParticles; }
  const char* GetPdtName() const { return fParticle.GetPdtName(); }
  Int_t       GetPdgCode() const { return fParticle.GetPdgCode(); }
  Float_t     GetMass() const { return fParticle.GetMass(); }
  T*          GetCandidate(const short i = 0) const;

  bool IsCharged() const { return fParticle.GetCharge(); }

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
  short    fNParticles;

  std::vector<T*>  fTrackColl;
  EMultiplicityCut fMultiplicityCut;
};

/// @}

template <typename T>
T* CandidateTracks<T>::GetCandidate(const short i) const
{
  if(i < fTrackColl.size()) return fTrackColl.at(i);
  return nullptr;
}

template <typename T>
bool CandidateTracks<T>::NextCombination()
{
  return IniSelect::CombinationShuffler::NextCombination(fTrackColl, fNParticles);
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