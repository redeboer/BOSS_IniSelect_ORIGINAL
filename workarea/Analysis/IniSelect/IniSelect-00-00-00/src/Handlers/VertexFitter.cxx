#include "IniSelect/Handlers/VertexFitter.h"
#include "IniSelect/Exceptions/OutOfRange.h"
using namespace std;

Int_t      VertexFitter::fNTracks      = 0;
Bool_t     VertexFitter::fIsSuccessful = false;
VertexFit* VertexFitter::fVertexFit    = VertexFit::instance();

void VertexFitter::Initialize()
{
  fVertexFit->init();
  fNTracks      = 0;
  fIsSuccessful = false;
}

void VertexFitter::AddTrack(EvtRecTrack* track, const Double_t mass)
{
  RecMdcKalTrack* kalTrack = track->mdcKalTrack();
  WTrackParameter wTrackParam(mass, kalTrack->getZHelix(), kalTrack->getZError());
  fVertexFit->AddTrack(fNTracks, wTrackParam);
  ++fNTracks;
}

void VertexFitter::AddTracks(ParticleSelection& selection)
{
  CandidateTracks<EvtRecTrack>* coll = selection.FirstParticle();
  while(coll)
  {
    for(Int_t i = 0; i < coll->GetNTracks(); ++i)
      VertexFitter::AddTrack(coll->GetCandidate(i), coll->GetMass());
    coll = selection.NextCharged();
  }
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

VertexParameter VertexFitter::BuildVertexParameter()
{
  HepPoint3D   vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  Evx[0][0] = 1E+12;
  Evx[1][1] = 1E+12;
  Evx[2][2] = 1E+12;
  VertexParameter vtxPar;
  vtxPar.setVx(vx);
  vtxPar.setEvx(Evx);
  return vtxPar;
}

WTrackParameter VertexFitter::GetTrack(Int_t i)
{
  if(i < fNTracks) return fVertexFit->wtrk(i);
  throw OutOfRange("VertexFitter::GetTrack", i, fNTracks);
}