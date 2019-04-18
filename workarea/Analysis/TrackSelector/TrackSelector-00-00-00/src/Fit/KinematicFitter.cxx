#include "TrackSelector/Fit/KinematicFitter.h"
#include "TrackSelector/TSGlobals.h"
#include "TrackSelector/TSGlobals/TSException.h"
using namespace TSGlobals;
using namespace TSGlobals::Error;

KinematicFitter::KinematicFitter() :
  fNTracks(0),
  fNConstraints(0),
  fConstraintCount(0),
  fIsSuccessful(false)
{
  Initialize();
}

KalmanKinematicFit* KinematicFitter::fKinematicFit = KalmanKinematicFit::instance();

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
  if(!fKinematicFit->Fit())
    throw Exception(Form("Kinematic fit failed: chisq = %g", fKinematicFit->chisq()));
  fIsSuccessful = true;
}

HepLorentzVector KinematicFitter::GetTrack(int i) const
{
  if(!fKinematicFit) throw Exception("KinematicFitter has empty pointer!");
  if(i < fNTracks) return fKinematicFit->pfit(i);
  throw OutOfRange("KinematicFitter::GetTrack", i, fNTracks);
}