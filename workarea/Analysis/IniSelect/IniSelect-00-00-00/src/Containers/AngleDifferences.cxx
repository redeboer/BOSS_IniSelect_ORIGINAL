#include "TrackSelector/Containers/AngleDifferences.h"
#include "TrackSelector/TrackSelector.h"
#include <float.h> // for DBL_MAX

using CLHEP::Hep3Vector;

const double gRadToDegree = 180. / (CLHEP::pi);

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
  fTheta = fmod(5 * CLHEP::pi + fTheta, CLHEP::twopi) - CLHEP::pi;
  fPhi   = fmod(5 * CLHEP::pi + fPhi, CLHEP::twopi) - CLHEP::pi;
}

void AngleDifferences::ConvertToDegrees()
{
  if(!fIsRadians) return;
  fTheta *= gRadToDegree;
  fPhi *= gRadToDegree;
  fAngle *= gRadToDegree;
  fIsRadians = false;
}

void AngleDifferences::ConvertToRadians()
{
  if(fIsRadians) return;
  fTheta /= gRadToDegree;
  fPhi /= gRadToDegree;
  fAngle /= gRadToDegree;
  fIsRadians = true;
}