#include "JpsiToDPV/Trees/TreeVertex.h"
#include "CLHEP/Geometry/Point3D.h"
#include "VertexFit/Helix.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

void TreeVertex::Fill(const Hep3Vector& origin, RecMdcTrack* mdcTrk)
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