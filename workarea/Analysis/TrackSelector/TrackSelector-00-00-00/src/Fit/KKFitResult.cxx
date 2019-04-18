// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "TrackSelector/Fit/KKFitResult.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <cmath>
using CLHEP::HepLorentzVector;

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //

double KKFitResult::fBestCompareValue = DBL_MAX;

/// Constructor that immediately instantiates all its members from a `KalmanKinematic` fit result.
KKFitResult::KKFitResult(KalmanKinematicFit* kkmfit) :
  fChiSquared(DBL_MAX),
  fFitMeasure(DBL_MAX),
  fFit(kkmfit),
  fHasResults(false)
{
  if(!fFit) return;
  fChiSquared = fFit->chisq();
}

// * ========================= * //
// * -------- SETTERS -------- * //
// * =u======================== * //

void KKFitResult::SetRunAndEventNumber(SmartDataPtr<Event::EventHeader> header)
{
  fRunNumber   = header->runNumber();
  fEventNumber = header->eventNumber();
}

// * =================================== * //
// * -------- PROTECTED METHODS -------- * //
// * =================================== * //

/// Comparison method.
bool KKFitResult::IsBetter(const double& value, double& bestvalue) const
{
  /// -# Returns `false` if the object does not contain a `KalmanKinematicFit`.
  if(!fFit) return false;
  /// -# Returns `false` if its own `fCompareValue` worse (`>`) than `fBestCompareValue`.
  if(value > bestvalue) return false;
  /// -# If not, update `fBestCompareValue`.
  bestvalue = value;
  return true;
}

/// Default function of `KKFitResult::IsBetter(const double &value, double &bestvalue)`.
/// Overwrite with your own implementation if necessary.
bool KKFitResult::IsBetter() const
{
  return KKFitResult::IsBetter(fFitMeasure, fBestCompareValue);
}

/// Compute length of 3-momentum of a Lorentz vector.
const double KKFitResult::ThreeMomentum(const HepLorentzVector& vec) const
{
  return std::sqrt(vec.px() * vec.px() + vec.py() * vec.py() + vec.pz() * vec.pz());
}