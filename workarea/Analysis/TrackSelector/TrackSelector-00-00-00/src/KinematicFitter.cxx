#include "TrackSelector/KinematicFitter.h"
#include "TrackSelector/TSGlobals.h"
using namespace TSGlobals;

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
  fKinematicFit->AddResonance(fConstraintCount, Mass::pi0, track1, track2);
  ++fConstraintCount;
  ++fNConstraints;
}

void KinematicFitter::Fit()
{
  if(fKinematicFit->Fit())
  {
    fIsSuccessful = true;
    return;
  }
  std::cout << "    fit failed: chisq = " << fKinematicFit->chisq() << std::endl;
  throw Error::FitFailed;
}

HepLorentzVector KinematicFitter::GetTrack(int i) const
{
  if(!fKinematicFit) throw Error::FitFailed;
  if(i < fNTracks) return fKinematicFit->pfit(i);
  throw Error::OutOfRange;
}