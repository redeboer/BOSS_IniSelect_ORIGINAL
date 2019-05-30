#include "JpsiToDPV/JpsiToDPV.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
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
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "McTruth/McParticle.h"
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

typedef vector<EvtRecTrack*>::iterator TrackIter;

// * ==================================== * //
// * ------- GLOBALS AND TYPEDEFS ------- * //
// * ==================================== * //

// * Constants * //
const double mD0    = 1.8648400;  // mass of D0
const double mpi0   = 0.13497700; // mass of pi0
const double mphi   = 1.0194550;  // mass of phi
const double momega = 0.78265000; // mass of omega

const double me  = 0.000511; // electron
const double mmu = 0.105658; // muon
const double mpi = 0.139570; // charged pion
const double mK  = 0.493677; // charged kaon
const double mp  = 0.938272; // proton

const double xmass[5] = {
  me,  // electron
  mmu, // muon
  mpi, // charged pion
  mK,  // charged kaon
  mp   // proton
};
const double velc_mm = 299.792458; // tof path unit in mm
const double Ecms    = 3.097;      // center-of-mass energy

const double DegToRad = 180. / (CLHEP::pi);
const double fivepi   = CLHEP::twopi + CLHEP::twopi + pi;

HepLorentzVector ecms(0.011 * Ecms, 0, 0, Ecms);

const int incPid1 = 91;
const int incPid2 = 92;
const int incPid  = 443;

// * Typedefs * //
typedef vector<HepLorentzVector> Vp4;

struct Reconstructed
{
  HepLorentzVector D0;
  HepLorentzVector omega;
  HepLorentzVector Jpsi;
};

struct LorentzVectors
{
  HepLorentzVector pim;
  HepLorentzVector pip1;
  HepLorentzVector pip2;
  HepLorentzVector Km;
  HepLorentzVector g1;
  HepLorentzVector g2;
  HepLorentzVector pi0;
  Reconstructed    comb1;
  Reconstructed    comb2;
};

LorentzVectors results;

ParticleID*         JpsiToDPV::pid    = ParticleID::instance();
VertexFit*          JpsiToDPV::vtxfit = VertexFit::instance();
KalmanKinematicFit* JpsiToDPV::kkmfit = KalmanKinematicFit::instance();

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //
/// Constructor for the `JpsiToDPV` algorithm.
/// Here, you should declare properties: give them a name, assign a parameter (data member of `JpsiToDPV`), and if required a documentation string. Note that you should define the paramters themselves in the header (JpsiToDPV/JpsiToDPV.h) and that you should assign the values in `share/jopOptions_JpsiToDPV.txt`. Algorithms should inherit from Gaudi's `Algorithm` class. See https://dayabay.bnl.gov/dox/GaudiKernel/html/classAlgorithm.html.
JpsiToDPV::JpsiToDPV(const string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  log(msgSvc(), this->name())
{
  declareProperty("OutFile", fFileName);

  // * Define r0, z0 cut for charged tracks *
  declareProperty("Vr0cut", fVr0cut);
  declareProperty("Vz0cut", fVz0cut);
  declareProperty("Vrvz0cut", fRvz0cut);
  declareProperty("Vrvxy0cut", fRvxy0cut);

  // * Define energy, dphi, dthe cuts for fake gamma's *
  declareProperty("EnergyThreshold", fEnergyThreshold);
  declareProperty("GammaPhiCut", fGammaPhiCut);
  declareProperty("GammaThetaCut", fGammaThetaCut);
  declareProperty("GammaAngleCut", fGammaAngleCut);

  // * Whether to test the success of the 4- and 5-constraint fits *
  declareProperty("Test4C", fDo_fit4c);   // write fit4c
  declareProperty("Test5C", fDo_fit5c);   // write fit5c and geff
  declareProperty("MaxChiSq", fMaxChiSq); // chisq for both fits should be less
  declareProperty("MinPID", fMinPID);     // PID probability should be at least this value

  // * Whether or not to check success of Particle Identification *
  DECLAREWRITE(fD0omega.K4pi.fit4c);
  DECLAREWRITE(fD0omega.K4pi.fit5c);
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

// * ========================== * //
// * ------- INITIALIZE ------- * //
// * ========================== * //
StatusCode JpsiToDPV::initialize()
{}

// * ========================= * //
// * -------- EXECUTE -------- * //
// * ========================= * //
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

    // * Get track info from Main Drift Chamber
    RecMdcTrack* mdcTrk = trk->mdcTrack();
    fTrees.v.p          = mdcTrk->p();
    fTrees.v.x          = mdcTrk->x();
    fTrees.v.y          = mdcTrk->y();
    fTrees.v.z          = mdcTrk->z();
    fTrees.v.phi        = mdcTrk->helix(1);

    // * Get vertex origin
    double xv  = xorigin.x();
    double yv  = xorigin.y();
    fTrees.v.r = (fTrees.v.x - xv) * cos(fTrees.v.phi) + (fTrees.v.y - yv) * sin(fTrees.v.phi);

    // * Get radii of vertex
    HepVector    a  = mdcTrk->helix();
    HepSymMatrix Ea = mdcTrk->err();
    HepPoint3D   point0(0., 0., 0.); // the initial point for MDC recosntruction
    HepPoint3D   IP(xorigin[0], xorigin[1], xorigin[2]);
    VFHelix      helixip(point0, a, Ea);
    helixip.pivot(IP);
    HepVector vecipa = helixip.a();
    fTrees.v.rxy     = fabs(vecipa[0]); // nearest distance to IP in xy plane
    fTrees.v.rz      = vecipa[3];       // nearest distance to IP in z direction
    fTrees.v.rphi    = vecipa[1];

    // * WRITE primary vertex position info ("vxyz" branch) *
    fTrees.v.Fill();

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

  /// <li> Check charged track dEdx PID information
  if(fTrees.dedx.write)
  {
    TrackIter it = tracks.charged.begin();
    for(; it != tracks.charged.end(); ++it)
    {
      if(!(*it)->isMdcTrackValid()) continue;
      if(!(*it)->isMdcDedxValid()) continue;
      RecMdcTrack* mdcTrk  = (*it)->mdcTrack();
      RecMdcDedx*  dedxTrk = (*it)->mdcDedx();

      fTrees.dedx.p      = mdcTrk->p();            // momentum of the track
      fTrees.dedx.chie   = dedxTrk->chiE();        // chi2 in case of electron
      fTrees.dedx.chimu  = dedxTrk->chiMu();       // chi2 in case of muon
      fTrees.dedx.chipi  = dedxTrk->chiPi();       // chi2 in case of pion
      fTrees.dedx.chik   = dedxTrk->chiK();        // chi2 in case of kaon
      fTrees.dedx.chip   = dedxTrk->chiP();        // chi2 in case of proton
      fTrees.dedx.probPH = dedxTrk->probPH();      // most probable pulse height from truncated mean
      fTrees.dedx.normPH = dedxTrk->normPH();      // normalized pulse height
      fTrees.dedx.ghit   = dedxTrk->numGoodHits(); // number of good hits
      fTrees.dedx.thit   = dedxTrk->numTotalHits(); // total number of hits
      fTrees.dedx.Fill();
    }
  }

  /// <li> Check charged track ToF PID information
  if(fTrees.TofEC.write || fTrees.TofIB.write || fTrees.TofOB.write)
  {
    TrackIter it = tracks.charged.begin();
    for(; it != tracks.charged.end(); ++it)
    {
      if(!(*it)->isMdcTrackValid()) continue;
      if(!(*it)->isTofTrackValid()) continue;

      RecMdcTrack*                mdcTrk    = (*it)->mdcTrack();
      SmartRefVector<RecTofTrack> tofTrkCol = (*it)->tofTrack();

      double ptrk = mdcTrk->p();

      SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
      for(; iter_tof != tofTrkCol.end(); ++iter_tof)
      {
        TofHitStatus hitStatus;
        hitStatus.setStatus((*iter_tof)->status());

        // * If end cap ToF detector: *
        if(!(hitStatus.is_barrel()))
        {
          // *  *
          if(!(hitStatus.is_counter())) continue; // ?
          if(hitStatus.layer() != 0) continue;    // abort if not end cap

          // * Get ToF info *
          fTrees.TofEC.p     = ptrk;
          fTrees.TofEC.path  = (*iter_tof)->path();         // distance of flight
          fTrees.TofEC.tof   = (*iter_tof)->tof();          // time of flight
          fTrees.TofEC.ph    = (*iter_tof)->ph();           // ToF pulse height
          fTrees.TofEC.zrhit = (*iter_tof)->zrhit();        // Track extrapolate Z or R Hit position
          fTrees.TofEC.qual  = 0. + (*iter_tof)->quality(); // data quality of reconstruction
          fTrees.TofEC.cntr  = 0. + (*iter_tof)->tofID();   // ToF counter ID

          // * Get ToF for each particle hypothesis *
          double texp[5];
          for(int j = 0; j < 5; j++)
          {
            double gb   = ptrk / xmass[j]; // v = p/m (non-relativistic velocity)
            double beta = gb / sqrt(1 + gb * gb);
            texp[j]     = 10 * fTrees.TofEC.path / beta / velc_mm; // hypothesis ToF
          }
          fTrees.TofEC.te =
            fTrees.TofEC.tof - texp[0]; // difference with ToF in electron hypothesis
          fTrees.TofEC.tmu = fTrees.TofEC.tof - texp[1]; // difference with ToF in muon hypothesis
          fTrees.TofEC.tpi =
            fTrees.TofEC.tof - texp[2]; // difference with ToF in charged pion hypothesis
          fTrees.TofEC.tk =
            fTrees.TofEC.tof - texp[3]; // difference with ToF in charged kaon hypothesis
          fTrees.TofEC.tp = fTrees.TofEC.tof - texp[4]; // difference with ToF in proton hypothesis
          fTrees.TofEC.Fill();
        }

        // * If ebarrel ToF ToF detector: *
        else
        {
          if(!hitStatus.is_counter()) continue;
          if(hitStatus.layer() == 1)
          { // * inner barrel ToF detector
            fTrees.TofIB.p     = ptrk;
            fTrees.TofIB.path  = (*iter_tof)->path();  // distance of flight
            fTrees.TofIB.tof   = (*iter_tof)->tof();   // time of flight
            fTrees.TofIB.ph    = (*iter_tof)->ph();    // ToF pulse height
            fTrees.TofIB.zrhit = (*iter_tof)->zrhit(); // Track extrapolate Z or R Hit position
            fTrees.TofIB.qual  = 0. + (*iter_tof)->quality(); // data quality of reconstruction
            fTrees.TofIB.cntr  = 0. + (*iter_tof)->tofID();   // ToF counter ID
            double texp[5];
            for(int j = 0; j < 5; j++)
            {
              double gb   = ptrk / xmass[j]; // v = p/m (non-relativistic velocity)
              double beta = gb / sqrt(1 + gb * gb);
              texp[j]     = 10 * fTrees.TofIB.path / beta / velc_mm; // hypothesis ToF
            }
            fTrees.TofIB.te =
              fTrees.TofIB.tof - texp[0]; // difference with ToF in electron hypothesis
            fTrees.TofIB.tmu = fTrees.TofIB.tof - texp[1]; // difference with ToF in muon hypothesis
            fTrees.TofIB.tpi =
              fTrees.TofIB.tof - texp[2]; // difference with ToF in charged pion hypothesis
            fTrees.TofIB.tk =
              fTrees.TofIB.tof - texp[3]; // difference with ToF in charged kaon hypothesis
            fTrees.TofIB.tp =
              fTrees.TofIB.tof - texp[4]; // difference with ToF in proton hypothesis
            fTrees.TofIB.Fill();
          }

          if(hitStatus.layer() == 2)
          { // * outer barrel ToF detector
            fTrees.TofOB.p     = ptrk;
            fTrees.TofOB.path  = (*iter_tof)->path();  // distance of flight
            fTrees.TofOB.tof   = (*iter_tof)->tof();   // ToF pulse height
            fTrees.TofOB.ph    = (*iter_tof)->ph();    // ToF pulse height
            fTrees.TofOB.zrhit = (*iter_tof)->zrhit(); // track extrapolate Z or R Hit position
            fTrees.TofOB.qual  = 0. + (*iter_tof)->quality(); // data quality of reconstruction
            fTrees.TofOB.cntr  = 0. + (*iter_tof)->tofID();   // ToF counter ID
            double texp[5];
            for(int j = 0; j < 5; j++)
            {
              double gb   = ptrk / xmass[j]; // v = p/m (non-relativistic velocity)
              double beta = gb / sqrt(1 + gb * gb);
              texp[j]     = 10 * fTrees.TofOB.path / beta / velc_mm; // hypothesis ToF
            }
            fTrees.TofOB.te =
              fTrees.TofOB.tof - texp[0]; // difference with ToF in electron hypothesis
            fTrees.TofOB.tmu = fTrees.TofOB.tof - texp[1]; // difference with ToF in muon hypothesis
            fTrees.TofOB.tpi =
              fTrees.TofOB.tof - texp[2]; // difference with ToF in charged pion hypothesis
            fTrees.TofOB.tk =
              fTrees.TofOB.tof - texp[3]; // difference with ToF in charged kaon hypothesis
            fTrees.TofOB.tp =
              fTrees.TofOB.tof - texp[4]; // difference with ToF in proton hypothesis

            // * WRITE ToF outer barrel info ("tof2" branch) *
            fTrees.TofOB.Fill();
          }
        }
      }
    } // loop all charged track
  }

  /// <li> Get 4-momentum for each charged track

  TrackIter it = tracks.charged.begin();
  for(; it != tracks.charged.end(); ++it)
  {
    pid->init();
    pid->setMethod(pid->methodProbability());
    // pid->setMethod(pid->methodLikelihood()); // for Likelihood Method

    pid->setChiMinCut(4);
    pid->setRecTrack(*it);
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2() | pid->useTofE());
    pid->identify(pid->onlyPion() | pid->onlyKaon());
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    RecMdcTrack* mdcTrk = (*it)->mdcTrack();

    // * WRITE particle identification info ("pid" branch) *
    if(fTrees.PID.write)
    {
      fTrees.PID.p     = mdcTrk->p();          // momentum of the track
      fTrees.PID.cost  = cos(mdcTrk->theta()); // theta angle of the track
      fTrees.PID.dedx  = pid->chiDedx(2);      // Chi squared of the dedx of the track
      fTrees.PID.tofIB = pid->chiTof1(2);      // Chi squared of the inner barrel ToF of the track
      fTrees.PID.tofOB = pid->chiTof2(2);      // Chi squared of the outer barrel ToF of the track
      fTrees.PID.probp = pid->probPion();      // probability that it is a pion
      fTrees.PID.probK = pid->probKaon();      // probability that it is a kaon
      fTrees.PID.Fill();
    }

    RecMdcKalTrack* mdcKalTrk = (*it)->mdcKalTrack();
    if(pid->probPion() > pid->probKaon())
    {
      if(pid->probPion() < fMinPID) continue;
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
      if(mdcKalTrk->charge() > 0)
        tracks.pip.push_back(*it);
      else
        tracks.pim.push_back(*it);
    }
    else if(pid->probKaon() < fMinPID)
      continue;
    RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);
    if(mdcKalTrk->charge() < 0)
      tracks.Km.push_back(*it);
    else
      tracks.Kp.push_back(*it);
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

  /// <li> Apply Kalman 4-constrain kinematic fit
  if(fDo_fit4c)
  {
    // * Run over all gamma pairs and find the pair with the best chi2
    fD0omega.K4pi.fit4c.chi2 = 9999.;
    HepLorentzVector pTot;
    TrackIter        it1 = tracks.photon.begin();
    for(; it1 != tracks.photon.end(); ++it1)
    {
      RecEmcShower* g1Trk = (*it1)->emcShower();
      TrackIter     it2   = tracks.photon.begin();
      for(; it2 != tracks.photon.end(); ++it2)
      {
        RecEmcShower* g2Trk = (*it2)->emcShower();
        kkmfit->init();
        kkmfit->AddTrack(0, wKm);
        kkmfit->AddTrack(1, wpim);
        kkmfit->AddTrack(2, wpip1);
        kkmfit->AddTrack(3, wpip2);
        kkmfit->AddTrack(4, 0.0, g1Trk);
        kkmfit->AddTrack(5, 0.0, g2Trk);
        kkmfit->AddFourMomentum(0, ecms);
        if(kkmfit->Fit())
        {
          double chi2 = kkmfit->chisq();
          if(chi2 < fD0omega.K4pi.fit4c.chi2)
          {
            fD0omega.K4pi.fit4c.chi2 = chi2;
            results.Km               = kkmfit->pfit(0);
            results.pim              = kkmfit->pfit(1);
            results.pip1             = kkmfit->pfit(2);
            results.pip2             = kkmfit->pfit(3);
            results.g1               = kkmfit->pfit(4);
            results.g2               = kkmfit->pfit(5);
            results.pi0              = results.g1 + results.g2;
          }
        }
      }
    }

    log << MSG::INFO << " chisq 4C = " << fD0omega.K4pi.fit4c.chi2 << endmsg;

    /// **Apply cut**: fit4c passed and ChiSq less than fMaxChiSq.
    if(fD0omega.K4pi.fit4c.chi2 < fMaxChiSq)
    {
      results.comb1.D0    = results.pip1 + results.Km;
      results.comb1.omega = results.pip2 + results.pim + results.pi0;
      results.comb2.D0    = results.pip2 + results.Km;
      results.comb2.omega = results.pip1 + results.pim + results.pi0;

      fD0omega.K4pi.fit4c.pi0.m = results.pi0.m();
      fD0omega.K4pi.fit4c.pi0.p = results.pi0.rho();

      double m1 = abs(results.comb1.omega.m() - momega);
      double m2 = abs(results.comb2.omega.m() - momega);
      if(m1 < m2)
      {
        fD0omega.K4pi.fit4c.D0.m    = results.comb1.D0.m();
        fD0omega.K4pi.fit4c.omega.m = results.comb1.omega.m();
        fD0omega.K4pi.fit4c.D0.p    = results.comb1.D0.rho();
        fD0omega.K4pi.fit4c.omega.p = results.comb1.omega.rho();
        if(fD0omega.K4pi.MC.write)
        {
          fD0omega.K4pi.MC.mD0_4C    = results.comb1.D0.m();
          fD0omega.K4pi.MC.momega_4C = results.comb1.omega.m();
        }
      }
      else
      {
        fD0omega.K4pi.fit4c.D0.m    = results.comb2.D0.m();
        fD0omega.K4pi.fit4c.omega.m = results.comb2.omega.m();
        fD0omega.K4pi.fit4c.D0.p    = results.comb2.D0.rho();
        fD0omega.K4pi.fit4c.omega.p = results.comb2.omega.rho();
        if(fD0omega.K4pi.MC.write)
        {
          fD0omega.K4pi.MC.mD0_4C    = results.comb2.D0.m();
          fD0omega.K4pi.MC.momega_4C = results.comb2.omega.m();
        }
      }
      fD0omega.K4pi.fit4c.Fill();
      fTrees.cuts[5]++;
    }
  }

  /// <li> Apply Kalman kinematic fit
  if(fDo_fit5c)
  {
    // * Find the best combination over all possible pi+ pi- gamma gamma pair
    fD0omega.K4pi.fit5c.chi2 = 9999.;

    TrackIter it1 = tracks.photon.begin();
    for(; it1 != tracks.photon.end(); ++it1)
    {
      RecEmcShower* g1Trk = (*it1)->emcShower();
      TrackIter     it2   = tracks.photon.begin();
      for(; it2 != tracks.photon.end(); ++it2)
      {
        RecEmcShower* g2Trk = (*it2)->emcShower();
        kkmfit->init();
        kkmfit->AddTrack(0, wKm);
        kkmfit->AddTrack(1, wpim);
        kkmfit->AddTrack(2, wpip1);
        kkmfit->AddTrack(3, wpip2);
        kkmfit->AddTrack(4, 0.0, g1Trk);
        kkmfit->AddTrack(5, 0.0, g2Trk);
        kkmfit->AddFourMomentum(0, ecms);
        kkmfit->AddResonance(1, mpi0, 4, 5);
        if(!kkmfit->Fit(0)) continue;
        if(!kkmfit->Fit(1)) continue;
        if(kkmfit->Fit())
        {
          double chi2 = kkmfit->chisq();
          if(chi2 < fD0omega.K4pi.fit5c.chi2)
          {
            fD0omega.K4pi.fit5c.chi2 = chi2;
            results.Km               = kkmfit->pfit(0);
            results.pim              = kkmfit->pfit(1);
            results.pip1             = kkmfit->pfit(2);
            results.pip2             = kkmfit->pfit(3);
            results.g1               = kkmfit->pfit(4);
            results.g2               = kkmfit->pfit(5);
            results.pi0              = results.g1 + results.g2;
          }
        }
      }
    }

    log << MSG::INFO << " chisq 5C = " << fD0omega.K4pi.fit5c.chi2 << endmsg;

    /// **Apply cut**: fit5c passed and ChiSq less than fMaxChiSq.
    if(fD0omega.K4pi.fit5c.chi2 < fMaxChiSq)
    {
      results.comb1.D0    = results.pip1 + results.Km;
      results.comb1.omega = results.pip2 + results.pim + results.pi0;
      results.comb2.D0    = results.pip2 + results.Km;
      results.comb2.omega = results.pip1 + results.pim + results.pi0;

      fD0omega.K4pi.fit5c.pi0.m = results.pi0.m();
      fD0omega.K4pi.fit5c.pi0.p = results.pi0.rho();

      double m1 = abs(results.comb1.omega.m() - momega);
      double m2 = abs(results.comb2.omega.m() - momega);
      if(m1 < m2)
      {
        fD0omega.K4pi.fit5c.D0.m    = results.comb1.D0.m();
        fD0omega.K4pi.fit5c.omega.m = results.comb1.omega.m();
        fD0omega.K4pi.fit5c.D0.p    = results.comb1.D0.rho();
        fD0omega.K4pi.fit5c.omega.p = results.comb1.omega.rho();
        if(fD0omega.K4pi.MC.write)
        {
          fD0omega.K4pi.MC.mD0_5C    = results.comb1.D0.m();
          fD0omega.K4pi.MC.momega_5C = results.comb1.omega.m();
        }
      }
      else
      {
        fD0omega.K4pi.fit5c.D0.m    = results.comb2.D0.m();
        fD0omega.K4pi.fit5c.omega.m = results.comb2.omega.m();
        fD0omega.K4pi.fit5c.D0.p    = results.comb2.D0.rho();
        fD0omega.K4pi.fit5c.omega.p = results.comb2.omega.rho();
        if(fD0omega.K4pi.MC.write)
        {
          fD0omega.K4pi.MC.mD0_5C    = results.comb2.D0.m();
          fD0omega.K4pi.MC.momega_5C = results.comb2.omega.m();
        }
      }

      // * Photon kinematics * //
      double eg1               = results.g1.e();
      double eg2               = results.g2.e();
      fD0omega.K4pi.fit5c.fcos = (eg1 - eg2) / results.pi0.rho(); // E/p ratio for pi^0 candidate
      fD0omega.K4pi.fit5c.Elow = (eg1 < eg2) ? eg1 : eg2;         // lowest energy of the two gammas

      // * WRITE pi^0 information from EMCal ("fit5c" branch) *
      fD0omega.K4pi.fit5c.Fill(); // "fit5c" branch
      fTrees.cuts[6]++;           // ChiSq has to be less than 200 and fit5c has to be passed
    }
  }

  /// <li> Get MC truth
  bool writeMC = (fD0omega.K4pi.MC.momega_4C < 100.) || (fD0omega.K4pi.MC.momega_5C < 100.);
  if(fD0omega.K4pi.MC.write && eventHeader->runNumber() < 0 && writeMC)
  {
    fD0omega.K4pi.MC.runid = eventHeader->runNumber();
    fD0omega.K4pi.MC.evtid = eventHeader->eventNumber();
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
    if(!mcParticleCol)
      cout << "Could not retrieve McParticelCol" << endl;
    else
    {
      fD0omega.K4pi.MC.n = 0;
      bool doNotInclude(true);
      int  indexOffset = -1;
      bool incPdcy(false);
      int  rootIndex(-1);

      Event::McParticleCol::iterator it = mcParticleCol->begin();
      fD0omega.K4pi.MC.PDG.clear();
      fD0omega.K4pi.MC.mother.clear();
      for(; it != mcParticleCol->end(); it++)
      {
        if((*it)->primaryParticle()) continue;
        if(!(*it)->decayFromGenerator()) continue;
        if((*it)->particleProperty() == incPid)
        {
          incPdcy   = true;
          rootIndex = (*it)->trackIndex();
        }
        if(!incPdcy) continue;
        fD0omega.K4pi.MC.PDG.push_back((*it)->particleProperty());
        if((*it)->mother().particleProperty() == incPid)
          fD0omega.K4pi.MC.mother.push_back((*it)->mother().trackIndex() - rootIndex);
        else
          fD0omega.K4pi.MC.mother.push_back((*it)->mother().trackIndex() - rootIndex - 1);
        if((*it)->particleProperty() == incPid) fD0omega.K4pi.MC.mother[fD0omega.K4pi.MC.n] = 0;
        ++fD0omega.K4pi.MC.n;
      }
      fD0omega.K4pi.MC.Fill();
    }
  }

  /// </ol>
  return StatusCode::SUCCESS;
}

// * ========================== * //
// * -------- FINALIZE -------- * //
// * ========================== * //
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
  if(fD0omega.K4pi.fit4c.write) cout << "  fit4c:  " << fD0omega.K4pi.fit4c.GetEntries() << endl;
  if(fD0omega.K4pi.fit5c.write) cout << "  fit5c:  " << fD0omega.K4pi.fit5c.GetEntries() << endl;
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
  fD0omega.K4pi.fit4c.Write();
  fD0omega.K4pi.fit5c.Write();
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