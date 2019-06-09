#include "IniSelect/Handlers/KinematicFitter.h"
#include "IniSelect/Exceptions/Exception.h"
#include "IniSelect/Exceptions/KinematicFitFailed.h"
#include "IniSelect/Exceptions/OutOfRange.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Handlers/VertexFitter.h"
#include "IniSelect/TrackCollections/CandidateIter.h"
using namespace IniSelect;
using namespace IniSelect::Physics;
using namespace std;

Int_t               KinematicFitter::fNTracks         = 0;
Int_t               KinematicFitter::fConstraintCount = 0;
Int_t               KinematicFitter::fNConstraints    = 0;
bool                KinematicFitter::fIsSuccessful    = false;
KalmanKinematicFit* KinematicFitter::fKinematicFit    = KalmanKinematicFit::instance();

std::map<std::string, std::vector<Int_t> >  KinematicFitter::fNameToIndex;

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

void KinematicFitter::AddTracks(FinalStateHandler& selection)
{
  CandidateIter it(selection.GetCandidates());
  while(CandidateTracks<EvtRecTrack>* coll = it.Next())
    for(Int_t i = 0; i < coll->GetNTracks(); ++i)
    {
      fNameToIndex[coll->GetPdtName()].push_back(i);
      if(coll->IsCharged())
        KinematicFitter::AddTrack(VertexFitter::GetTrack(coll->GetPdtName()));
      else
        KinematicFitter::AddTrack(coll->GetCandidate(i)->emcShower());
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

HepLorentzVector KinematicFitter::GetTrack(const string& pdtName, size_t i)
{
  KinematicFitter::ThrowIfEmpty();
  return fKinematicFit->pfit(fNameToIndex.at(pdtName).at(i));
  throw OutOfRange(Form("KinematicFitter::GetTrack key %s[%ul]", pdtName.c_str(), i));
}

void KinematicFitter::ThrowIfEmpty()
{
  if(!fKinematicFit) throw NoKinematicFit();
}