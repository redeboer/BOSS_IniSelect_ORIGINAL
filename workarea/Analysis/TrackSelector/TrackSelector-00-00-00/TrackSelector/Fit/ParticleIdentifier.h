#ifndef Analysis_TrackSelector_ParticleIdentifier_H
#define Analysis_TrackSelector_ParticleIdentifier_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "ParticleID/ParticleID.h"

/// Static access to the [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) class in BOSS.
/// @addtogroup BOSS_objects
/// @{
class ParticleIdentifier
{
public:
  static void Reset() { gPID->init(); }

  static void UseLikelihoodMethod() { gPID->setMethod(gPID->methodLikelihood()); }
  static void UseProbabilityMethod() { gPID->setMethod(gPID->methodProbability()); }
  static void UseNeuronNetwork() { gPID->setMethod(gPID->methodNeuronNetwork()); }

  static void UseDedx() { gPID->usePidSys(gPID->useDedx()); }
  static void UseTofIB() { gPID->usePidSys(gPID->useTof1()); }
  static void UseTofOB() { gPID->usePidSys(gPID->useTof2()); }
  static void UseTofE() { gPID->usePidSys(gPID->useTofE()); }

  static void SetMinimalChi2(double minchi) { gPID->setChiMinCut(minchi); }

  static void IdentifyElectron() { gPID->identify(gPID->onlyElectron()); }
  static void IdentifyMuon() { gPID->identify(gPID->onlyMuon()); }
  static void IdentifyPion() { gPID->identify(gPID->onlyPion()); }
  static void IdentifyKaon() { gPID->identify(gPID->onlyKaon()); }
  static void IdentifyProton() { gPID->identify(gPID->onlyProton()); }
  static void IdentifyParticle(const int pdgCode);

  static void SetTrack(EvtRecTrack* trk) { gPID->setRecTrack(trk); }

  static bool Calculate();

  static double GetChiTofE() { gPID->chiTofE(2); }
  static double GetChiTofIB() { gPID->chiTof1(2); }
  static double GetChiTofOB() { gPID->chiTof2(2); }
  static double GetChiDedx() { gPID->chiDedx(2); }

  static double GetProbKaon() { gPID->probKaon(); }
  static double GetProbElectron() { gPID->probElectron(); }
  static double GetProbMuon() { gPID->probMuon(); }
  static double GetProbProton() { gPID->probProton(); }
  static double GetProbPion() { gPID->probPion(); }
  static double GetProbByPDGCode(const int pdgCode);

private:
  ParticleIdentifier();
  ParticleIdentifier(const ParticleIdentifier&){};
  ParticleIdentifier& operator=(const ParticleIdentifier&){};

  static ParticleID* gPID;
};
/// @}
#endif