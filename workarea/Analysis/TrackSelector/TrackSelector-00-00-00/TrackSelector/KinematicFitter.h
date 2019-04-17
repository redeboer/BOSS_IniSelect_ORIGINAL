#ifndef Physics_KinematicFitter_H
#define Physics_KinematicFitter_H

#include "EmcRecEventModel/RecEmcShower.h"
#include "VertexFit/KalmanKinematicFit.h"

/// @addtogroup BOSS_objects
/// @{
class KinematicFitter
{
public:
  KinematicFitter();

  void Initialize();
  void AddTrack(RecEmcShower* track);
  void AddTrack(WTrackParameter track);
  void AddConstraintCMS();
  void AddResonance(const double& mass, int track1, int track2);
  void Fit();

  const bool&         IsSuccessful() const { return fIsSuccessful; };
  const int&          GetNContstraints() const { return fNConstraints; }
  HepLorentzVector    GetTrack(int i) const;
  KalmanKinematicFit* GetFit() { return fKinematicFit; }
  ///< @todo `fKinematicFit` should somehow be protected, but this is not yet possible as long as KKFitResult does not accept `const KalmanKinematicFit*`.

private:
  int  fNTracks;
  int  fConstraintCount;
  int  fNConstraints;
  bool fIsSuccessful;

  static KalmanKinematicFit* fKinematicFit;
};
/// @}
#endif