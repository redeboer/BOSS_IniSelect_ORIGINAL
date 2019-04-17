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
    fName(name),
    fIterators(nparticles)
  {}

  void Clear() { fCollection.clear(); }
  void AddTrack(T* track) { fCollection.push_back(track); }
  bool HasCorrectNTracks() const { return fCollection.size() == fIterators.size(); }
  bool HasAtLeastNTracks() const { return fCollection.size() >= fIterators.size(); }
  bool HasIdenticalIters();

  const std::vector<T*> GetCollection() const { return fCollection; }

  const size_t GetNTracks() const { return fCollection.size(); }
  const size_t GetNParticles() const { return fIterators.size(); }
  const char*  GetName() { return fName.Data(); }

  T* UnpackIter(const size_t i = 0);
  T* Next(const size_t i = 0);
  bool Next();

  void ResetIterators();
  void LineUpIterators();

  enum OutOfRange
  {
    NumberOfIterators
  };

private:
  TString fName;

  std::vector<T*> fCollection;

  std::vector<typename std::vector<T*>::iterator> fIterators;

  typename std::vector<T*>::iterator& Iter(const size_t& i);
};

/// @}

template <typename T>
T* TrackCollection<T>::UnpackIter(const size_t i)
{
  return *Iter(i);
}

template <typename T>
typename std::vector<T*>::iterator& TrackCollection<T>::Iter(const size_t& i)
{
  if(i >= fIterators.size()) throw NumberOfIterators;
  return fIterators.at(i);
}

template <typename T>
T* TrackCollection<T>::Next(const size_t i)
{
  if(Iter(i) == fCollection.end()) return nullptr;
  ++Iter(i);
}

template <typename T>
void TrackCollection<T>::ResetIterators()
{
  typename std::vector<typename std::vector<T*>::iterator>::iterator it;
  for(it = fIterators.begin(); it != fIterators.end(); ++it) *it = fCollection.begin();
  void ResetIterators();
}

template <typename T>
void TrackCollection<T>::LineUpIterators()
{
  Iter(0) = fCollection.begin();
  for(size_t i = 1; i < fIterators.size(); ++i) Iter(i) = Iter(i - 1) + 1;
}

template <typename T>
bool TrackCollection<T>::HasIdenticalIters()
{
  for(size_t i = 0; i < fIterators.size(); ++i)
    for(size_t j = i + 1; j < fIterators.size(); ++j)
      if(Iter(i) == Iter(j)) return true;
  return false;
}

template <typename T>
bool TrackCollection<T>::Next()
{
  return TSGlobals::CombinationShuffler::NextCombination(fCombinations, fIterators.size());
}

#endif