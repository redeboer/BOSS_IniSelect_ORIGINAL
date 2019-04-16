#include "TrackSelector/Containers/SecondaryVertexGeometry.h"
#include "VertexFit/Helix.h"
#include <cmath>
using namespace std;

SecondaryVertexGeometry::SecondaryVertexGeometry(RecMdcTrack* mdcTrack, const HepPoint3D& vtxPoint)
{
  SetValues(mdcTrack, vtxPoint);
}

void SecondaryVertexGeometry::SetValues(RecMdcTrack* mdcTrack, const HepPoint3D& vtxPoint)
{
  fPoint.setX(mdcTrack->x());
  fPoint.setY(mdcTrack->y());
  fPoint.setZ(mdcTrack->z());

  fPhi = mdcTrack->helix(1);
  fR   = (fPoint.x() - vtxPoint.x()) * cos(fPhi) + (fPoint.y() - vtxPoint.y()) * sin(fPhi);

  // * Get radii of track vertex
  HepVector    a  = mdcTrack->helix();
  HepSymMatrix Ea = mdcTrack->err();

  HepPoint3D point0(0., 0., 0.); // initial point for MDC reconstruction
  VFHelix    helixip(point0, a, Ea);
  helixip.pivot(vtxPoint);
  HepVector vecipa = helixip.a();

  fRvxy  = fabs(vecipa[0]);
  fRvz   = vecipa[3];
  fRvphi = vecipa[1];
}