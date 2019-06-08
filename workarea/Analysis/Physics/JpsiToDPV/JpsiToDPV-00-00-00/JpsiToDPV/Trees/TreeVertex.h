#ifndef JpsiToDPV_TreeVertex_H
#define JpsiToDPV_TreeVertex_H

#include "JpsiToDPV/Trees/TreeContainer.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "VertexFit/Helix.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::Hep3Vector;

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeVertex : public TreeContainer
{
public:
  TreeVertex() : TreeContainer("vxyz", "Vertex information of the charged tracks")
  {
    BRANCH(p);
    BRANCH(x);
    BRANCH(y);
    BRANCH(z);
    BRANCH(r);
    BRANCH(rxy);
    BRANCH(rz);
    BRANCH(rphi);
  }
  Double_t p;    ///< Momentum
  Double_t x;    ///< Primary \f$x\f$-vertex as determined by MDC
  Double_t y;    ///< Primary \f$y\f$-vertex as determined by MDC
  Double_t z;    ///< Primary \f$z\f$-vertex as determined by MDC
  Double_t r;    ///< Distance from origin in \f$xy\f$-plane
  Double_t phi;
  Double_t rxy;  ///< Nearest distance to IP in \f$xy\f$ plane
  Double_t rz;   ///< Nearest distance to IP in \f$z\f$ direction
  Double_t rphi; ///< Angle in the \f$xy\f$-plane (?)
  void Fill(const Hep3Vector& origin, RecMdcTrack* mdcTrk)
  {
    // * Get track info from Main Drift Chamber
    p   = mdcTrk->p();
    x   = mdcTrk->x();
    y   = mdcTrk->y();
    z   = mdcTrk->z();
    phi = mdcTrk->helix(1);

    // * Get vertex origin
    double xv  = origin.x();
    double yv  = origin.y();
    r = (x - xv) * cos(phi) + (y - yv) * sin(phi);

    // * Get radii of vertex
    HepVector    a  = mdcTrk->helix();
    HepSymMatrix Ea = mdcTrk->err();
    HepPoint3D   point0(0., 0., 0.); // the initial point for MDC recosntruction
    HepPoint3D   IP(origin[0], origin[1], origin[2]);
    VFHelix      helixip(point0, a, Ea);
    helixip.pivot(IP);
    HepVector vecipa = helixip.a();
    rxy     = fabs(vecipa[0]); // nearest distance to IP in xy plane
    rz      = vecipa[3];       // nearest distance to IP in z direction
    rphi    = vecipa[1];

    // * WRITE primary vertex position info ("vxyz" branch) *
    fTree->Fill();
  }
};
/// @}
#endif