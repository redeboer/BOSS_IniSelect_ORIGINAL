#ifndef Analysis_IniSelect_FinalStateHandler_H
#define Analysis_IniSelect_FinalStateHandler_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/Containers/FinalState.h"
#include "IniSelect/Containers/ParticleTrack.h"
#include "IniSelect/TrackCollections/ParticleSelection.h"
#include "TString.h"
#include "VertexFit/VertexFit.h"
#include <map>
#include <string>
#include <vector>

/// @addtogroup BOSS_objects
/// @{
class FinalStateHandler
{
public:
  FinalStateHandler() {}
  void                 SetFinalState(const TString& input);
  ParticleTrack&       GetParticle(const std::string& pdtName, size_t i = 0);
  ParticleSelection&   GetParticleSelection() { return fParticleSel; }
  ParticleSelectionMC& GetParticleSelectionMC() { return fParticleSelMC; }

private:
  std::map<std::string, std::vector<ParticleTrack> > fCollection;

  FinalState          fFinalState;
  ParticleSelection   fParticleSel;
  ParticleSelectionMC fParticleSelMC;

  void  AddParticle(const std::string& pdtName, size_t n = 1);
  Int_t CountOccurences(const TString& input, const TString& particle_name);
};
/// @}
#endif