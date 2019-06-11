#include "JpsiToDPV/JpsiToDPV_new.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "IniSelect/Exceptions/Exception.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Particle/ParticleDatabase.h"
#include "IniSelect/Handlers/PID.h"
#include "IniSelect/Handlers/TrackCollectionNew.h"
#include "TError.h"
#include "TFile.h"
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

DECLARE_ALGORITHM_FACTORY(JpsiToDPV_new)

// * Typedefs * //
typedef vector<HepLorentzVector> Vp4;

JpsiToDPV_new::JpsiToDPV_new(const string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  fLog(msgSvc(), this->name()),
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
  declareProperty("MaxChiSq", fMaxChiSq);
  // declareProperty("MinPID", fMinPID);

  /// -# Whether or not to check success of Particle Identification.
  DECLAREWRITE(fD0omega.K4pi.tree);
  DECLAREWRITE(fD0phi.KpiKK.tree);
  DECLAREWRITE(fTrees.v);
  DECLAREWRITE(fTrees.photon);
  DECLAREWRITE(fTrees.dedx);
  DECLAREWRITE(fTrees.TofEC);
  DECLAREWRITE(fTrees.TofIB);
  DECLAREWRITE(fTrees.TofOB);
  DECLAREWRITE(fTrees.PID);
  fTrees.cuts.write = true;
}

StatusCode JpsiToDPV_new::initialize()
{}

/// Inherited `execute` method of the `Algorithm`. This function is called *for each event*.
StatusCode JpsiToDPV_new::execute()
{
  try
  {
    fLog << MSG::INFO << "In execute():" << endmsg;

    /// <ol>
    /// <li> Load next event from DST file

    // * Load DST file info *
    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(), "/Event/EventHeader");
    fLog << MSG::DEBUG << "run, evtnum = " << eventHeader->runNumber() << " , " << eventHeader->eventNumber() << endmsg;
    fTrees.cuts[0]++; // counter for all events

    // * Load event information and track collection *
    /*
      http://bes3.to.infn.it/Boss/7.0.2/html/namespaceEventModel_1_1EvtRec.html (namespace)
      http://bes3.to.infn.it/Boss/7.0.2/html/classEvtRecEvent.html (class)
      http://bes3.to.infn.it/Boss/7.0.2/html/EvtRecTrack_8h.html (typedef EvtRecTrackCol)
    */
    SmartDataPtr<EvtRecEvent>    evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
    SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");

    // * Reset values * //
    TrackCollectionNew::Reset();

    // * Log number of events *
    fLog << MSG::DEBUG << "Ncharged, Nneutral, Ntotal = " << evtRecEvent->totalCharged() << ", "
        << evtRecEvent->totalNeutral() << ", " << evtRecEvent->totalTracks() << endmsg;

    /// <li> Set vertex origin
    Hep3Vector    xorigin(0, 0, 0);
    IVertexDbSvc* vtxsvc;
    Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
    if(vtxsvc->isVertexValid())
    {
      double* dbv = vtxsvc->PrimaryVertex();
      double* vv  = vtxsvc->SigmaPrimaryVertex();
      // HepVector dbv = fReader.PrimaryVertex(eventHeader->runNumber());
      // HepVector vv  = fReader.SigmaPrimaryVertex(eventHeader->runNumber());
      xorigin.setX(dbv[0]);
      xorigin.setY(dbv[1]);
      xorigin.setZ(dbv[2]);
    }

    /// <li> Select good charged tracks.
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
      TrackCollectionNew::charged.push_back(trk);
      netCharge += mdcTrk->charge();
    }

    /// **Apply cut**: cut number of charged tracks and net charge
    fLog << MSG::DEBUG << "ngood, totcharge = " << TrackCollectionNew::charged.size() << " , " << netCharge
        << endmsg;
    if(TrackCollectionNew::charged.size() != 4) return StatusCode::SUCCESS;
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
        thed        = fmod(thed + Math::fivepi, Math::twopi) - Math::pi;
        phid        = fmod(phid + Math::fivepi, Math::twopi) - Math::pi;
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
      fTrees.photon.dthe = fTrees.photon.dthe * Math::RadToDeg;
      fTrees.photon.dphi = fTrees.photon.dphi * Math::RadToDeg;
      fTrees.photon.dang = fTrees.photon.dang * Math::RadToDeg;

      // * WRITE photon info ("photon" branch)
      fTrees.photon.Fill();

      // * Apply photon cuts
      if(fTrees.photon.eraw < fEnergyThreshold) continue;
      if((fabs(fTrees.photon.dthe) < fGammaThetaCut) && (fabs(fTrees.photon.dphi) < fGammaPhiCut))
        continue;
      if(fabs(fTrees.photon.dang) < fGammaAngleCut) continue;

      // * Add photon track to vector
      TrackCollectionNew::photon.push_back(trk);
    }
    fLog << MSG::DEBUG << "Number of good photons: " << TrackCollectionNew::photon.size() << "/"
        << evtRecEvent->totalNeutral() << endmsg;

    /// <li> Check charged track \f$dE/dx\f$ information.
    fTrees.dedx.Fill(TrackCollectionNew::charged);

    /// <li> Check charged track ToF PID information.
    TreeToF::WriteToF(TrackCollectionNew::charged, fTrees.TofEC, fTrees.TofIB, fTrees.TofOB);
    if(fTrees.TofEC.write || fTrees.TofIB.write || fTrees.TofOB.write)
    {
      for(auto trk : TrackCollectionNew::charged)
      {
        if(!trk->isMdcTrackValid()) continue;
        if(!trk->isTofTrackValid()) continue;

        RecMdcTrack*                mdcTrk    = trk->mdcTrack();
        SmartRefVector<RecTofTrack> tofTrkCol = trk->tofTrack();

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
            else if(hitStatus.layer() == 2)
              fTrees.TofOB.Fill(*it_tof, ptrk);
          }
        }
      }
    }

    /// <li> Perform PID on charged tracks.
    for(auto trk : TrackCollectionNew::charged)
    {
      PID::Initialize();

      PID::UseProbabilityMethod();
      PID::SetMinimalChi2(4.);

      PID::UseDedx();
      PID::UseTofIB();
      PID::UseTofOB();
      PID::UseTofE();

      PID::SetIdentifyPion();
      PID::SetIdentifyKaon();

      TrackCollectionNew::PushBack(trk);
    }
  cout << TrackCollectionNew::K.neg.size() << "\t";
  cout << TrackCollectionNew::pi.neg.size() << "\t";
  cout << TrackCollectionNew::pi.pos.size() << endl;

    /// <li> Perform study in case of \f$J/\psi \rightarrow K^-\pi^+\pi^-\pi^+\gamma\gamma\f$ (`D0omega_K4pi`).
    if((TrackCollectionNew::K.neg.size() == 1) && (TrackCollectionNew::pi.neg.size() == 1) && (TrackCollectionNew::pi.pos.size() == 2))
    {
      fLog << MSG::DEBUG << "PID: K- pi+ pi- pi+" << endmsg;
      fTrees.cuts[3]++;
      if(TrackCollectionNew::photon.size() < 2) return StatusCode::SUCCESS;
      fLog << MSG::DEBUG << "     >2 gammas" << endmsg;
      fTrees.cuts[4]++;
      fD0omega.K4pi.DoFit(fMaxChiSq);
      fLog << MSG::DEBUG << "     D0omega_K4pi fit successful" << endmsg;
      fTrees.cuts[5]++;
      fD0omega.K4pi.tree.runid = eventHeader->runNumber();
      fD0omega.K4pi.tree.evtid = eventHeader->eventNumber();
      if(eventHeader->runNumber() < 0) fD0omega.K4pi.tree.SetMC(mcParticleCol);
      fD0omega.K4pi.tree.Fill();
    }

    /// <li> Perform study in case of \f$J/\psi \rightarrow K^-\pi^+K^-K^+\f$ (`D0phi_KpiKK`).
    if((TrackCollectionNew::K.neg.size() == 2) && (TrackCollectionNew::K.pos.size() == 1) && (TrackCollectionNew::pi.pos.size() == 1))
    {
      fLog << MSG::DEBUG << "PID: K- pi+ K- K+" << endmsg;
      fTrees.cuts[6]++;
      fD0phi.KpiKK.DoFit(fMaxChiSq);
      fLog << MSG::DEBUG << "     D0phi_KpiKK fit successful" << endmsg;
      fTrees.cuts[7]++;
      fD0phi.KpiKK.tree.runid = eventHeader->runNumber();
      fD0phi.KpiKK.tree.evtid = eventHeader->eventNumber();
      if(eventHeader->runNumber() < 0) fD0phi.KpiKK.tree.SetMC(mcParticleCol);
      fD0phi.KpiKK.tree.Fill();
    }
  }
  catch(const NotPidInfoValid& e)
  {
    cout << "NotPidInfoValid: " << e.what() << endl;
  }
  catch(const Exception& e)
  {
    cout << "Exception: " << e.what() << endl;
  }
  /// </ol>
  return StatusCode::SUCCESS;
}

/// Inherited `finalize` method of `Algorithm`. This function is only called once, after running over all events. Prints the flow chart to the terminal, so **make sure you save this output!**
StatusCode JpsiToDPV_new::finalize()
{
  fLog << MSG::INFO << "in finalize()" << endmsg;

  fTrees.cuts.Fill();
  cout << endl;
  cout << "Resulting flow chart:" << endl;
  cout << "  Total number of events: " << fTrees.cuts[0] << endl;
  cout << "  Pass N charged tracks:  " << fTrees.cuts[1] << endl;
  cout << "  Pass zero net charge:   " << fTrees.cuts[2] << endl;
  if(fD0omega.K4pi.tree.write)
  {
    cout << "  D0omega study:" << endl;
    cout << "    Pass K- pi+ pi- pi+: " << fTrees.cuts[3] << endl;
    cout << "    Pass >2 gammas:      " << fTrees.cuts[4] << endl;
    cout << "    Pass Kalman fit:     " << fTrees.cuts[5] << endl;
  }
  if(fD0phi.KpiKK.tree.write)
  {
    cout << "  D0phi study:" << endl;
    cout << "    Pass K- pi+ K- K+: " << fTrees.cuts[6] << endl;
    cout << "    Pass Kalman fit:   " << fTrees.cuts[7] << endl;
  }
  cout << endl;
  cout << "Trees:" << endl;
  if(fD0omega.K4pi.tree.write)
    cout << "  " << fD0omega.K4pi.tree.GetName() << ":    " << fD0omega.K4pi.tree.GetEntries() << " events" << endl;
  if(fD0phi.KpiKK.tree.write)
    cout << "  " << fD0phi.KpiKK.tree.GetName() << ":    " << fD0phi.KpiKK.tree.GetEntries() << " events" << endl;
  cout << "  ------------------" << endl;
  if(fTrees.v.write) cout << "  v:      " << fTrees.v.GetEntries() << " tracks" << endl;
  if(fTrees.photon.write) cout << "  photon: " << fTrees.photon.GetEntries() << " tracks" << endl;
  if(fTrees.dedx.write) cout << "  dedx:   " << fTrees.dedx.GetEntries() << " tracks" << endl;
  if(fTrees.TofEC.write) cout << "  TofEC:  " << fTrees.TofEC.GetEntries() << " tracks" << endl;
  if(fTrees.TofIB.write) cout << "  TofIB:  " << fTrees.TofIB.GetEntries() << " tracks" << endl;
  if(fTrees.TofOB.write) cout << "  TofOB:  " << fTrees.TofOB.GetEntries() << " tracks" << endl;
  if(fTrees.PID.write) cout << "  PID:    " << fTrees.PID.GetEntries() << " tracks" << endl;
  if(fTrees.cuts.write) cout << "  (cuts:  " << fTrees.cuts.GetEntries() << " entry)" << endl;
  cout << endl;

  TFile file(fFileName.c_str(), "RECREATE");
  if(!file.IsOpen())
  {
    fLog << MSG::ERROR << "Failed to load output file \"" << fFileName << "\"" << endmsg;
    return StatusCode::FAILURE;
  }
  if(file.IsZombie())
  {
    fLog << MSG::ERROR << "Output file \"" << fFileName << "\" is zombie" << endmsg;
    return StatusCode::FAILURE;
  }
  fD0omega.K4pi.tree.Write();
  fD0phi.KpiKK.tree.Write();
  fTrees.v.Write();
  fTrees.photon.Write();
  fTrees.dedx.Write();
  fTrees.TofEC.Write();
  fTrees.TofIB.Write();
  fTrees.TofOB.Write();
  fTrees.PID.Write();
  fTrees.cuts.Write();
  file.Close();

  fLog << MSG::INFO << "Successfully returned from finalize()" << endmsg;
  return StatusCode::SUCCESS;
}