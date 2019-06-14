#include "IniSelect/Handlers/VertexFitter.h"
#include "IniSelect/Exceptions/OutOfRange.h"
#include "IniSelect/TrackCollections/CandidateIter.h"
using namespace std;

Int_t      VertexFitter::fNTracks      = 0;
Bool_t     VertexFitter::fIsSuccessful = false;
VertexFit* VertexFitter::fVertexFit    = VertexFit::instance();

std::map<std::string, std::vector<Int_t> > VertexFitter::fNameToIndex;

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

void VertexFitter::AddTracks(FinalStateHandler& selection)
{
  ChargedCandidateIter it(selection.GetCandidates());
  while(CandidateTracks<EvtRecTrack>* coll = it.Next())
    for(Int_t i = 0; i < coll->GetNTracks(); ++i)
    {
      VertexFitter::AddTrack(coll->GetCandidate(i), coll->GetMass());
      fNameToIndex[coll->GetPdtName()].push_back(i);
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

WTrackParameter VertexFitter::GetTrack(const std::string& pdtName, size_t i)
{
  VertexFitter::ThrowIfEmpty();
  return fVertexFit->wtrk(fNameToIndex.at(pdtName).at(i));
  throw OutOfRange(Form("VertexFitter::GetTrack key %s[%ul]", pdtName.c_str(), i));
}

void VertexFitter::ThrowIfEmpty()
{
  if(!fVertexFit) throw NoVertexFit();
}