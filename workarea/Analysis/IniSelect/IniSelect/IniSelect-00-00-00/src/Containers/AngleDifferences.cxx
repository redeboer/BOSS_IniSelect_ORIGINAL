#include "IniSelect/Containers/AngleDifferences.h"
#include "IniSelect/Globals.h"
#include <float.h> // for DBL_MAX

using CLHEP::Hep3Vector;
using namespace IniSelect;

void AngleDifferences::SetToMax()
{
  fTheta = DBL_MAX;
  fPhi   = DBL_MAX;
  fAngle = DBL_MAX;
}

AngleDifferences::AngleDifferences(const Hep3Vector& vec1, const Hep3Vector& vec2) :
  fIsRadians{true}
{
  SetAngleDifferences(vec1, vec2);
}

void AngleDifferences::SetAngleDifferences(const Hep3Vector& vec1, const Hep3Vector& vec2)
{
  fAngle = vec1.angle(vec2);
  fTheta = vec1.theta() - vec2.theta();
  fPhi   = vec1.deltaPhi(vec2);
  fTheta = fmod(5 * Math::pi + fTheta, Math::twopi) - Math::pi;
  fPhi   = fmod(5 * Math::pi + fPhi, Math::twopi) - Math::pi;
}

void AngleDifferences::ConvertToDegrees()
{
  if(!fIsRadians) return;
  fTheta *= Math::RadToDeg;
  fPhi *= Math::RadToDeg;
  fAngle *= Math::RadToDeg;
  fIsRadians = false;
}

void AngleDifferences::ConvertToRadians()
{
  if(fIsRadians) return;
  fTheta /= Math::RadToDeg;
  fPhi /= Math::RadToDeg;
  fAngle /= Math::RadToDeg;
  fIsRadians = true;
}