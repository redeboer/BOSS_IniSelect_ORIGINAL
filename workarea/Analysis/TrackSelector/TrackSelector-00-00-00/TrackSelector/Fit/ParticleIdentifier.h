#ifndef Analysis_TrackSelector_ParticleIdentifier_H
#define Analysis_TrackSelector_ParticleIdentifier_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "ParticleID/ParticleID.h"
#include "TrackSelector/Containers/CutObject.h"
#include <string>

/// Static access to the [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) class in BOSS.
/// @addtogroup BOSS_objects
/// @{
class ParticleIdentifier
{
public:
  static void Initialize() { gPID->init(); }

  static void UseLikelihoodMethod() { gPID->setMethod(gPID->methodLikelihood()); }
  static void UseProbabilityMethod() { gPID->setMethod(gPID->methodProbability()); }
  static void UseNeuronNetwork() { gPID->setMethod(gPID->methodNeuronNetwork()); }

  static void UseDedx() { gPID->usePidSys(gPID->useDedx()); }
  static void UseTofIB() { gPID->usePidSys(gPID->useTof1()); }
  static void UseTofOB() { gPID->usePidSys(gPID->useTof2()); }
  static void UseTofE() { gPID->usePidSys(gPID->useTofE()); }

  static void SetMinimalChi2(double minchi) { gPID->setChiMinCut(minchi); }
  static void SetParticleToIdentify(int pdgCode);

  static std::string FindMostProbable(EvtRecTrack* trk, CutObject& pidCut);

  static double GetChiTofE() { return gPID->chiTofE(2); }
  static double GetChiTofIB() { return gPID->chiTof1(2); }
  static double GetChiTofOB() { return gPID->chiTof2(2); }
  static double GetChiDedx() { return gPID->chiDedx(2); }

  static double GetProbKaon() { return gPID->probKaon(); }
  static double GetProbElectron() { return gPID->probElectron(); }
  static double GetProbMuon() { return gPID->probMuon(); }
  static double GetProbProton() { return gPID->probProton(); }
  static double GetProbPion() { return gPID->probPion(); }

private:
  ParticleIdentifier();
  ParticleIdentifier(const ParticleIdentifier&){};
  ParticleIdentifier& operator=(const ParticleIdentifier&){};

  static ParticleID* gPID;
  static int         fBestIndex;
  static std::string fBestName;
  static int         fParticleToIdentify;

  static bool SetProbabilities(EvtRecTrack* trk);
  static void SetMostProbable();
  static bool FailsProbPidCut(CutObject& pidCut);
  static void AppendChargeSign(EvtRecTrack* trk);

  static std::string ConvertIndexToName(int index);
  static bool        IsIdentified(int index);
  static void        SetPidType(int index);
};
/// @}
#endif
