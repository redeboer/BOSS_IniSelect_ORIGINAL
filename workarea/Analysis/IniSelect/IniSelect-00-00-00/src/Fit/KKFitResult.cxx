#include "IniSelect/Fit/KKFitResult.h"
#include <float.h> // for DBL_MAX
using CLHEP::HepLorentzVector;

double KKFitResult::fBestCompareValue = DBL_MAX;

/// Constructor that immediately instantiates all its members from a `KalmanKinematic` fit result.
KKFitResult::KKFitResult() :
  fChiSquared(DBL_MAX),
  fFitMeasure(DBL_MAX),
  fHasResults(false)
{
  KinematicFitter::ThrowIfEmpty();
  SetValues();
  fChiSquared = KinematicFitter::GetFit()->chisq();
}

void KKFitResult::SetValues()
{
  SetValues_impl();
  fHasResults = true;
}

/// Comparison method.
bool KKFitResult::IsBetter(const double& value, double& bestvalue) const
{
  KinematicFitter::ThrowIfEmpty();
  if(value > bestvalue) return false;
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