#include "IniSelect/Handlers/KinematicFitter.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Globals/Exception.h"
#include "IniSelect/Handlers/VertexFitter.h"
using namespace IniSelect;
using namespace IniSelect::Error;

int                 KinematicFitter::fNTracks         = 0;
int                 KinematicFitter::fConstraintCount = 0;
int                 KinematicFitter::fNConstraints    = 0;
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
    for(int i = 0; i < coll->GetNTracks(); ++i)
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
  fKinematicFit->AddFourMomentum(fConstraintCount, gEcmsVec);
  ++fConstraintCount;
  fNConstraints += 4;
}

void KinematicFitter::AddResonance(const double& mass, int track1, int track2)
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

HepLorentzVector KinematicFitter::GetTrack(int i)
{
  if(!fKinematicFit) throw Exception("KinematicFitter has empty pointer!");
  if(i < fNTracks) return fKinematicFit->pfit(i);
  throw OutOfRange("KinematicFitter::GetTrack", i, fNTracks);
}