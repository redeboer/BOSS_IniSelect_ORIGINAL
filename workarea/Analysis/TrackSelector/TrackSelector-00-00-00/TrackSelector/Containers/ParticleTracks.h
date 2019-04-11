#ifndef Analysis_ParticleTracks_H
#define Analysis_ParticleTracks_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "EvtRecEvent/EvtRecTrack.h"
#include "McTruth/McParticle.h"
#include "TString.h"
#include "TrackSelector/Containers/TrackCollection.h"
#include <list>
#include <vector>

// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_objects
/// @{

/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date   April 11th, 2019
class ParticleTracks
{
public:
  ParticleTracks(const TString& particle, const TString& latex = "", const size_t nparticles = 1);
  ~ParticleTracks() { fInstances.remove(this); }

  void         ClearAll();
  const size_t GetNParticles() const { return fTrackCollection.GetNumberOfIterators(); }
  const char*  BuildNameForMultiplicityBranch() const { return Form("N%s", fParticleName.Data()); }

  static void Reset() { fIterator = fInstances.begin(); }
  static bool Next()
  {
    ++fIterator;
    return fIterator != fInstances.end();
  }
  static bool Iter_IsEnd() { return fIterator == fInstances.end(); }
  static void Get() { *fIterator; }
  static void Iter_Increase() { ++fIterator; }

  static bool HasEqualIterators()
  {
    std::list<const ParticleTracks*>::iterator it = fInstances.begin();
    for(; it != fInstances.end(); ++it)
      if((*it)->HasEqualIterators()) return true;
    return false;
  }

  const TrackCollection<Event::McParticle*>& GetCollection() { return fTrackCollection; }
  const TrackCollection<EvtRecTrack*>& GetCollectionMC() { return fTrackCollectionMC; }

  const TString& GetName() const { return fParticleName; }
  const TString& GetLaTeXName() const { return fParticleLaTeX; }

private:
  const TString fParticleName;
  const TString fParticleLaTeX;

  TrackCollection<Event::McParticle*> fTrackCollectionMC;
  TrackCollection<EvtRecTrack*>       fTrackCollection;

  static std::list<const ParticleTracks*>           fInstances;
  static std::list<const ParticleTracks*>::iterator fIterator;
};

std::list<const ParticleTracks*>           ParticleTracks::fInstances;
std::list<const ParticleTracks*>::iterator ParticleTracks::fIterator;

/// @}

ParticleTracks::ParticleTracks(const TString& particle, const TString& latex = "",
                               const size_t nparticles = 1) :
  fTrackCollection(nparticles),
  fTrackCollectionMC(nparticles)
{
  fInstances.insert(this);
}

#endif