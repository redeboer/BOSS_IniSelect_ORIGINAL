#ifndef Analysis_TrackCollection_H
#define Analysis_TrackCollection_H

#include "TrackSelector/TSGlobals/CombinationShuffler.h"
#include "TString.h"
#include <vector>

/// @addtogroup BOSS_objects
/// @{

/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 16th, 2018
template <typename T>
class TrackCollection
{
public:
  TrackCollection(const TString& name, const size_t nparticles = 1) :
    fParticleName(name), fNParticles(nparticles)
  {
  }

  void Clear() { fTrackColl.clear(); }
  void AddTrack(T* track) { fTrackColl.push_back(track); }

  bool FailsNumberOfParticles() const { return fTrackColl.size() != fNParticles; }
  bool FailsMinimumNumberOfParticles() const { return fTrackColl.size() < fNParticles; }
  bool FailsMaximumNumberOfParticles() const { return fTrackColl.size() > fNParticles; }

  const std::vector<T*>& GetCollection() const { return fTrackColl; }

  const size_t GetNTracks() const { return fTrackColl.size(); }
  const char*  GetName() const { return fParticleName.Data(); }
  T*           GetParticle(const size_t i = 0) const;

  bool NextCombination();

private:
  const TString   fParticleName;
  const size_t    fNParticles;
  std::vector<T*> fTrackColl;
};

/// @}

template <typename T>
T* TrackCollection<T>::GetParticle(const size_t i) const
{
  if(i < fTrackColl.size()) return fTrackColl.at(i);
  return nullptr;
}

template <typename T>
bool TrackCollection<T>::NextCombination()
{
  return TSGlobals::CombinationShuffler::NextCombination(fTrackColl, fNParticles);
}

#endif