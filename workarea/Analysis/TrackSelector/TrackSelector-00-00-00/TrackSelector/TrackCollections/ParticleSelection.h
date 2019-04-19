#ifndef Analysis_TrackSelector_ParticleSelection_H
#define Analysis_TrackSelector_ParticleSelection_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "TrackSelector/TrackCollections/CandidateTracks.h"
#include <map>
#include <string>

/// @addtogroup BOSS_objects
/// @{

/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 18th, 2018
class ParticleSelection
{
public:
  ParticleSelection() { fSelectionsIter = fSelections.begin(); }

  void SetN_Photons(size_t n) { SetParticleToN("g", n); }
  void SetN_PionMin(size_t n) { SetParticleToN("pi-", n); }
  void SetN_PionPlus(size_t n) { SetParticleToN("pi+", n); }
  void SetN_KaonPlus(size_t n) { SetParticleToN("K+", n); }
  void SetN_KaonMin(size_t n) { SetParticleToN("K-", n); }
  void SetN_Electrons(size_t n) { SetParticleToN("e-", n); }
  void SetN_Positrons(size_t n) { SetParticleToN("e+", n); }
  void SetN_Muons(size_t n) { SetParticleToN("mu-+", n); }
  void SetN_AntiMuons(size_t n) { SetParticleToN("mu+", n); }
  void SetN_Protons(size_t n) { SetParticleToN("p+", n); }
  void SetN_AntiProtons(size_t n) { SetParticleToN("p-", n); }

  void ClearCharged();
  void ClearNeutral();

  bool NextPhotonCombination();

  CandidateTracks<EvtRecTrack>& GetCandidates(const std::string& pdtName) { fSelections[pdtName]; }
  CandidateTracks<EvtRecTrack>& GetPhotons() { return GetCandidates("g"); };

  CandidateTracks<EvtRecTrack>* ResetLooper();
  CandidateTracks<EvtRecTrack>* Next();
  CandidateTracks<EvtRecTrack>* NextCharged();


private:
  std::map<std::string, CandidateTracks<EvtRecTrack> >           fSelections;
  std::map<std::string, CandidateTracks<EvtRecTrack> >::iterator fSelectionsIter;
  void SetParticleToN(const TString& pdtName, const size_t& nInstances);

  CandidateTracks<EvtRecTrack>* UnpackIter();
};

/// @}
#endif