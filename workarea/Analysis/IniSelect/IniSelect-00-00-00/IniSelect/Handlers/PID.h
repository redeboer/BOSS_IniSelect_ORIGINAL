#ifndef Analysis_IniSelect_PID_H
#define Analysis_IniSelect_PID_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/Containers/CutObject.h"
#include "IniSelect/Exceptions/Exception.h"
#include "ParticleID/ParticleID.h"
#include <string>
#include <vector>

/// Static access to the [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) class in BOSS.
/// @addtogroup BOSS_objects
/// @{
class PID
{
public:
  static void Initialize() { gPID->init(); }
  // static void Reset(); //! to be implemented: Initialize plus get back settings

  static void UseLikelihoodMethod() { gPID->setMethod(gPID->methodLikelihood()); }
  static void UseProbabilityMethod() { gPID->setMethod(gPID->methodProbability()); }
  static void UseNeuronNetwork() { gPID->setMethod(gPID->methodNeuronNetwork()); }

  static void UseDedx() { gPID->usePidSys(gPID->useDedx()); }
  static void UseTofIB() { gPID->usePidSys(gPID->useTof1()); }
  static void UseTofOB() { gPID->usePidSys(gPID->useTof2()); }
  static void UseTofE() { gPID->usePidSys(gPID->useTofE()); }

  static void SetMinimalChi2(Double_t minchi) { gPID->setChiMinCut(minchi); }

  static void SetIdentifyElectron();
  static void SetIdentifyMuon();
  static void SetIdentifyPion();
  static void SetIdentifyKaon();
  static void SetIdentifyProton();
  static void SetParticleToIdentify(Int_t pdgCode);

  static Int_t FindMostProbable(EvtRecTrack* trk);

  static Double_t GetChiTofE() { return gPID->chiTofE(2); }
  static Double_t GetChiTofIB() { return gPID->chiTof1(2); }
  static Double_t GetChiTofOB() { return gPID->chiTof2(2); }
  static Double_t GetChiDedx() { return gPID->chiDedx(2); }

  static Double_t GetProbElectron() { return gPID->probElectron(); }
  static Double_t GetProbMuon() { return gPID->probMuon(); }
  static Double_t GetProbPion() { return gPID->probPion(); }
  static Double_t GetProbKaon() { return gPID->probKaon(); }
  static Double_t GetProbProton() { return gPID->probProton(); }

private:
  PID();
  PID(const PID&) {}
  PID& operator=(const PID&) {}

  static ParticleID* gPID;
  static CutObject   fCut_PIDprob;
  static Int_t       fPIDbit;

  static void  SetProbabilities(EvtRecTrack* trk);
  static Int_t FindMostProbable();

  static const char* ConvertIndexToName(Int_t index);
  static Int_t  ConvertIndexToPdg(Int_t index, EvtRecTrack* trk);
  static Bool_t IsIdentified(Int_t index);
  static void   SetMdcKalPidType(Int_t index);
};

struct UnknownPidIndex : public Exception
{
  UnknownPidIndex(Int_t i) : Exception("PID index %d is undefined", i) {}
};

struct NotPidInfoValid : public Exception
{
  NotPidInfoValid() : Exception("Track is not PID info valid") {}
};

/// @}
#endif
