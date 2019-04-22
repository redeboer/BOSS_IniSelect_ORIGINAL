#ifndef Analysis_IniSelect_KinematicFitter_H
#define Analysis_IniSelect_KinematicFitter_H

#include "EmcRecEventModel/RecEmcShower.h"
#include "IniSelect/TrackCollections/ParticleSelection.h"
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
  static void AddResonance(const double& mass, int track1, int track2);
  static void Fit();

  static bool                IsSuccessful() { return fIsSuccessful; };
  static int                 GetNContstraints() { return fNConstraints; }
  static HepLorentzVector    GetTrack(int i);
  static KalmanKinematicFit* GetFit() { return fKinematicFit; }
  ///< @todo `fKinematicFit` should somehow be protected, but this is not yet possible as long as KKFitResult does not accept `const KalmanKinematicFit*`.
  static double GetChi2() { return fKinematicFit->chisq(); }

private:
  KinematicFitter() { Initialize(); }
  KinematicFitter(const KinematicFitter&) {}
  KinematicFitter& operator=(const KinematicFitter&) {}

  static int                 fNTracks;
  static int                 fConstraintCount;
  static int                 fNConstraints;
  static bool                fIsSuccessful;
  static KalmanKinematicFit* fKinematicFit;
};
/// @}
#endif