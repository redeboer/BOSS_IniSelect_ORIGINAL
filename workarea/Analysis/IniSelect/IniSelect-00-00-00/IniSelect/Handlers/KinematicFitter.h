#ifndef Analysis_IniSelect_KinematicFitter_H
#define Analysis_IniSelect_KinematicFitter_H

#include "EmcRecEventModel/RecEmcShower.h"
#include "IniSelect/Globals/Exception.h"
#include "IniSelect/TrackCollections/ParticleSelection.h"
#include "TString.h"
#include "VertexFit/KalmanKinematicFit.h"

/// @todo This class functions more as a `namespce`, but has to be a class as it wraps the `KalmanKinematicFit` singleton.
/// @addtogroup BOSS_objects
/// @{
class KinematicFitter
{
public:
  static void Initialize();
  static void AddTrack(RecEmcShower* track);
  static void AddTrack(WTrackParameter track);
  static void AddTracks(ParticleSelection& selection);
  static void AddConstraintCMS();
  static void AddResonance(const Double_t mass, Int_t track1, Int_t track2);
  static void Fit();

  static bool                IsSuccessful() { return fIsSuccessful; };
  static Int_t               GetNContstraints() { return fNConstraints; }
  static HepLorentzVector    GetTrack(Int_t i);
  static KalmanKinematicFit* GetFit() { return fKinematicFit; }
  ///< @todo `fKinematicFit` should somehow be protected, but this is not yet possible as long as KKFitResult does not accept `const KalmanKinematicFit*`.
  static Double_t GetChi2() { return fKinematicFit->chisq(); }

private:
  KinematicFitter() { Initialize(); }
  KinematicFitter(const KinematicFitter&) {}
  KinematicFitter& operator=(const KinematicFitter&) {}

  static Int_t               fNTracks;
  static Int_t               fConstraintCount;
  static Int_t               fNConstraints;
  static bool                fIsSuccessful;
  static KalmanKinematicFit* fKinematicFit;
};

namespace IniSelect
{
  namespace Error
  {
    class KinematicFitFailed : public Exception
    {
    public:
      KinematicFitFailed(Double_t chi2) : Exception(Form("Kinematic fit failed: chi2 = %g", chi2))
      {}
    };
  } // namespace Error
} // namespace IniSelect
/// @}
#endif