#include "IniSelect/Algorithms/TrackSelector.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "DstEvent/TofHitStatus.h"
#include "GaudiKernel/Bootstrap.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Handlers/ParticleIdentifier.h"
#include "TMath.h"
#include "TString.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"
#include <cmath>

using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using namespace IniSelect;
using namespace std;

/// This method is called **for each event**.
StatusCode TrackSelector::execute()
{
  cout << endl;
  LOG_FUNCTION();
  try
  {
    fInputFile.LoadNextEvent();
    PrintEventInfo();
    SetVertexOrigin();
    CreateChargedCollection();
    CreateNeutralCollection();
    WriteVertexInfo();
    CutNumberOfChargedParticles();
    SelectChargedCandidates();
    CreateNeutralTrackSelections();
    WriteMultiplicities();
    PrintMultiplicities();
    CutPID();
    WriteDedxOfSelectedParticles();
    FindBestKinematicFit();
  }
  catch(...)
  {}
  return StatusCode::SUCCESS;
}

void TrackSelector::PrintEventInfo()
{
  fLog << MSG::DEBUG << "RUN " << fInputFile.RunNumber() << ", "
       << "event number " << fInputFile.EventNumber() << endmsg;
  fLog << MSG::DEBUG << "Ncharged = " << fInputFile.TotalChargedTracks() << ", "
       << "Nneutral = " << fInputFile.TotalNeutralTracks() << ", "
       << "Ntotal = " << fInputFile.TotalTracks() << endmsg;
}

void TrackSelector::SetVertexOrigin()
{
  LOG_FUNCTION();
  IVertexDbSvc* vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(!vtxsvc->isVertexValid()) return;
  double* dbv = vtxsvc->PrimaryVertex();
  // double* vv  = vtxsvc->SigmaPrimaryVertex();
  fVertexPoint.set(dbv[0], dbv[1], dbv[2]);
}

/// Create a preselection of charged tracks (without cuts).
/// This method is used in `TrackSelector::execute` only. See `fChargedTracks` for more information.
void TrackSelector::CreateChargedCollection()
{
  LOG_FUNCTION();
  if(!fCreateChargedCollection) return;
  fChargedTracks.clear();
  fNetChargeMDC = 0;
  ChargedTrackIter iter(fInputFile);
  while(EvtRecTrack* track = iter.Next())
  {
    fTrackMDC = track->mdcTrack();
    fSecondaryVtx.SetValues(fTrackMDC, fVertexPoint);
    if(!CutSecondaryVertex()) continue;

    // * Add charged track to vector
    fChargedTracks.push_back(track);
    fNetChargeMDC += fTrackMDC->charge();
    WriteChargedTrackVertex();

    /// -# **Write** dE/dx PID information ("dedx" branch)
    WriteDedxInfo(track, fNTuple_dedx);

    /// -# **Write** Time-of-Flight PID information ("tof*" branch)
    if(fNTuple_TofEC.DoWrite() || fNTuple_TofIB.DoWrite() || fNTuple_TofOB.DoWrite())
    {
      // * Check if MDC and TOF info for track are valid *
      if(!track->isMdcTrackValid()) continue;
      if(!track->isTofTrackValid()) continue;
      // * Get momentum as determined by MDC *
      fTrackMDC = track->mdcTrack();
      double ptrk;
      if(fTrackMDC) ptrk = fTrackMDC->p();
      SmartRefVector<RecTofTrack>           tofTrkCol = track->tofTrack();
      SmartRefVector<RecTofTrack>::iterator iter_tof  = tofTrkCol.begin();
      for(; iter_tof != tofTrkCol.end(); ++iter_tof)
      {
        TofHitStatus hitStatus;
        hitStatus.setStatus((*iter_tof)->status());
        if(!hitStatus.is_counter()) continue;
        if(hitStatus.is_barrel())
        {
          if(hitStatus.layer() == 1) // inner barrel
            WriteTofInformation(iter_tof->data(), ptrk, fNTuple_TofIB);
          else if(hitStatus.layer() == 2) // outer barrel
            WriteTofInformation(iter_tof->data(), ptrk, fNTuple_TofOB);
        }
        else if(hitStatus.layer() == 0) // end cap
          WriteTofInformation(iter_tof->data(), ptrk, fNTuple_TofEC);
      }
    }
  }

  fLog << MSG::DEBUG << "Number of 'good' charged tracks: " << fChargedTracks.size() << endmsg;
}

bool TrackSelector::CutSecondaryVertex()
{
  if(fCut_Vz.FailsMax(fabs(fTrackMDC->z()))) return false;
  if(fCut_Vxy.FailsMax(fSecondaryVtx.GetR())) return false;
  if(fCut_Rz.FailsMax(fabs(fSecondaryVtx.GetRz()))) return false;
  if(fCut_Rxy.FailsMax(fSecondaryVtx.GetRxy())) return false;
  if(fCut_CosTheta.FailsMax(fabs(cos(fTrackMDC->theta())))) return false;
  return true;
}

void TrackSelector::WriteChargedTrackVertex()
{
  if(!fNTuple_charged.DoWrite()) return;
  fNTuple_charged.GetItem<double>("vx")    = fSecondaryVtx.GetPoint().x();
  fNTuple_charged.GetItem<double>("vy")    = fSecondaryVtx.GetPoint().y();
  fNTuple_charged.GetItem<double>("vz")    = fSecondaryVtx.GetPoint().z();
  fNTuple_charged.GetItem<double>("vr")    = fSecondaryVtx.GetR();
  fNTuple_charged.GetItem<double>("rvxy")  = fSecondaryVtx.GetRxy();
  fNTuple_charged.GetItem<double>("rvz")   = fSecondaryVtx.GetRz();
  fNTuple_charged.GetItem<double>("rvphi") = fSecondaryVtx.GetRphi();
  fNTuple_charged.GetItem<double>("phi")   = fSecondaryVtx.GetPhi();
  fNTuple_charged.GetItem<double>("p")     = fSecondaryVtx.GetMomentum();
  fNTuple_charged.Write();
}

/// Encapsulates of the writing procedure for \f$dE/dx\f$ energy loss information *for one track*. Here, you should use `map::at` to access the `NTuple::Item`s and `NTuplePtr`, because you want your package to throw an exception if the element does not exist. See http://bes3.to.infn.it/Boss/7.0.2/html/TRecMdcDedx_8h-source.html#l00115 for available data members of `RecMdcDedx`
void TrackSelector::WriteDedxInfo(EvtRecTrack* evtRecTrack, NTupleContainer& tuple)
{
  /// -# Abort if the 'write `JobSwitch`' has been set to `false`.
  if(!tuple.DoWrite()) return;

  /// -# Abort if \f$dE/dx\f$ and MDC info does not exist
  if(!evtRecTrack->isMdcTrackValid()) return;
  if(!evtRecTrack->isMdcDedxValid()) return;
  fTrackMDC  = evtRecTrack->mdcTrack();
  fTrackDedx = evtRecTrack->mdcDedx();

  /// -# Set \f$dE/dx\f$ info and set the `NTuple::Item`s.
  // tuple.GetItem<double>("dedx_K")     = fTrackDedx->getDedxExpect(3);
  // tuple.GetItem<double>("dedx_P")     = fTrackDedx->getDedxExpect(4);
  // tuple.GetItem<double>("dedx_e")     = fTrackDedx->getDedxExpect(0);
  // tuple.GetItem<double>("dedx_mom")   = fTrackDedx->getDedxMoment();
  // tuple.GetItem<double>("dedx_mu")    = fTrackDedx->getDedxExpect(1);
  // tuple.GetItem<double>("dedx_pi")    = fTrackDedx->getDedxExpect(2);
  tuple.GetItem<int>("Ngoodhits")  = fTrackDedx->numGoodHits();
  tuple.GetItem<int>("Ntotalhits") = fTrackDedx->numTotalHits();
  tuple.GetItem<double>("chie")    = fTrackDedx->chiE();
  tuple.GetItem<double>("chik")    = fTrackDedx->chiK();
  tuple.GetItem<double>("chimu")   = fTrackDedx->chiMu();
  tuple.GetItem<double>("chip")    = fTrackDedx->chiP();
  tuple.GetItem<double>("chipi")   = fTrackDedx->chiPi();
  tuple.GetItem<double>("normPH")  = fTrackDedx->normPH();
  tuple.GetItem<double>("p")       = fTrackMDC->p();
  tuple.GetItem<double>("probPH")  = fTrackDedx->probPH();

  /// -# **Write** \f$dE/dx\f$ info.
  tuple.Write();
}

/// Helper method for writing Time-of-Flight information.
/// This function has be created to enable you to write TOF information for different collections of tracks.
void TrackSelector::WriteTofInformation(RecTofTrack* tofTrack, double ptrk, NTupleContainer& tuple)
{
  /// -# Abort if the 'write `JobSwitch`' has been set to `false`.
  if(!tuple.DoWrite()) return;

  /// -# Compute ToF for each particle hypothesis
  double         path = tofTrack->path();
  vector<double> texp(Mass::TOF.size());
  for(size_t j = 0; j < texp.size(); ++j)
  {
    double gb   = ptrk / Mass::TOF.at(j); // v = p/m (non-relativistic velocity)
    double beta = gb / sqrt(1 + gb * gb);
    texp[j]     = 10 * path / beta / gSpeedOfLight; // hypothesis ToF
  }

  /// -# Set the `NTuple::Item`s.
  tuple.GetItem<double>("p")      = ptrk;
  tuple.GetItem<double>("tof")    = tofTrack->tof();
  tuple.GetItem<double>("path")   = path;
  tuple.GetItem<double>("cntr")   = tofTrack->tofID();
  tuple.GetItem<double>("ph")     = tofTrack->ph();
  tuple.GetItem<double>("zrhit")  = tofTrack->zrhit();
  tuple.GetItem<double>("qual")   = tofTrack->quality();
  tuple.GetItem<double>("tof_e")  = path - texp[0];
  tuple.GetItem<double>("tof_mu") = path - texp[1];
  tuple.GetItem<double>("tof_pi") = path - texp[2];
  tuple.GetItem<double>("tof_K")  = path - texp[3];
  tuple.GetItem<double>("tof_p")  = path - texp[4];

  /// -# **Write** ToF info
  tuple.Write();
}

/// Create a preselection of **neutral** tracks (without cuts).
/// This method is used in `TrackSelector::execute` only. See `fNeutralTracks` for more information.
void TrackSelector::CreateNeutralCollection()
{
  LOG_FUNCTION();
  if(!fCreateNeutralCollection) return;
  fNeutralTracks.clear();
  NeutralTrackIter iter(fInputFile);
  while(EvtRecTrack* track = iter.Next())
  {
    if(!track->isEmcShowerValid()) continue;
    if(!track->emcShower()) continue;
    fTrackEMC = track->emcShower();
    if(fCut_PhotonEnergy.FailsMin(fTrackEMC->energy())) continue;
    if(fNTuple_neutral.DoWrite())
    {
      fNTuple_neutral.GetItem<double>("E")     = fTrackEMC->energy();
      fNTuple_neutral.GetItem<double>("x")     = fTrackEMC->x();
      fNTuple_neutral.GetItem<double>("y")     = fTrackEMC->y();
      fNTuple_neutral.GetItem<double>("z")     = fTrackEMC->z();
      fNTuple_neutral.GetItem<double>("phi")   = fTrackEMC->phi();
      fNTuple_neutral.GetItem<double>("theta") = fTrackEMC->theta();
      fNTuple_neutral.GetItem<double>("time")  = fTrackEMC->time();
      fNTuple_neutral.Write();
    }
    fNeutralTracks.push_back(track);
  }
  fLog << MSG::DEBUG << "Number of 'good' photons: " << fNeutralTracks.size() << endmsg;
}

void TrackSelector::WriteMultiplicities()
{
  LOG_FUNCTION();
  if(!fNTuple_mult.DoWrite()) return;
  fNTuple_mult.GetItem<int>("Ntotal")   = fInputFile.TotalTracks();
  fNTuple_mult.GetItem<int>("Ncharge")  = fInputFile.TotalChargedTracks();
  fNTuple_mult.GetItem<int>("Nneutral") = fInputFile.TotalNeutralTracks();
  if(fCreateChargedCollection) fNTuple_mult.GetItem<int>("NgoodCharged") = fChargedTracks.size();
  if(fCreateNeutralCollection) fNTuple_mult.GetItem<int>("NgoodNeutral") = fNeutralTracks.size();
  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    fNTuple_mult.GetItem<int>(Form("N_%s", coll->GetPdtName())) = coll->GetNTracks();
    coll                                                        = fParticleSel.NextParticle();
  }
  fNTuple_mult.Write();
}

void TrackSelector::PrintMultiplicities()
{
  LOG_FUNCTION();
  // fLog << MSG::INFO;
  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    cout << "N_" << coll->GetPdtName() << " = " << coll->GetNTracks();
    coll = fParticleSel.NextParticle();
    if(coll) cout << ", ";
  }
  cout << endl;
}

/// **PID cut**: apply a strict cut on the number of the selected particles.
void TrackSelector::CutPID()
{
  LOG_FUNCTION();
  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    if(coll->FailsMultiplicityCut()) throw StatusCode::SUCCESS;
    coll = fParticleSel.NextCharged();
  }
  ++fCutFlow_NPIDnumberOK;
  fLog << MSG::INFO << "PID selection passed for (run, event) = (" << fInputFile.RunNumber() << ", "
       << fInputFile.EventNumber() << ")" << endmsg;
}

/// **Write** information about the interaction point (`"vertex"` branch).
void TrackSelector::WriteVertexInfo()
{
  LOG_FUNCTION();
  if(!fNTuple_vertex.DoWrite()) return;
  fNTuple_vertex.GetItem<double>("vx0") = fVertexPoint.x();
  fNTuple_vertex.GetItem<double>("vy0") = fVertexPoint.y();
  fNTuple_vertex.GetItem<double>("vz0") = fVertexPoint.z();
  fNTuple_vertex.Write();
}

void TrackSelector::CutNumberOfChargedParticles()
{
  LOG_FUNCTION();
  fLog << MSG::DEBUG << "Has " << fChargedTracks.size() << " charged candidated, should be "
       << fParticleSel.GetNCharged() << endmsg;
  if(fChargedTracks.size() != fParticleSel.GetNCharged()) throw StatusCode::SUCCESS;
  ++fCutFlow_NChargedOK;
}

void TrackSelector::SelectChargedCandidates()
{
  LOG_FUNCTION();
  ParticleIdentifier::Initialize();
  ConfigurePID();

  fLog << MSG::DEBUG << "Will identify particles: ";
  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    fLog << MSG::DEBUG << coll->GetPdtName();
    ParticleIdentifier::SetParticleToIdentify(coll->GetPdgCode());
    coll = fParticleSel.NextCharged();
    if(coll) fLog << MSG::DEBUG << ", ";
  }
  fLog << MSG::DEBUG << endmsg;

  fParticleSel.ClearCharged();
  fLog << MSG::DEBUG << "Identified: ";
  for(fTrackIter = fChargedTracks.begin(); fTrackIter != fChargedTracks.end(); ++fTrackIter)
  {
    string particleName = ParticleIdentifier::FindMostProbable(*fTrackIter, fCut_PIDProb);
    if(particleName.compare("") == 0) continue;
    fLog << MSG::DEBUG << particleName << "  ";
    if(!fParticleSel.HasParticle(particleName)) continue;
    WritePIDInformation();
    fParticleSel.AddCandidate(*fTrackIter, particleName);
  }
  fLog << MSG::DEBUG << endmsg;
}

/// Encapsulates the proces of writing PID info.
/// This allows you to write the PID information after the particle selection as well.
void TrackSelector::WritePIDInformation()
{
  /// -# @b Abort if the 'write `JobSwitch`' has been set to `false`.
  if(!fNTuple_PID.DoWrite()) return;

  /// -# @b Abort if there is no `fTrackMDC`.
  if(!fTrackMDC) return;

  /// -# Get PID info and set the `NTuple::Item`s.
  fLog << MSG::DEBUG << "Writing PID information" << endmsg;
  fTrackMDC                               = (*fTrackIter)->mdcTrack();
  fNTuple_PID.GetItem<double>("p")        = fTrackMDC->p();
  fNTuple_PID.GetItem<double>("cost")     = cos(fTrackMDC->theta());
  fNTuple_PID.GetItem<double>("chiToFEC") = ParticleIdentifier::GetChiTofE();
  fNTuple_PID.GetItem<double>("chiToFIB") = ParticleIdentifier::GetChiTofIB();
  fNTuple_PID.GetItem<double>("chiToFOB") = ParticleIdentifier::GetChiTofOB();
  fNTuple_PID.GetItem<double>("chidEdx")  = ParticleIdentifier::GetChiDedx();
  fNTuple_PID.GetItem<double>("prob_K")   = ParticleIdentifier::GetProbKaon();
  fNTuple_PID.GetItem<double>("prob_e")   = ParticleIdentifier::GetProbElectron();
  fNTuple_PID.GetItem<double>("prob_mu")  = ParticleIdentifier::GetProbMuon();
  fNTuple_PID.GetItem<double>("prob_p")   = ParticleIdentifier::GetProbProton();
  fNTuple_PID.GetItem<double>("prob_pi")  = ParticleIdentifier::GetProbPion();

  /// -# @b Write PID info.
  fNTuple_PID.Write();
}

void TrackSelector::CreateNeutralTrackSelections()
{
  LOG_FUNCTION();
  fParticleSel.ClearNeutral();
  for(fTrackIter = fNeutralTracks.begin(); fTrackIter != fNeutralTracks.end(); ++fTrackIter)
  {
    AngleDifferences smallestAngles = FindSmallestPhotonAngles();
    smallestAngles.ConvertToDegrees();
    WritePhotonKinematics(smallestAngles);
    if(CutPhotonAngles(smallestAngles)) continue;
    fParticleSel.GetPhotons().AddTrack(*fTrackIter);
  }
}

/// Find angle differences with nearest charged track.
AngleDifferences TrackSelector::FindSmallestPhotonAngles()
{
  GetEmcPosition();
  AngleDifferences smallestAngles;
  for(vector<EvtRecTrack*>::iterator it = fChargedTracks.begin(); it != fChargedTracks.end(); ++it)
  {
    if(!GetExtendedEmcPosition(*it)) continue;
    AngleDifferences angles(fExtendedEmcPosition, fEmcPosition);
    if(angles.IsSmaller(smallestAngles)) smallestAngles = angles;
  }
  return smallestAngles;
}

void TrackSelector::GetEmcPosition()
{
  fTrackEMC    = (*fTrackIter)->emcShower();
  fEmcPosition = fTrackEMC->position();
}

bool TrackSelector::GetExtendedEmcPosition(EvtRecTrack* track)
{
  if(!track->isExtTrackValid()) return false;
  fTrackExt = track->extTrack();
  if(fTrackExt->emcVolumeNumber() == -1) return false;
  fExtendedEmcPosition = fTrackExt->emcPosition();
  return true;
}

/// *Write* photon info (`"photon"` branch).
void TrackSelector::WritePhotonKinematics(const AngleDifferences& angles)
{
  if(!fNTuple_photon.DoWrite()) return;
  double eraw  = fTrackEMC->energy();
  double phi   = fTrackEMC->phi();
  double theta = fTrackEMC->theta();

  HepLorentzVector four_mom(eraw * sin(theta) * cos(phi), eraw * sin(theta) * sin(phi),
                            eraw * cos(theta), eraw);
  fNTuple_photon.GetItem<double>("E")              = four_mom.e();
  fNTuple_photon.GetItem<double>("px")             = four_mom.px();
  fNTuple_photon.GetItem<double>("py")             = four_mom.py();
  fNTuple_photon.GetItem<double>("pz")             = four_mom.pz();
  fNTuple_photon.GetItem<double>("smallest_phi")   = angles.GetTheta();
  fNTuple_photon.GetItem<double>("smallest_theta") = angles.GetPhi();
  fNTuple_photon.GetItem<double>("smallest_angle") = angles.GetAngle();
  fNTuple_photon.Write();
}

/// Apply angle cut on the photons: you do not want to photons to be too close to any charged track to avoid noise from EMC showers that came from a charged track.
bool TrackSelector::CutPhotonAngles(const AngleDifferences& angles)
{
  if(!fCut_GammaAngle.FailsCut(angles.GetAbsoluteAngle())) return false;
  if(!fCut_GammaPhi.FailsCut(angles.GetAbsolutePhi())) return false;
  if(!fCut_GammaTheta.FailsCut(angles.GetAbsoluteTheta())) return false;
  return true;
}