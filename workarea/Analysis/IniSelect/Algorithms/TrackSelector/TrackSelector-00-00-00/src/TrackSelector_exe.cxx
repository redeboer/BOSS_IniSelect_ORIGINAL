#include "TrackSelector/TrackSelector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/Event.h"
#include "EventModel/EventModel.h"
#include "GaudiKernel/Bootstrap.h"
#include "IniSelect/Containers/NTupleTopoAna.h"
#include "IniSelect/Handlers/ParticleIdentifier.h"
#include "IniSelect/Globals.h"
#include "TMath.h"
#include "TString.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"
#include <cmath>

using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using namespace IniSelect;

/// This method is called **for each event**.
StatusCode TrackSelector::execute()
{
  PrintFunctionName("TrackSelector", __func__);
  try
  {
    LoadDstFile();
    SetVertexOrigin();
    CreateCollections();
    WriteVertexInfo();
    CutNumberOfChargedParticles();
    CreateChargedTrackSelections();
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

void TrackSelector::LoadDstFile()
{
  LoadDstHeaders();
  PrintEventInfo();
  IncrementCounters();
}

void TrackSelector::LoadDstHeaders()
{
  /// Load headers, event, and track collection from the DST input file.
  /// For more info see:
  /// * [Namespace `EventModel`](http://bes3.to.infn.it/Boss/7.0.2/html/namespaceEventModel_1_1EvtRec.html)
  /// * [Class `EvtRecEvent`](http://bes3.to.infn.it/Boss/7.0.2/html/classEvtRecEvent.html)
  /// * [Type definition `EvtRecTrackCol`](http://bes3.to.infn.it/Boss/7.0.2/html/EvtRecTrack_8h.html)
  /// * [Type definition`Event::McParticleCol`](http://bes3.to.infn.it/Boss/7.0.0/html/namespaceEvent.html#b6a28637c54f890ed93d8fd13d5021ed)
  fLog << MSG::DEBUG << "Loading EventHeader, EvtRecEvent, and EvtRecTrackCol" << endmsg;
  fEventHeader  = SmartDataPtr<Event::EventHeader>(eventSvc(), "/Event/EventHeader");
  fEvtRecEvent  = SmartDataPtr<EvtRecEvent>(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  fEvtRecTrkCol = SmartDataPtr<EvtRecTrackCol>(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
}

void TrackSelector::PrintEventInfo()
{
  /// Log run number, event number, and number of events.
  fLog << MSG::DEBUG << "RUN " << fEventHeader->runNumber() << ", "
       << "event number " << fEventHeader->eventNumber() << endmsg;
  fLog << MSG::DEBUG << "Ncharged = " << fEvtRecEvent->totalCharged() << ", "
       << "Nneutral = " << fEvtRecEvent->totalNeutral() << ", "
       << "Ntotal = " << fEvtRecEvent->totalTracks() << endmsg;
}

void TrackSelector::IncrementCounters()
{
  ++fCutFlow_Nevents;
  fCounter_Ncharged += fEvtRecEvent->totalCharged();
  fCounter_Nneutral += fEvtRecEvent->totalNeutral();
  fCounter_Ntracks += fEvtRecEvent->totalTracks();
}

void TrackSelector::SetVertexOrigin()
{
  IVertexDbSvc* vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(!vtxsvc->isVertexValid()) return;
  double* dbv = vtxsvc->PrimaryVertex();
  // double* vv  = vtxsvc->SigmaPrimaryVertex();
  fVertexPoint.set(dbv[0], dbv[1], dbv[2]);
}

void TrackSelector::CreateCollections()
{
  CreateChargedCollection(); /// -# Perform `CreateChargedCollection`.
  CreateNeutralCollection(); /// -# Perform `CreateNeutralCollection`.
  /// @remark You should call `CreateMCtruthCollection` in the derived class.
}

/// Create a preselection of charged tracks (without cuts).
/// This method is used in `TrackSelector::execute` only. See `fChargedTracks` for more information.
void TrackSelector::CreateChargedCollection()
{
  if(!fCreateChargedCollection) return;
  fChargedTracks.clear();
  if(!fEvtRecEvent->totalCharged()) return;
  /// -# Loop over the charged tracks in the loaded `fEvtRecEvent` track collection. The first part of the set of reconstructed tracks are the charged tracks.
  fNetChargeMDC = 0;
  fLog << MSG::DEBUG << "Starting 'good' charged track selection:" << endmsg;
  for(int i = 0; i < fEvtRecEvent->totalCharged(); ++i)
  {
    SetTrackIter(i);
    if(!IsMdcTrackValid()) continue;
    fTrackMDC = (*fTrackIter)->mdcTrack();
    fSecondaryVtx.SetValues(fTrackMDC, fVertexPoint);
    if(!CutSecondaryVertex()) continue;

    // * Add charged track to vector
    fChargedTracks.push_back(*fTrackIter);
    fNetChargeMDC += fTrackMDC->charge();
    WriteChargedTrackVertex();

    /// -# **Write** dE/dx PID information ("dedx" branch)
    WriteDedxInfo(*fTrackIter, fNTuple_dedx);

    /// -# **Write** Time-of-Flight PID information ("tof*" branch)
    if(fNTuple_TofEC.DoWrite() || fNTuple_TofIB.DoWrite() || fNTuple_TofOB.DoWrite())
    {
      // * Check if MDC and TOF info for track are valid *
      if(!(*fTrackIter)->isMdcTrackValid()) continue;
      if(!(*fTrackIter)->isTofTrackValid()) continue;
      // * Get momentum as determined by MDC *
      fTrackMDC = (*fTrackIter)->mdcTrack();
      double ptrk;
      if(fTrackMDC) ptrk = fTrackMDC->p();
      SmartRefVector<RecTofTrack>           tofTrkCol = (*fTrackIter)->tofTrack();
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

void TrackSelector::SetTrackIter(const int& i)
{
  fLog << MSG::DEBUG << "   charged track " << i << "/" << fEvtRecEvent->totalCharged() << endmsg;
  fTrackIter = fEvtRecTrkCol->begin() + i;
}

bool TrackSelector::IsMdcTrackValid()
{
  if(!(*fTrackIter)->isMdcTrackValid()) return false;
  ++fCounter_Nmdcvalid;
  return true;
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

  /// -# @b Write \f$dE/dx\f$ info.
  tuple.Write();
}

/// Helper method for writing Time-of-Flight information.
/// This function has be created to enable you to write TOF information for different collections of tracks.
void TrackSelector::WriteTofInformation(RecTofTrack* tofTrack, double ptrk, NTupleContainer& tuple)
{
  /// -# Abort if the 'write `JobSwitch`' has been set to `false`.
  if(!tuple.DoWrite()) return;

  /// -# Compute ToF for each particle hypothesis
  double              path = tofTrack->path();
  std::vector<double> texp(Mass::TOF.size());
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

  /// -# @b Write ToF info
  tuple.Write();
}

/// Create a preselection of @b neutral tracks (without cuts).
/// This method is used in `TrackSelector::execute` only. See `fNeutralTracks` for more information.
void TrackSelector::CreateNeutralCollection()
{
  /// <ol>
  /// <li> @b Abort if `fCreateNeutralCollection` has been set to `false`. This is decided in the derived class.
  if(!fCreateNeutralCollection) return;

  /// <li> Clear `fNeutralTracks` collection `vector`.
  fNeutralTracks.clear();

  /// <li> @ Abort if there are no charged tracks in the `fEvtRecEvent` track collection.
  if(!fEvtRecEvent->totalNeutral())

  /// <li> Loop over the neutral tracks in the loaded `fEvtRecEvent` track collection. The second part of the set of reconstructed events consists of the neutral tracks, that is, the photons detected by the EMC (by clustering EMC crystal energies).
  fLog << MSG::DEBUG << "Starting 'good' neutral track selection:" << endmsg;
  for(int i = fEvtRecEvent->totalCharged(); i < fEvtRecEvent->totalTracks(); ++i)
  {
    /// <ol>
    /// <li> Get EMC information
    fLog << MSG::DEBUG << "   neutral track " << i - fEvtRecEvent->totalCharged() << "/"
         << fEvtRecEvent->totalNeutral() << endmsg;
    fTrackIter = fEvtRecTrkCol->begin() + i;
    if(!(*fTrackIter)->isEmcShowerValid()) continue;
    fTrackEMC = (*fTrackIter)->emcShower();
    if(!fTrackEMC) continue;

    /// <li> Apply photon energy cut (set by `TrackSelector.cut_PhotonEnergy`).
    if(fCut_PhotonEnergy.FailsMin(fTrackEMC->energy())) continue;

    /// <li> @b Write neutral track information (if `write_neutral` is set to `true`).
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

    /// <li> Add photon track to vector of neutral tracks (`fNeutralTracks`).
    fNeutralTracks.push_back(*fTrackIter);

    /// </ol>
  }
  fLog << MSG::DEBUG << "Number of 'good' photons: " << fNeutralTracks.size() << endmsg;
  /// </ol>
}

void TrackSelector::WriteMultiplicities()
{
  if(!fNTuple_mult.DoWrite()) return;
  fNTuple_mult.GetItem<int>("Ntotal")   = fEvtRecEvent->totalTracks();
  fNTuple_mult.GetItem<int>("Ncharge")  = fEvtRecEvent->totalCharged();
  fNTuple_mult.GetItem<int>("Nneutral") = fEvtRecEvent->totalNeutral();
  if(fCreateChargedCollection) fNTuple_mult.GetItem<int>("NgoodCharged") = fChargedTracks.size();
  if(fCreateNeutralCollection) fNTuple_mult.GetItem<int>("NgoodNeutral") = fNeutralTracks.size();
  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    fNTuple_mult.GetItem<int>(Form("N_%s", coll->GetPdtName())) = coll->GetNTracks();
    coll = fParticleSel.NextCharged();
  }
  fNTuple_mult.Write();
}

void TrackSelector::PrintMultiplicities()
{
  fLog << MSG::INFO;
  Int_t                         i    = 0;
  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    if(i) fLog << ", ";
    fLog << Form("N_%s = ", coll->GetPdtName(), coll->GetNTracks());
    ++i;
    coll = fParticleSel.NextCharged();
  }
  fLog << endmsg;
}

/// **PID cut**: apply a strict cut on the number of the selected particles.
void TrackSelector::CutPID()
{
  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    if(coll->FailsMultiplicityCut()) throw StatusCode::SUCCESS;
    coll = fParticleSel.NextCharged();
  }
  ++fCutFlow_NPIDnumberOK;
  fLog << MSG::INFO << "PID selection passed for (run, event) = (" << fEventHeader->runNumber()
       << ", " << fEventHeader->eventNumber() << ")" << endmsg;
}

void TrackSelector::WriteVertexInfo()
{
  /// **Write** information about the interaction point (`"vertex"` branch).
  if(!fNTuple_vertex.DoWrite()) return;
  fNTuple_vertex.GetItem<double>("vx0") = fVertexPoint.x();
  fNTuple_vertex.GetItem<double>("vy0") = fVertexPoint.y();
  fNTuple_vertex.GetItem<double>("vz0") = fVertexPoint.z();
  fNTuple_vertex.Write();
}

void TrackSelector::CutNumberOfChargedParticles()
{
  if(fChargedTracks.size() != fParticleSel.GetNCharged()) throw StatusCode::SUCCESS;
  ++fCutFlow_NChargedOK;
}

void TrackSelector::CreateChargedTrackSelections()
{
  ParticleIdentifier::Initialize();
  ConfigurePID();

  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    ParticleIdentifier::SetParticleToIdentify(coll->GetPdgCode());
    coll = fParticleSel.NextCharged();
  }

  fParticleSel.ClearCharged();
  for(fTrackIter = fChargedTracks.begin(); fTrackIter != fChargedTracks.end(); ++fTrackIter)
  {
    std::string particleName = ParticleIdentifier::FindMostProbable(*fTrackIter, fCut_PIDProb);
    if(!fParticleSel.HasParticle(particleName)) continue;
    WritePIDInformation();
    fParticleSel.AddTrackToParticle(*fTrackIter, particleName);
  }
}

void TrackSelector::CreateNeutralTrackSelections()
{
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
  for(std::vector<EvtRecTrack*>::iterator it = fChargedTracks.begin(); it != fChargedTracks.end();
      ++it)
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