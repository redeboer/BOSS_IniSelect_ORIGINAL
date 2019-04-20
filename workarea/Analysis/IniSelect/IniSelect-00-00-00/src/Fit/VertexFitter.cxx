#include "TrackSelector/Fit/VertexFitter.h"
#include "TrackSelector/TSGlobals/TSException.h"
using namespace TSGlobals::Error;

VertexFitter::VertexFitter() : fNTracks(0), fIsSuccessful(false)
{
  Initialize();
}

VertexFit* VertexFitter::fVertexFit = VertexFit::instance();

void VertexFitter::Initialize()
{
  fVertexFit->init();
  fNTracks      = 0;
  fIsSuccessful = false;
}

void VertexFitter::AddTrack(EvtRecTrack* track, const double& mass)
{
  RecMdcKalTrack* kalTrack = track->mdcKalTrack();
  WTrackParameter wTrackParam(mass, kalTrack->getZHelix(), kalTrack->getZError());
  fVertexFit->AddTrack(fNTracks, wTrackParam);
  ++fNTracks;
}

void VertexFitter::AddCleanVertex()
{
  fVertexFit->AddVertex(0, BuildVertexParameter(), 0, 1);
}

void VertexFitter::FitAndSwim()
{
  if(!fVertexFit->Fit(0)) throw Exception("Vertex fit failed");
  fVertexFit->Swim(0);
  fIsSuccessful = true;
}

VertexParameter VertexFitter::BuildVertexParameter() const
{
  HepPoint3D   vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  double       bx = 1E+6;
  double       by = 1E+6;
  double       bz = 1E+6;
  Evx[0][0]       = bx * bx;
  Evx[1][1]       = by * by;
  Evx[2][2]       = bz * bz;
  VertexParameter vtxPar;
  vtxPar.setVx(vx);
  vtxPar.setEvx(Evx);
  return vtxPar;
}

WTrackParameter VertexFitter::GetTrack(int i) const
{
  if(i < fNTracks) return fVertexFit->wtrk(i);
  throw OutOfRange("VertexFitter::GetTrack", i, fNTracks);
}