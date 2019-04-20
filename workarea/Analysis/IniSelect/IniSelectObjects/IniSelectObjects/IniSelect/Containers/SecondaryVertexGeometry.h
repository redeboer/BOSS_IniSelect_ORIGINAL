#ifndef Analysis_IniSelect_SecondaryVertexGeometry_H
#define Analysis_IniSelect_SecondaryVertexGeometry_H

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "MdcRecEvent/RecMdcTrack.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

/// @addtogroup BOSS_objects
/// @{

/// A container for \f$\theta\f$, \f$\phi\f$, and angle.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 16th, 2018
class SecondaryVertexGeometry
{
public:
  SecondaryVertexGeometry() {}
  SecondaryVertexGeometry(RecMdcTrack* mdcTrack, const HepPoint3D& vertexPoint);

  void SetValues(RecMdcTrack* mdcTrack, const HepPoint3D& vertexPoint);

  const HepPoint3D& GetPoint() const { return fPoint; }

  const double& GetPhi() const { return fPhi; }
  const double& GetR() const { return fR; }
  const double& GetRphi() const { return fRphi; }
  const double& GetRxy() const { return fRxy; }
  const double& GetRz() const { return fRz; }
  const double& GetMomentum() const { return fMomentum; }

private:
  HepPoint3D fPoint;

  double fPhi;
  double fR;    ///< Nearest distance to IP in xy plane.
  double fRphi; ///< Angle in the xy-plane (?).
  double fRxy;  ///< Nearest distance to IP in xy plane.
  double fRz;   ///< Nearest distance to IP in z direction.
  double fMomentum;
};

/// @}

#endif