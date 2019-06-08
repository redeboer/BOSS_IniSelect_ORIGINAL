#include "JpsiToDPV/Globals.h"
#include "JpsiToDPV/JpsiToDPV.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "TError.h"
#include "TFile.h"
#include "TMath.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"
#include <vector>
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using namespace std;
using namespace IniSelect;

// * Typedefs * //
typedef vector<HepLorentzVector> Vp4;

ParticleID*         JpsiToDPV::pid    = ParticleID::instance();
VertexFit*          JpsiToDPV::vtxfit = VertexFit::instance();
KalmanKinematicFit* JpsiToDPV::kkmfit = KalmanKinematicFit::instance();

JpsiToDPV::JpsiToDPV(const string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  log(msgSvc(), this->name())
{
  /// -# Suppres ROOT error messages.
  /// @todo Check whether the "memory-resident" `TTree` is reliable.
  gErrorIgnoreLevel = 6000;
  declareProperty("OutFile", fFileName);

  /// -# Define r0, z0 cut for charged tracks.
  declareProperty("Vr0cut", fVr0cut);
  declareProperty("Vz0cut", fVz0cut);
  declareProperty("Vrvz0cut", fRvz0cut);
  declareProperty("Vrvxy0cut", fRvxy0cut);

  /// -# Define energy, dphi, dthe cuts for fake gamma's.
  declareProperty("EnergyThreshold", fEnergyThreshold);
  declareProperty("GammaPhiCut", fGammaPhiCut);
  declareProperty("GammaThetaCut", fGammaThetaCut);
  declareProperty("GammaAngleCut", fGammaAngleCut);

  /// -# Whether to test the success of the 4- and 5-constraint fits.
  declareProperty("DoKKFit", fDo_fit);
  declareProperty("MaxChiSq", fMaxChiSq);
  declareProperty("MinPID", fMinPID);

  /// -# Which particles to identify
  declareProperty("IdentifyElectron", fPID_e);
  declareProperty("IdentifyKaon", fPID_K);
  declareProperty("IdentifyPion", fPID_pi);

  /// -# Whether or not to check success of Particle Identification.
  DECLAREWRITE(fD0omega.K4pi.fit);
  DECLAREWRITE(fD0omega.K4pi.MC);
  DECLAREWRITE(fTrees.v);
  DECLAREWRITE(fTrees.photon);
  DECLAREWRITE(fTrees.dedx);
  DECLAREWRITE(fTrees.TofEC);
  DECLAREWRITE(fTrees.TofIB);
  DECLAREWRITE(fTrees.TofOB);
  DECLAREWRITE(fTrees.PID);
  fTrees.cuts.write = true;
}

StatusCode JpsiToDPV::initialize()
{}

/// Inherited `execute` method of the `Algorithm`. This function is called *for each event*.
StatusCode JpsiToDPV::execute()
{
  log << MSG::INFO << "In execute():" << endmsg;

  /// <ol>
  /// <li> Load next event from DST file

  // * Load DST file info *
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(), "/Event/EventHeader");
  int                              runNo = eventHeader->runNumber();
  int                              evtNo = eventHeader->eventNumber();
  log << MSG::DEBUG << "run, evtnum = " << runNo << " , " << evtNo << endmsg;
  fTrees.cuts[0]++; // counter for all events

  // * Load event information and track collection *
  /*
    http://bes3.to.infn.it/Boss/7.0.2/html/namespaceEventModel_1_1EvtRec.html (namespace)
    http://bes3.to.infn.it/Boss/7.0.2/html/classEvtRecEvent.html (class)
    http://bes3.to.infn.it/Boss/7.0.2/html/EvtRecTrack_8h.html (typedef EvtRecTrackCol)
  */
  SmartDataPtr<EvtRecEvent>    evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);

  // * Reset values * //
  fD0omega.K4pi.MC.Reset();
  tracks.Reset();

  // * Log number of events *
  log << MSG::DEBUG << "Ncharged, Nneutral, Ntotal = " << evtRecEvent->totalCharged() << ", "
      << evtRecEvent->totalNeutral() << ", " << evtRecEvent->totalTracks() << endmsg;

  /// <li> Set vertex origin
  Hep3Vector    xorigin(0, 0, 0);
  IVertexDbSvc* vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid())
  {
    double* dbv = vtxsvc->PrimaryVertex();
    double* vv  = vtxsvc->SigmaPrimaryVertex();
    // HepVector dbv = fReader.PrimaryVertex(runNo);
    // HepVector vv  = fReader.SigmaPrimaryVertex(runNo);
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);
  }

  /// <li> LOOP OVER CHARGED TRACKS: select charged tracks (eventual pions)
  int netCharge = 0;
  for(int i = 0; i < evtRecEvent->totalCharged(); ++i)
  {
    // * Get track: beginning of all tracks + event number
    EvtRecTrack* trk = *(evtRecTrkCol->begin() + i);
    if(!trk->isMdcTrackValid()) continue;

    // * WRITE primary vertex position info ("vxyz" branch) *
    RecMdcTrack* mdcTrk = trk->mdcTrack();
    fTrees.v.Fill(xorigin, mdcTrk);

    // * Apply vertex cuts *
    if(fabs(fTrees.v.z) >= fVz0cut) continue;
    if(fabs(fTrees.v.rxy) >= fVr0cut) continue;
    if(fabs(fTrees.v.rz) >= fRvz0cut) continue;
    if(fabs(fTrees.v.rxy) >= fRvxy0cut) continue;

    // * Add charged track to vector *
    tracks.charged.push_back(trk);
    netCharge += mdcTrk->charge();
  }

  /// **Apply cut**: cut number of charged tracks and net charge
  log << MSG::DEBUG << "ngood, totcharge = " << tracks.charged.size() << " , " << netCharge
      << endmsg;
  if(tracks.charged.size() != 4) return StatusCode::SUCCESS;
  fTrees.cuts[1]++;
  if(netCharge != 0) return StatusCode::SUCCESS;
  fTrees.cuts[2]++;

  /// <li> LOOP OVER NEUTRAL TRACKS: select photons
  for(int i = evtRecEvent->totalCharged(); i < evtRecEvent->totalTracks(); ++i)
  {
    // * Get track
    EvtRecTrack* trk = *(evtRecTrkCol->begin() + i);
    if(!trk->isEmcShowerValid()) continue;
    RecEmcShower* emcTrk = trk->emcShower();
    Hep3Vector    emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());

    // * Find the theta, phi, and angle difference with nearest charged track
    fTrees.photon.dthe = 200.; // start value for difference in theta
    fTrees.photon.dphi = 200.; // start value for difference in phi
    fTrees.photon.dang = 200.; // start value for difference in angle (?)
    for(int j = 0; j < evtRecEvent->totalCharged(); j++)
    {
      EvtRecTrack* trk2 = *(evtRecTrkCol->begin() + j);
      if(!trk2->isExtTrackValid()) continue;
      RecExtTrack* extTrk = trk2->extTrack();
      if(extTrk->emcVolumeNumber() == -1) continue;
      Hep3Vector extpos = extTrk->emcPosition();
      // double ctht = extpos.cosTheta(emcpos);
      double angd = extpos.angle(emcpos);
      double thed = extpos.theta() - emcpos.theta();
      double phid = extpos.deltaPhi(emcpos);
      thed        = fmod(thed + fivepi, CLHEP::twopi) - CLHEP::pi;
      phid        = fmod(phid + fivepi, CLHEP::twopi) - CLHEP::pi;
      if(angd < fTrees.photon.dang)
      {
        fTrees.photon.dang = angd;
        fTrees.photon.dthe = thed;
        fTrees.photon.dphi = phid;
      }
    }

    // * Apply angle cut
    if(fTrees.photon.dang >= 200) continue;
    fTrees.photon.eraw = emcTrk->energy();
    fTrees.photon.dthe = fTrees.photon.dthe * DegToRad;
    fTrees.photon.dphi = fTrees.photon.dphi * DegToRad;
    fTrees.photon.dang = fTrees.photon.dang * DegToRad;

    // * WRITE photon info ("photon" branch)
    fTrees.photon.Fill();

    // * Apply photon cuts
    if(fTrees.photon.eraw < fEnergyThreshold) continue;
    if((fabs(fTrees.photon.dthe) < fGammaThetaCut) && (fabs(fTrees.photon.dphi) < fGammaPhiCut))
      continue;
    if(fabs(fTrees.photon.dang) < fGammaAngleCut) continue;

    // * Add photon track to vector
    tracks.photon.push_back(trk);
  }

  // * Finish Good Photon Selection *
  log << MSG::DEBUG << "Number of good photons: " << tracks.photon.size() << "/"
      << evtRecEvent->totalNeutral() << endmsg;
  if(tracks.photon.size() < 2) return StatusCode::SUCCESS;
  fTrees.cuts[3]++;

  fTrees.dedx.Fill(tracks.charged);

  /// <li> Check charged track ToF PID information
  TreeToF::WriteToF(tracks.charged, fTrees.TofEC, fTrees.TofIB, fTrees.TofOB);
  if(fTrees.TofEC.write || fTrees.TofIB.write || fTrees.TofOB.write)
  {
    vector<EvtRecTrack*>::iterator it = tracks.charged.begin();
    for(; it != tracks.charged.end(); ++it)
    {
      if(!(*it)->isMdcTrackValid()) continue;
      if(!(*it)->isTofTrackValid()) continue;

      RecMdcTrack*                mdcTrk    = (*it)->mdcTrack();
      SmartRefVector<RecTofTrack> tofTrkCol = (*it)->tofTrack();

      double ptrk = mdcTrk->p();

      SmartRefVector<RecTofTrack>::iterator it_tof = tofTrkCol.begin();
      for(; it_tof != tofTrkCol.end(); ++it_tof)
      {
        TofHitStatus hitStatus;
        hitStatus.setStatus((*it_tof)->status());
        if(!hitStatus.is_barrel())
        {
          if(!(hitStatus.is_counter())) continue; // ?
          if(hitStatus.layer() == 0) fTrees.TofEC.Fill(*it_tof, ptrk);
        }
        else
        {
          if(!hitStatus.is_counter()) continue;
          if(hitStatus.layer() == 1)
            fTrees.TofIB.Fill(*it_tof, ptrk);
          else
            if(hitStatus.layer() == 2) fTrees.TofOB.Fill(*it_tof, ptrk);
        }
      }
    } // loop all charged track
  }

  /// <li> Get 4-momentum for each charged track
  vector<EvtRecTrack*>::iterator it = tracks.charged.begin();
  for(; it != tracks.charged.end(); ++it)
  {
    pid->init();
    pid->setMethod(pid->methodProbability());
    // pid->setMethod(pid->methodLikelihood()); // for Likelihood Method

    pid->setChiMinCut(4);
    pid->setRecTrack(*it);
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2() | pid->useTofE());
    if(fPID_pi) pid->identify(pid->onlyPion());
    if(fPID_K) pid->identify(pid->onlyKaon());
    if(fPID_e) pid->identify(pid->onlyElectron());
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;

    // * WRITE particle identification info ("pid" branch) *
    fTrees.PID.Fill(pid, *it);

    RecMdcKalTrack* mdcKalTrk = (*it)->mdcKalTrack();
    if(fPID_pi && pid->probPion() > pid->probKaon() && pid->probPion() > pid->probElectron())
    {
      if(pid->probPion() < fMinPID) continue;
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
      if(mdcKalTrk->charge() < 0)
        tracks.pim.push_back(*it);
      else
        tracks.pip.push_back(*it);
    }
    else
    {
      if(fPID_K && pid->probKaon() > pid->probElectron())
      {
        if(pid->probKaon() < fMinPID) continue;
        RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);
        if(mdcKalTrk->charge() < 0)
          tracks.Km.push_back(*it);
        else
          tracks.Kp.push_back(*it);
      }
      else if(fPID_e)
      {
        if(pid->probElectron() < fMinPID) continue;
        RecMdcKalTrack::setPidType(RecMdcKalTrack::electron);
        if(mdcKalTrk->charge() < 0)
          tracks.em.push_back(*it);
        else
          tracks.ep.push_back(*it);
      }
    }
  }

  /// **Apply cut**: PID
  if(tracks.Km.size() != 1) return SUCCESS;
  if(tracks.pim.size() != 1) return SUCCESS;
  if(tracks.pip.size() != 2) return SUCCESS;
  fTrees.cuts[4]++;

  RecMdcKalTrack* KmTrk   = tracks.Km[0]->mdcKalTrack();
  RecMdcKalTrack* pimTrk  = tracks.pim[0]->mdcKalTrack();
  RecMdcKalTrack* pip1Trk = tracks.pip[0]->mdcKalTrack();
  RecMdcKalTrack* pip2Trk = tracks.pip[1]->mdcKalTrack();

  WTrackParameter wvKmTrk(mK, KmTrk->getZHelix(), KmTrk->getZError());
  WTrackParameter wvpimTrk(mpi, pimTrk->getZHelix(), pimTrk->getZError());
  WTrackParameter wvpip1Trk(mpi, pip1Trk->getZHelix(), pip1Trk->getZError());
  WTrackParameter wvpip2Trk(mpi, pip2Trk->getZHelix(), pip2Trk->getZError());

  /// <li> Get vertex fit
  HepPoint3D   vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  double       bx = 1E+6;
  double       by = 1E+6;
  double       bz = 1E+6;
  Evx[0][0]       = bx * bx;
  Evx[1][1]       = by * by;
  Evx[2][2]       = bz * bz;

  VertexParameter vxpar;
  vxpar.setVx(vx);
  vxpar.setEvx(Evx);

  vtxfit->init();
  vtxfit->AddTrack(0, wvKmTrk);
  vtxfit->AddTrack(1, wvpimTrk);
  vtxfit->AddTrack(2, wvpip1Trk);
  vtxfit->AddTrack(3, wvpip2Trk);
  vtxfit->AddVertex(0, vxpar, 0, 1);
  if(!vtxfit->Fit(0)) return SUCCESS;
  vtxfit->Swim(0);

  WTrackParameter wKm   = vtxfit->wtrk(0);
  WTrackParameter wpim  = vtxfit->wtrk(1);
  WTrackParameter wpip1 = vtxfit->wtrk(2);
  WTrackParameter wpip2 = vtxfit->wtrk(3);

  /// <li> Perform Kalman kinematic fit.
  if(fDo_fit) if(fD0omega.K4pi.DoFit(fMaxChiSq, tracks)) fTrees.cuts[6]++;

  /// <li> Get MC truth.
  bool writeMC = (fD0omega.K4pi.MC.momega < 100.);
  if(fD0omega.K4pi.MC.write && eventHeader->runNumber() < 0 && writeMC)
  {
    fD0omega.K4pi.MC.runid = eventHeader->runNumber();
    fD0omega.K4pi.MC.evtid = eventHeader->eventNumber();
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
    fD0omega.K4pi.MC.Fill(mcParticleCol);
  }

  /// </ol>
  return StatusCode::SUCCESS;
}

/// Inherited `finalize` method of `Algorithm`. This function is only called once, after running over all events. Prints the flow chart to the terminal, so **make sure you save this output!**
StatusCode JpsiToDPV::finalize()
{
  log << MSG::INFO << "in finalize()" << endmsg;

  fTrees.cuts.Fill();
  cout << endl;
  cout << "Resulting flow chart:" << endl;
  cout << "  Total number of events: " << fTrees.cuts[0] << endl;
  cout << "  Pass N charged tracks:  " << fTrees.cuts[1] << endl;
  cout << "  Pass zero net charge    " << fTrees.cuts[2] << endl;
  cout << "  Pass N gammas:          " << fTrees.cuts[3] << endl;
  cout << "  Pass PID:               " << fTrees.cuts[4] << endl;
  cout << "  Pass 4C Kalman fit:     " << fTrees.cuts[5] << endl;
  cout << "  Pass 5C Kalman fit:     " << fTrees.cuts[6] << endl;
  cout << endl;
  cout << "Trees:" << endl;
  if(fD0omega.K4pi.fit.write) cout << "  fit:  " << fD0omega.K4pi.fit.GetEntries() << endl;
  if(fD0omega.K4pi.MC.write) cout << "  MC:     " << fD0omega.K4pi.MC.GetEntries() << endl;
  if(fTrees.cuts.write) cout << "  cuts:   " << fTrees.cuts.GetEntries() << endl;
  if(fTrees.v.write) cout << "  v:      " << fTrees.v.GetEntries() << endl;
  if(fTrees.photon.write) cout << "  photon: " << fTrees.photon.GetEntries() << endl;
  if(fTrees.dedx.write) cout << "  dedx:   " << fTrees.dedx.GetEntries() << endl;
  if(fTrees.TofEC.write) cout << "  TofEC:  " << fTrees.TofEC.GetEntries() << endl;
  if(fTrees.TofIB.write) cout << "  TofIB:  " << fTrees.TofIB.GetEntries() << endl;
  if(fTrees.TofOB.write) cout << "  TofOB:  " << fTrees.TofOB.GetEntries() << endl;
  if(fTrees.PID.write) cout << "  PID:    " << fTrees.PID.GetEntries() << endl;
  cout << endl;

  TFile file(fFileName.c_str(), "RECREATE");
  if(!file.IsOpen())
  {
    log << MSG::ERROR << "Failed to load output file \"" << fFileName << "\"" << endmsg;
    return StatusCode::FAILURE;
  }
  if(file.IsZombie())
  {
    log << MSG::ERROR << "Output file \"" << fFileName << "\" is zombie" << endmsg;
    return StatusCode::FAILURE;
  }
  fD0omega.K4pi.fit.Write();
  fD0omega.K4pi.MC.Write();
  fTrees.v.Write();
  fTrees.photon.Write();
  fTrees.dedx.Write();
  fTrees.TofEC.Write();
  fTrees.TofIB.Write();
  fTrees.TofOB.Write();
  fTrees.PID.Write();
  fTrees.cuts.Write();
  file.Close();

  log << MSG::INFO << "Successfully returned from finalize()" << endmsg;
  return StatusCode::SUCCESS;
}