#include "IniSelect/Handlers/KinematicFitter.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Exceptions/KinematicFitFailed.h"
#include "IniSelect/Handlers/VertexFitter.h"
using namespace IniSelect;
using namespace IniSelect::Physics;

Int_t               KinematicFitter::fNTracks         = 0;
Int_t               KinematicFitter::fConstraintCount = 0;
Int_t               KinematicFitter::fNConstraints    = 0;
bool                KinematicFitter::fIsSuccessful    = false;
KalmanKinematicFit* KinematicFitter::fKinematicFit    = KalmanKinematicFit::instance();

void KinematicFitter::Initialize()
{
  fKinematicFit->init();
  fNTracks         = 0;
  fNConstraints    = 0;
  fConstraintCount = 0;
  fIsSuccessful    = false;
}

void KinematicFitter::AddTrack(RecEmcShower* track)
{
  fKinematicFit->AddTrack(fNTracks, 0., track);
  ++fNTracks;
}

void KinematicFitter::AddTrack(WTrackParameter track)
{
  fKinematicFit->AddTrack(fNTracks, track);
  ++fNTracks;
}

void KinematicFitter::AddTracks(ParticleSelection& selection)
{
  CandidateTracks<EvtRecTrack>* coll = selection.FirstParticle();
  while(coll)
  {
    for(Int_t i = 0; i < coll->GetNTracks(); ++i)
      if(coll->IsCharged())
        KinematicFitter::AddTrack(VertexFitter::GetTrack(i));
      else
        KinematicFitter::AddTrack(coll->GetCandidate(i)->emcShower());
    coll = selection.NextParticle();
  }
}

void KinematicFitter::AddConstraintCMS()
{
  /// Add the typical 4-constraint: CMS energy and 3-momentum
  fKinematicFit->AddFourMomentum(fConstraintCount, EcmsVec);
  ++fConstraintCount;
  fNConstraints += 4;
}

void KinematicFitter::AddResonance(const Double_t mass, Int_t track1, Int_t track2)
{
  fKinematicFit->AddResonance(fConstraintCount, mass, track1, track2);
  ++fConstraintCount;
  ++fNConstraints;
}

void KinematicFitter::Fit()
{
  if(!fKinematicFit->Fit()) throw KinematicFitFailed(fKinematicFit->chisq());
  fIsSuccessful = true;
}

HepLorentzVector KinematicFitter::GetTrack(Int_t i)
{
  if(!fKinematicFit) throw Exception("KinematicFitter has empty pointer!");
  if(i < fNTracks) return fKinematicFit->pfit(i);
  throw OutOfRange("KinematicFitter::GetTrack", i, fNTracks);
}