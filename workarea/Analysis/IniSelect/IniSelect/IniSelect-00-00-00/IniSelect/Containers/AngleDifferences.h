#ifndef Analysis_IniSelect_AngleDifferences_H
#define Analysis_IniSelect_AngleDifferences_H

#include "CLHEP/Vector/ThreeVector.h"
#include <cmath>

/// @addtogroup BOSS_objects
/// @{

/// A container for \f$\theta\f$, \f$\phi\f$, and angle.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 16th, 2018
class AngleDifferences
{
public:
  AngleDifferences() : fIsRadians{true} { SetToMax(); }
  AngleDifferences(const CLHEP::Hep3Vector& vec1, const CLHEP::Hep3Vector& vec2);

  void SetToMax();
  void SetAngleDifferences(const CLHEP::Hep3Vector& vec1, const CLHEP::Hep3Vector& vec2);
  void ConvertToDegrees();
  void ConvertToRadians();

  const double& GetAngle() const { return fAngle; }
  const double& GetPhi() const { return fPhi; }
  const double& GetTheta() const { return fTheta; }

  const double GetAbsoluteAngle() const { return std::abs(GetAngle()); }
  const double GetAbsolutePhi() const { return std::abs(GetPhi()); }
  const double GetAbsoluteTheta() const { return std::abs(GetTheta()); }

  bool IsSmaller(const AngleDifferences& other) const { return fAngle < other.GetAngle(); }

private:
  double fAngle;
  double fPhi;
  double fTheta;
  bool   fIsRadians;
};

/// @}

#endif