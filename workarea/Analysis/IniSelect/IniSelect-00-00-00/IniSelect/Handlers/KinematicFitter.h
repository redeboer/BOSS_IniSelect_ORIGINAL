#ifndef Analysis_IniSelect_KinematicFitter_H
#define Analysis_IniSelect_KinematicFitter_H

#include "EmcRecEventModel/RecEmcShower.h"
#include "IniSelect/Exceptions/Exception.h"
#include "IniSelect/Handlers/FinalStateHandler.h"
#include "TString.h"
#include "VertexFit/KalmanKinematicFit.h"
#include <map>
#include <string>
#include <vector>

/// @todo This class functions more as a `namespce`, but has to be a class as it wraps the `KalmanKinematicFit` singleton.
/// @addtogroup BOSS_objects
/// @{
class KinematicFitter
{
public:
  static void Initialize();
  static void AddTrack(RecEmcShower* track);
  static void AddTrack(WTrackParameter track);
  static void AddTracks(FinalStateHandler& selection);
  static void AddConstraintCMS();
  static void AddResonance(const Double_t mass, Int_t track1, Int_t track2);
  static void Fit();

  static bool                IsSuccessful() { return fIsSuccessful; };
  static Int_t               GetNContstraints() { return fNConstraints; }
  static HepLorentzVector    GetTrack(const std::string& pdtName, size_t i = 0);
  static KalmanKinematicFit* GetFit() { return fKinematicFit; }
  ///< @todo `fKinematicFit` should somehow be protected, but this is not yet possible as long as KKFitResult does not accept `const KalmanKinematicFit*`.
  static Double_t GetChi2() { return fKinematicFit->chisq(); }

  static void ThrowIfEmpty();

private:
  KinematicFitter() { Initialize(); }
  KinematicFitter(const KinematicFitter&) {}
  KinematicFitter& operator=(const KinematicFitter&) {}

  static Int_t               fNTracks;
  static Int_t               fConstraintCount;
  static Int_t               fNConstraints;
  static bool                fIsSuccessful;
  static KalmanKinematicFit* fKinematicFit;

  static std::map<std::string, std::vector<Int_t> > fNameToIndex;
};
/// @}

/// @addtogroup BOSS_globals
/// @{
/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date May 17th, 2019
class NoKinematicFit : public Exception
{
public:
  NoKinematicFit() : Exception("Empty pointer for kinematic fit") {}
};
/// @}
#endif