#ifndef Analysis_TrackCollection_H
#define Analysis_TrackCollection_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "TString.h"
#include <list>
#include <vector>

// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_objects
/// @{

/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date   April 11th, 2019
template <typename T>
class TrackCollection
{
public:
  TrackCollection(const size_t nparticles = 1);

  void ClearCollection() { fTrackCollection.clear(); }

  void         SetNumberOfIterators(size_t nparticles);
  const size_t GetNumberOfIterators() const { return fIterators.size(); }

  std::vector<T*>::iterator& GetIterator() { return fIterators.front(); }
  std::vector<T*>::iterator& GetIterator(const size_t i) { return fIterators.at(i); }

  void ResetIterator() { fIterators.front() = fTrackCollection.begin(); }
  void ResetIterator(const size_t i) { fIterators[i] = fTrackCollection.begin(); }
  void ResetAllIterators();

  const bool HasEqualIterators() const;

private:
  std::vector<T*>                        fTrackCollection;
  std::vector<std::vector<T*>::iterator> fIterators;
};

/// @}

template <typename T>
TrackCollection<T>::TrackCollection(const size_t nparticles = 1) : fIterators(nparticles)
{
  SetNumberOfIterators(nparticles);
}

template <typename T>
void TrackCollection<T>::SetNumberOfIterators(size_t nparticles)
{
  fIterators.resize(nparticles);
  ResetAllIterators();
}

template <typename T>
void TrackCollection<T>::ResetAllIterators()
{
  std::list<std::vector<T*>::iterator>::iterator it = fIterators.begin();
  for(; it != fIterators.end(); ++it) (*it) = fTrackCollection.begin();
}

template <typename T>
const bool TrackCollection<T>::HasEqualIterators() const
{
  std::list<std::vector<T*>::iterator>::iterator i, j;
  for(i = fIterators.begin(); i != fIterators.end(); ++i)
    for(j = i + 1; j != fIterators.end(); ++j)
      if(*i == *j) return true;
  return false;
}

#endif