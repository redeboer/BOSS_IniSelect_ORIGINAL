// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
#include "D0phi_3Kpi/D0phi_3Kpi.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
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
#include "ParticleID/ParticleID.h"
#include "TMath.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include <vector>
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using namespace std;

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

HepLorentzVector ecms(0.034, 0, 0, Ecms);

const int incPid1 = 91;
const int incPid2 = 92;
const int incPid  = 443;

// * Typedefs * //
typedef vector<int>              Vint;
typedef vector<HepLorentzVector> Vp4;

struct Reconstructed
{
  HepLorentzVector D0;
  HepLorentzVector phi;
  HepLorentzVector Jpsi;
};

struct LorentzVectors
{
  HepLorentzVector Km1;
  HepLorentzVector Km2;
  HepLorentzVector Kp;
  HepLorentzVector pip;
  Reconstructed    comb1;
  Reconstructed    comb2;
};

LorentzVectors results;

double ThreeMom(const HepLorentzVector& v)
{
  return v.e() * v.e() - v.m() * v.m();
}

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //
/// Constructor for the `D0phi_3Kpi` algorithm.
/// Here, you should declare properties: give them a name, assign a parameter (data member of `D0phi_3Kpi`), and if required a documentation string. Note that you should define the paramters themselves in the header (D0phi_3Kpi/D0phi_3Kpi.h) and that you should assign the values in `share/jopOptions_D0phi_3Kpi.txt`. Algorithms should inherit from Gaudi's `Algorithm` class. See https://dayabay.bnl.gov/dox/GaudiKernel/html/classAlgorithm.html.
D0phi_3Kpi::D0phi_3Kpi(const string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{
  // * Define r0, z0 cut for charged tracks *
  declareProperty("Vr0cut", fVr0cut);
  declareProperty("Vz0cut", fVz0cut);
  declareProperty("Vrvz0cut", fRvz0cut);
  declareProperty("Vrvxy0cut", fRvxy0cut);

  // * Whether to test the success of the 4- and 5-constraint fits *
  declareProperty("Test4C", fDo_fit4c);   // write fit4c
  declareProperty("MaxChiSq", fMaxChiSq); // chisq for both fits should be less
  declareProperty("MinPID", fMinPID);     // PID probability should be at least this value

  // * Whether or not to check success of Particle Identification *
  declareProperty("CheckVertex", fCheckVertex);
  declareProperty("CheckMCtruth", fCheckMC);
  declareProperty("CheckDedx", fCheckDedx);
  declareProperty("CheckTof", fCheckTof);
  declareProperty("CheckPID", fCheckPID);
}

// * ========================== * //
// * ------- INITIALIZE ------- * //
// * ========================== * //
/// (Inherited) `initialize` step of `Algorithm`. This function is called only once in the beginning. **Define and load `NTuple`s here.**
StatusCode D0phi_3Kpi::initialize()
{
  // * Log stream and status code * //
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "In initialize():" << endmsg;

  /// <table>
  /// <tr><td colspan="2">**`NTuple "vxyz"`:   Vertex information of the charged tracks**</td></tr>
  if(fCheckVertex)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/vxyz");
    if(nt)
      fTupleVxyz = nt;
    else
    {
      fTupleVxyz = ntupleSvc()->book("FILE1/vxyz", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleVxyz)
      {
        fTupleVxyz->addItem("vx0", fVx0);
        /// <tr><td>`"vx0"`   </td><td>Primary \f$x\f$-vertex as determined by MDC</td></tr>
        fTupleVxyz->addItem("vy0", fVy0);
        /// <tr><td>`"vy0"`   </td><td>Primary \f$y\f$-vertex as determined by MDC</td></tr>
        fTupleVxyz->addItem("vz0", fVz0);
        /// <tr><td>`"vz0"`   </td><td>Primary \f$z\f$-vertex as determined by MDC</td></tr>
        fTupleVxyz->addItem("vr0", fVr0);
        /// <tr><td>`"vr0"`   </td><td>Distance from origin in \f$xy\f$-plane</td></tr>
        fTupleVxyz->addItem("rvxy0", fRvxy0);
        /// <tr><td>`"rvxy0"` </td><td>Nearest distance to IP in \f$xy\f$ plane</td></tr>
        fTupleVxyz->addItem("rvz0", fRvz0);
        /// <tr><td>`"rvz0"`  </td><td>Nearest distance to IP in \f$z\f$ direction</td></tr>
        fTupleVxyz->addItem("rvphi0", fRvphi0);
        /// <tr><td>`"rvphi0"`</td><td>Angle in the \f$xy\f$-plane (?)</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleVxyz) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "fit4c"`:  4-constraint fit branch**</td></tr>
  if(fDo_fit4c)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/fit4c");
    if(nt)
      fTupleFit4C = nt;
    else
    {
      fTupleFit4C = ntupleSvc()->book("FILE1/fit4c", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleFit4C)
      {
        fTupleFit4C->addItem("chi2", fChi_4C);
        /// <tr><td>`"chi2"` </td><td>\f$\chi^2\f$ of the Kalman kinematic fit</td></tr>
        fTupleFit4C->addItem("mpi0", fMpi0_4C);
        /// <tr><td>`"mpi0"`</td><td>Reconstructed invariant \f$\pi^0\f$ mass</td></tr>
        fTupleFit4C->addItem("mD0", fMD0_4C);
        /// <tr><td>`"mD0"`</td><td>Reconstructed invariant \f$\pi^0\f$ mass</td></tr>
        fTupleFit4C->addItem("mphi", fMphi_4C);
        /// <tr><td>`"mphi"`</td><td>Reconstructed invariant \f$\pi^0\f$ mass</td></tr>
        fTupleFit4C->addItem("ppi0", fPpi0_4C);
        /// <tr><td>`"mpi0"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
        fTupleFit4C->addItem("pD0", fPD0_4C);
        /// <tr><td>`"mD0"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
        fTupleFit4C->addItem("pphi", fPphi_4C);
        /// <tr><td>`"mphi"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleFit4C) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "dedx"`:   Import dE/dx PID branch**</td></tr>
  if(fCheckDedx)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/dedx");
    if(nt)
      fTupleDedx = nt;
    else
    {
      fTupleDedx = ntupleSvc()->book("FILE1/dedx", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleDedx)
      {
        fTupleDedx->addItem("ptrk", fPtrack);
        /// <tr><td>`"ptrk"`  </td><td>Momentum of the track</td></tr>
        fTupleDedx->addItem("chie", fChi2e);
        /// <tr><td>`"chie"`  </td><td>\f$\chi^2\f$ in case of electron</td></tr>
        fTupleDedx->addItem("chimu", fChi2mu);
        /// <tr><td>`"chimu"` </td><td>\f$\chi^2\f$ in case of muon</td></tr>
        fTupleDedx->addItem("chipi", fChi2pi);
        /// <tr><td>`"chipi"` </td><td>\f$\chi^2\f$ in case of pion</td></tr>
        fTupleDedx->addItem("chik", fChi2k);
        /// <tr><td>`"chik"`  </td><td>\f$\chi^2\f$ in case of kaon</td></tr>
        fTupleDedx->addItem("chip", fChi2p);
        /// <tr><td>`"chip"`  </td><td>\f$\chi^2\f$ in case of proton</td></tr>
        fTupleDedx->addItem("probPH", fProbPH);
        /// <tr><td>`"probPH"`</td><td>Most probable pulse height from truncated mean</td></tr>
        fTupleDedx->addItem("normPH", fNormPH);
        /// <tr><td>`"normPH"`</td><td>Normalized pulse height</td></tr>
        fTupleDedx->addItem("ghit", fGhit);
        /// <tr><td>`"ghit"`  </td><td>Number of good hits</td></tr>
        fTupleDedx->addItem("thit", fThit);
        /// <tr><td>`"thit"`  </td><td>Total number of hits</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleDedx) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "tofe"`:   ToF endcap branch**</td></tr>
  if(fCheckTof)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/tofe");
    if(nt)
      fTupleTofEC = nt;
    else
    {
      fTupleTofEC = ntupleSvc()->book("FILE1/tofe", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleTofEC)
      {
        fTupleTofEC->addItem("ptrk", fPtotTofEC);
        /// <tr><td>`"ptrk"`</td><td>Momentum of the track as reconstructed by MDC</td></tr>
        fTupleTofEC->addItem("path", fPathTofEC);
        /// <tr><td>`"path"`</td><td>Path length</td></tr>
        fTupleTofEC->addItem("tof", fTofEC);
        /// <tr><td>`"tof"` </td><td>Time of flight</td></tr>
        fTupleTofEC->addItem("cntr", fCntrTofEC);
        /// <tr><td>`"cntr"`</td><td>ToF counter ID</td></tr>
        fTupleTofEC->addItem("ph", fPhTofEC);
        /// <tr><td>`"ph"`  </td><td>ToF pulse height</td></tr>
        fTupleTofEC->addItem("rhit", fRhitTofEC);
        /// <tr><td>`"rhit"`</td><td>Track extrapolate Z or R Hit position</td></tr>
        fTupleTofEC->addItem("qual", fQualTofEC);
        /// <tr><td>`"qual"`</td><td>Data quality of reconstruction</td></tr>
        fTupleTofEC->addItem("te", fElectronTofEC);
        /// <tr><td>`"te"`  </td><td>Difference with ToF in electron hypothesis</td></tr>
        fTupleTofEC->addItem("tmu", fMuonTofEC);
        /// <tr><td>`"tmu"` </td><td>Difference with ToF in muon hypothesis</td></tr>
        fTupleTofEC->addItem("tpi", fProtoniTofEC);
        /// <tr><td>`"tpi"` </td><td>Difference with ToF in charged pion hypothesis</td></tr>
        fTupleTofEC->addItem("tk", fKaonTofEC);
        /// <tr><td>`"tk"`  </td><td>Difference with ToF in charged kaon hypothesis</td></tr>
        fTupleTofEC->addItem("tp", fProtonTofEC);
        /// <tr><td>`"tp"`  </td><td>Difference with ToF in proton hypothesis</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleTofEC) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "tof1"`:   ToF *inner* barrel branch**</td></tr>
  if(fCheckTof)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/tof1");
    if(nt)
      fTupleTofIB = nt;
    else
    {
      fTupleTofIB = ntupleSvc()->book("FILE1/tof1", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleTofIB)
      {
        fTupleTofIB->addItem("ptrk", fPtotTofIB);
        /// <tr><td>`"ptrk"`</td><td>Momentum of the track as reconstructed by MDC</td></tr>
        fTupleTofIB->addItem("path", fPathTofIB);
        /// <tr><td>`"path"`</td><td>Path length</td></tr>
        fTupleTofIB->addItem("tof", fTofIB);
        /// <tr><td>`"tof"` </td><td>Time of flight</td></tr>
        fTupleTofIB->addItem("cntr", fCntrTofIB);
        /// <tr><td>`"cntr"`</td><td>ToF counter ID</td></tr>
        fTupleTofIB->addItem("ph", fPhTofIB);
        /// <tr><td>`"ph"`  </td><td>ToF pulse height</td></tr>
        fTupleTofIB->addItem("zhit", fZhitTofIB);
        /// <tr><td>`"zhit"`</td><td>Track extrapolate Z or R Hit position</td></tr>
        fTupleTofIB->addItem("qual", fQualTofIB);
        /// <tr><td>`"qual"`</td><td>Data quality of reconstruction</td></tr>
        fTupleTofIB->addItem("te", fElectronTofIB);
        /// <tr><td>`"te"`  </td><td>Difference with ToF in electron hypothesis</td></tr>
        fTupleTofIB->addItem("tmu", fMuonTofIB);
        /// <tr><td>`"tmu"` </td><td>Difference with ToF in muon hypothesis</td></tr>
        fTupleTofIB->addItem("tpi", fProtoniTofIB);
        /// <tr><td>`"tpi"` </td><td>Difference with ToF in charged pion hypothesis</td></tr>
        fTupleTofIB->addItem("tk", fKaonTofIB);
        /// <tr><td>`"tk"`  </td><td>Difference with ToF in charged kaon hypothesis</td></tr>
        fTupleTofIB->addItem("tp", fProtonTofIB);
        /// <tr><td>`"tp"`  </td><td>Difference with ToF in proton hypothesis</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleTofIB) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "tof2"`:   ToF *outer* barrel branch**</td></tr>
  if(fCheckTof)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/tof2");
    if(nt)
      fTupleTofOB = nt;
    else
    {
      fTupleTofOB = ntupleSvc()->book("FILE1/tof2", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleTofOB)
      {
        fTupleTofOB->addItem("ptrk", fPtotTofOB);
        /// <tr><td>`"ptrk"`</td><td>Momentum of the track as reconstructed by MDC</td></tr>
        fTupleTofOB->addItem("path", fPathTofOB);
        /// <tr><td>`"path"`</td><td>Path length</td></tr>
        fTupleTofOB->addItem("tof", fTofOB);
        /// <tr><td>`"tof"` </td><td>Time of flight</td></tr>
        fTupleTofOB->addItem("cntr", fCntrTofOB);
        /// <tr><td>`"cntr"`</td><td>ToF counter ID</td></tr>
        fTupleTofOB->addItem("ph", fPhTofOB);
        /// <tr><td>`"ph"`  </td><td>ToF pulse height</td></tr>
        fTupleTofOB->addItem("zhit", fZhitTofOB);
        /// <tr><td>`"zhit"`</td><td>Track extrapolate Z or R Hit position</td></tr>
        fTupleTofOB->addItem("qual", fQualTofOB);
        /// <tr><td>`"qual"`</td><td>Data quality of reconstruction</td></tr>
        fTupleTofOB->addItem("te", fElectronTofOB);
        /// <tr><td>`"te"`  </td><td>Difference with ToF in electron hypothesis</td></tr>
        fTupleTofOB->addItem("tmu", fMuonTofOB);
        /// <tr><td>`"tmu"` </td><td>Difference with ToF in muon hypothesis</td></tr>
        fTupleTofOB->addItem("tpi", fProtoniTofOB);
        /// <tr><td>`"tpi"` </td><td>Difference with ToF in charged pion hypothesis</td></tr>
        fTupleTofOB->addItem("tk", fKaonTofOB);
        /// <tr><td>`"tk"`  </td><td>Difference with ToF in charged kaon hypothesis</td></tr>
        fTupleTofOB->addItem("tp", fProtonTofOB);
        /// <tr><td>`"tp"`  </td><td>Difference with ToF in proton hypothesis</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleTofOB) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "pid"`:    Track PID information**</td></tr>
  if(fCheckPID)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/pid");
    if(nt)
      fTuplePID = nt;
    else
    {
      fTuplePID = ntupleSvc()->book("FILE1/pid", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTuplePID)
      {
        fTuplePID->addItem("ptrk", fPtrackPID);
        /// <tr><td>`"ptrk"`</td><td>Momentum of the track</td></tr>
        fTuplePID->addItem("cost", fCostPID);
        /// <tr><td>`"cost"`</td><td>Theta angle of the track</td></tr>
        fTuplePID->addItem("dedx", fDedxPID);
        /// <tr><td>`"dedx"`</td><td>\f$\chi^2\f$ of the \f$dE/dx\f$ of the track</td></tr>
        fTuplePID->addItem("tof1", fTof1PID);
        /// <tr><td>`"tof1"`</td><td>\f$\chi^2\f$ of the inner barrel ToF of the track</td></tr>
        fTuplePID->addItem("tof2", fTof2PID);
        /// <tr><td>`"tof2"`</td><td>\f$\chi^2\f$ of the outer barrel ToF of the track</td></tr>
        fTuplePID->addItem("prob", fProbPi);
        /// <tr><td>`"prob"`</td><td>Probability that it is a pion</td></tr>
        fTuplePID->addItem("prob", fProbK);
        /// <tr><td>`"prob"`</td><td>Probability that it is a kaon</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTuplePID) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "topology"`: Monte Carlo truth for TopoAna package**</td></tr>
  if(fCheckMC)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/topology");
    if(nt)
      fTupleMC = nt;
    else
    {
      fTupleMC = ntupleSvc()->book("FILE1/topology", CLID_ColumnWiseTuple,
                                   "Monte Carlo truth for TopoAna package");
      if(fTupleMC)
      {
        fTupleMC->addItem("mD0_fit4c", fMC_4C_mD0);
        fTupleMC->addItem("mphi_fit4c", fMC_4C_mphi);
        fTupleMC->addItem("run_number", fRunid);
        /// <tr><td>`"Runid"`</td><td>run number ID</td></tr>
        fTupleMC->addItem("event_number", fEvtid);
        /// <tr><td>`"Evtid"`</td><td>event number ID</td></tr>
        fTupleMC->addItem("Nparticles", fNparticles, 0, 100);
        /// <tr><td>`"Nparticles"`</td><td>number of MC particles stored for this event. This one is necessary for loading following two items, because they are arrays</td></tr>
        fTupleMC->addIndexedItem("PDG", fNparticles, fPDG);
        /// <tr><td>`"PDG"` (array)</td><td>PDG code for the particle in this array</td></tr>
        fTupleMC->addIndexedItem("mother", fNparticles, fMother);
        /// <tr><td>`"mother"` (array)</td><td>track index of the mother particle (corrected with `rootIndex`)</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleMC) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// **`NTuple "cutflow"`: Counters for cut flow**
  if(true)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/_cutvalues");
    if(nt)
      fTupleCutFlow = nt;
    else
    {
      fTupleCutFlow = ntupleSvc()->book("FILE1/_cutvalues", CLID_ColumnWiseTuple,
                                        "Monte Carlo truth for TopoAna package");
      if(fTupleCutFlow)
      {
        fTupleCutFlow->addItem("Total_events", fNCut0);
        fTupleCutFlow->addItem("Pass_N_charged_tracks", fNCut1);
        fTupleCutFlow->addItem("Pass_zero_net_charge", fNCut2);
        fTupleCutFlow->addItem("Pass_PID", fNCut3);
        fTupleCutFlow->addItem("Pass_4C_Kalman_fit", fNCut4);
        fNCut0 = 0;
        fNCut1 = 0;
        fNCut2 = 0;
        fNCut3 = 0;
        fNCut4 = 0;
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleCutFlow) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// </table>
  log << MSG::INFO << "Successfully returned from initialize()" << endmsg;
  return StatusCode::SUCCESS;
}

// * ========================= * //
// * -------- EXECUTE -------- * //
// * ========================= * //
/// Inherited `execute` method of the `Algorithm`. This function is called *for each event*.
StatusCode D0phi_3Kpi::execute()
{
  /// <ol>
  /// <li> Create log stream (`MsgStream` class)
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "In execute():" << endmsg;

  /// <li> Load next event from DST file

  // * Load DST file info *
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(), "/Event/EventHeader");
  int                              runNo = eventHeader->runNumber();
  int                              evtNo = eventHeader->eventNumber();
  log << MSG::DEBUG << "run, evtnum = " << runNo << " , " << evtNo << endmsg;
  fNCut0++; // counter for all events

  // Reset chi square values
  if(fCheckMC)
  {
    fMC_4C_mD0  = 9999.;
    fMC_4C_mphi = 9999.;
  }

  // * Load event information and track collection *
  /*
    http://bes3.to.infn.it/Boss/7.0.2/html/namespaceEventModel_1_1EvtRec.html (namespace)
    http://bes3.to.infn.it/Boss/7.0.2/html/classEvtRecEvent.html (class)
    http://bes3.to.infn.it/Boss/7.0.2/html/EvtRecTrack_8h.html (typedef EvtRecTrackCol)
  */
  SmartDataPtr<EvtRecEvent>    evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);

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
  // The first part of the set of reconstructed tracks are the charged tracks
  int  nCharge = 0; // Number of charged tracks as identified by the MDC.
  Vint iGood;
  // Vector of integers that give the position of tracks in the `evtRecEvent` marked good.
  for(int i = 0; i < evtRecEvent->totalCharged(); ++i)
  {
    // * Get track: beginning of all tracks + event number
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;

    // * Get track info from Main Drift Chamber
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    double       pch    = mdcTrk->p();
    double       x0     = mdcTrk->x();
    double       y0     = mdcTrk->y();
    double       z0     = mdcTrk->z();
    double       phi0   = mdcTrk->helix(1);

    // * Get vertex origin
    double xv  = xorigin.x();
    double yv  = xorigin.y();
    double Rxy = (x0 - xv) * cos(phi0) + (y0 - yv) * sin(phi0);

    // * Get radii of vertex
    HepVector    a  = mdcTrk->helix();
    HepSymMatrix Ea = mdcTrk->err();
    HepPoint3D   point0(0., 0., 0.); // the initial point for MDC recosntruction
    HepPoint3D   IP(xorigin[0], xorigin[1], xorigin[2]);
    VFHelix      helixip(point0, a, Ea);
    helixip.pivot(IP);
    HepVector vecipa = helixip.a();
    double    Rvxy0  = fabs(vecipa[0]); // nearest distance to IP in xy plane
    double    Rvz0   = vecipa[3];       // nearest distance to IP in z direction
    double    Rvphi0 = vecipa[1];

    // * WRITE primary vertex position info ("vxyz" branch) *
    if(fCheckVertex)
    {
      fVx0    = x0;        // primary x-vertex as determined by MDC
      fVy0    = y0;        // primary y-vertex as determined by MDC
      fVz0    = z0;        // primary z-vertex as determined by MDC
      fVr0    = Rxy;       // distance from origin in xy-plane
      fRvxy0  = Rvxy0;     // nearest distance to IP in xy plane
      fRvz0   = Rvz0;      // nearest distance to IP in z direction
      fRvphi0 = Rvphi0;    // angle in the xy-plane (?)
      fTupleVxyz->write(); // "vxyz" branch
    }

    // * Apply vertex cuts *
    if(fabs(z0) >= fVz0cut) continue;
    if(fabs(Rxy) >= fVr0cut) continue;
    if(fabs(Rvz0) >= fRvz0cut) continue;
    if(fabs(Rvxy0) >= fRvxy0cut) continue;

    // * Add charged track to vector *
    iGood.push_back(i);
    nCharge += mdcTrk->charge();
  }

  /// **Apply cut**: cut number of charged tracks and net charge
  int nGood = iGood.size();
  log << MSG::DEBUG << "ngood, totcharge = " << nGood << " , " << nCharge << endmsg;
  if(nGood != 4) return StatusCode::SUCCESS;
  fNCut1++;
  if(nCharge != 0) return StatusCode::SUCCESS;
  fNCut2++;

  /// <li> Check charged track ToF PID information
  if(fCheckTof)
  {
    for(int i = 0; i < nGood; ++i)
    {
      EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i];
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if(!(*itTrk)->isTofTrackValid()) continue;

      RecMdcTrack*                mdcTrk    = (*itTrk)->mdcTrack();
      SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();

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
          double path = (*iter_tof)->path();         // distance of flight
          double tof  = (*iter_tof)->tof();          // time of flight
          double ph   = (*iter_tof)->ph();           // ToF pulse height
          double rhit = (*iter_tof)->zrhit();        // Track extrapolate Z or R Hit position
          double qual = 0. + (*iter_tof)->quality(); // data quality of reconstruction
          double cntr = 0. + (*iter_tof)->tofID();   // ToF counter ID

          // * Get ToF for each particle hypothesis *
          double texp[5];
          for(int j = 0; j < 5; j++)
          {
            double gb   = ptrk / xmass[j]; // v = p/m (non-relativistic velocity)
            double beta = gb / sqrt(1 + gb * gb);
            texp[j]     = 10 * path / beta / velc_mm; // hypothesis ToF
          }

          // * WRITE ToF end cap info ("tofe" branch) *
          fPtotTofEC     = ptrk;          // momentum of the track as reconstructed by MDC
          fPathTofEC     = path;          // path length
          fTofEC         = tof;           // time of flight
          fCntrTofEC     = cntr;          // ToF counter ID
          fPhTofEC       = ph;            // ToF pulse height
          fRhitTofEC     = rhit;          // track extrapolate Z or R Hit position
          fQualTofEC     = qual;          // data quality of reconstruction
          fElectronTofEC = tof - texp[0]; // difference with ToF in electron hypothesis
          fMuonTofEC     = tof - texp[1]; // difference with ToF in muon hypothesis
          fProtoniTofEC  = tof - texp[2]; // difference with ToF in charged pion hypothesis
          fKaonTofEC     = tof - texp[3]; // difference with ToF in charged kaon hypothesis
          fProtonTofEC   = tof - texp[4]; // difference with ToF in proton hypothesis
          fTupleTofEC->write();           // "tofe" branch
        }

        // * If ebarrel ToF ToF detector: *
        else
        {
          if(!hitStatus.is_counter()) continue;
          if(hitStatus.layer() == 1)
          {                                            // * inner barrel ToF detector
            double path = (*iter_tof)->path();         // distance of flight
            double tof  = (*iter_tof)->tof();          // time of flight
            double ph   = (*iter_tof)->ph();           // ToF pulse height
            double rhit = (*iter_tof)->zrhit();        // Track extrapolate Z or R Hit position
            double qual = 0. + (*iter_tof)->quality(); // data quality of reconstruction
            double cntr = 0. + (*iter_tof)->tofID();   // ToF counter ID
            double texp[5];
            for(int j = 0; j < 5; j++)
            {
              double gb   = ptrk / xmass[j]; // v = p/m (non-relativistic velocity)
              double beta = gb / sqrt(1 + gb * gb);
              texp[j]     = 10 * path / beta / velc_mm; // hypothesis ToF
            }

            // * WRITE ToF inner barrel info ("tof1" branch) *
            fPtotTofIB     = ptrk;          // momentum of the track as reconstructed by MDC
            fPathTofIB     = path;          // path length
            fTofIB         = tof;           // time of flight
            fCntrTofIB     = cntr;          // ToF counter ID
            fPhTofIB       = ph;            // ToF pulse height
            fZhitTofIB     = rhit;          // track extrapolate Z or R Hit position
            fQualTofIB     = qual;          // data quality of reconstruction
            fElectronTofIB = tof - texp[0]; // difference with ToF in electron hypothesis
            fMuonTofIB     = tof - texp[1]; // difference with ToF in muon hypothesis
            fProtoniTofIB  = tof - texp[2]; // difference with ToF in charged pion hypothesis
            fKaonTofIB     = tof - texp[3]; // difference with ToF in charged kaon hypothesis
            fProtonTofIB   = tof - texp[4]; // difference with ToF in proton hypothesis
            fTupleTofIB->write();           // "tof1" branch
          }

          if(hitStatus.layer() == 2)
          {                                            // * outer barrel ToF detector
            double path = (*iter_tof)->path();         // distance of flight
            double tof  = (*iter_tof)->tof();          // ToF pulse height
            double ph   = (*iter_tof)->ph();           // ToF pulse height
            double rhit = (*iter_tof)->zrhit();        // track extrapolate Z or R Hit position
            double qual = 0. + (*iter_tof)->quality(); // data quality of reconstruction
            double cntr = 0. + (*iter_tof)->tofID();   // ToF counter ID
            double texp[5];
            for(int j = 0; j < 5; j++)
            {
              double gb   = ptrk / xmass[j]; // v = p/m (non-relativistic velocity)
              double beta = gb / sqrt(1 + gb * gb);
              texp[j]     = 10 * path / beta / velc_mm; // hypothesis ToF
            }

            // * WRITE ToF outer barrel info ("tof2" branch) *
            fPtotTofOB     = ptrk;          // momentum of the track as reconstructed by MDC
            fPathTofOB     = path;          // path length
            fTofOB         = tof;           // time of flight
            fCntrTofOB     = cntr;          // ToF counter ID
            fPhTofOB       = ph;            // ToF pulse height
            fZhitTofOB     = rhit;          // track extrapolate Z or R Hit position
            fQualTofOB     = qual;          // data quality of reconstruction
            fElectronTofOB = tof - texp[0]; // difference with ToF in electron hypothesis
            fMuonTofOB     = tof - texp[1]; // difference with ToF in muon hypothesis
            fProtoniTofOB  = tof - texp[2]; // difference with ToF in charged pion hypothesis
            fKaonTofOB     = tof - texp[3]; // difference with ToF in charged kaon hypothesis
            fProtonTofOB   = tof - texp[4]; // difference with ToF in proton hypothesis
            fTupleTofOB->write();           // "tof2" branch
          }
        }
      }
    } // loop all charged track
  }

  /// <li> Get 4-momentum for each charged track
  Vint        iKm, iKp, ipip;
  Vp4         pKm, pKp, ppip;
  ParticleID* pid = ParticleID::instance();
  for(int i = 0; i < nGood; ++i)
  {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i];
    pid->init();
    pid->setMethod(pid->methodProbability());
    // pid->setMethod(pid->methodLikelihood()); // for Likelihood Method

    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2() | pid->useTofE());
    pid->identify(pid->onlyPion() | pid->onlyKaon());
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();

    // * WRITE particle identification info ("pid" branch) *
    if(fCheckPID)
    {
      fPtrackPID = mdcTrk->p();          // momentum of the track
      fCostPID   = cos(mdcTrk->theta()); // theta angle of the track
      fDedxPID   = pid->chiDedx(2);      // Chi squared of the dedx of the track
      fTof1PID   = pid->chiTof1(2);      // Chi squared of the inner barrel ToF of the track
      fTof2PID   = pid->chiTof2(2);      // Chi squared of the outer barrel ToF of the track
      fProbPi    = pid->probPion();      // probability that it is a pion
      fProbK     = pid->probKaon();      // probability that it is a kaon
      fTuplePID->write();                // "pid" branch
    }

    if(pid->probPion() > pid->probKaon())
    {
      if(pid->probPion() < fMinPID) continue;

      RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();
      // After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);

      if(mdcKalTrk->charge() > 0)
      {
        ipip.push_back(iGood[i]);
        HepLorentzVector ptrk;
        ptrk.setPx(mdcKalTrk->px());
        ptrk.setPy(mdcKalTrk->py());
        ptrk.setPz(mdcKalTrk->pz());
        double p3 = ptrk.mag();
        ptrk.setE(sqrt(p3 * p3 + mpi * mpi));
        // ptrk = ptrk.boost(-0.011, 0, 0); // boost to cms
        ppip.push_back(ptrk);
      }
    }
    else
    {
      if(pid->probKaon() < fMinPID) continue;

      RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();
      // After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
      RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);

      if(mdcKalTrk->charge() < 0)
      {
        iKm.push_back(iGood[i]);
        HepLorentzVector ptrk;
        ptrk.setPx(mdcKalTrk->px());
        ptrk.setPy(mdcKalTrk->py());
        ptrk.setPz(mdcKalTrk->pz());
        double p3 = ptrk.mag();
        ptrk.setE(sqrt(p3 * p3 + mK * mK));
        // ptrk = ptrk.boost(-0.011, 0, 0); // boost to cms
        pKm.push_back(ptrk);
      }
      else
      {
        iKp.push_back(iGood[i]);
        HepLorentzVector ptrk;
        ptrk.setPx(mdcKalTrk->px());
        ptrk.setPy(mdcKalTrk->py());
        ptrk.setPz(mdcKalTrk->pz());
        double p3 = ptrk.mag();
        ptrk.setE(sqrt(p3 * p3 + mK * mK));
        // ptrk = ptrk.boost(-0.011, 0, 0); // boost to cms
        pKp.push_back(ptrk);
      }
    }
  }

  /// **Apply cut**: PID
  if(iKm.size() != 2) return SUCCESS;
  if(iKp.size() != 1) return SUCCESS;
  if(ipip.size() != 1) return SUCCESS;
  fNCut3++;

  RecMdcKalTrack* Km1Trk = (*(evtRecTrkCol->begin() + iKm[0]))->mdcKalTrack();
  RecMdcKalTrack* Km2Trk = (*(evtRecTrkCol->begin() + iKm[1]))->mdcKalTrack();
  RecMdcKalTrack* KpTrk  = (*(evtRecTrkCol->begin() + iKp[0]))->mdcKalTrack();
  RecMdcKalTrack* pipTrk = (*(evtRecTrkCol->begin() + ipip[0]))->mdcKalTrack();

  WTrackParameter wvKm1Trk(mK, Km1Trk->getZHelix(), Km1Trk->getZError());
  WTrackParameter wvKm2Trk(mK, Km2Trk->getZHelix(), Km2Trk->getZError());
  WTrackParameter wvKpTrk(mK, KpTrk->getZHelix(), KpTrk->getZError());
  WTrackParameter wvpipTrk(mpi, pipTrk->getZHelix(), pipTrk->getZError());

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

  VertexFit* vtxfit = VertexFit::instance();
  vtxfit->init();
  vtxfit->AddTrack(0, wvKm1Trk);
  vtxfit->AddTrack(1, wvKm2Trk);
  vtxfit->AddTrack(2, wvKpTrk);
  vtxfit->AddTrack(3, wvpipTrk);
  vtxfit->AddVertex(0, vxpar, 0, 1);
  if(!vtxfit->Fit(0)) return SUCCESS;
  vtxfit->Swim(0);

  WTrackParameter wKm1 = vtxfit->wtrk(0);
  WTrackParameter wKm2 = vtxfit->wtrk(1);
  WTrackParameter wKp  = vtxfit->wtrk(2);
  WTrackParameter wpip = vtxfit->wtrk(3);

  KalmanKinematicFit* kkmfit = KalmanKinematicFit::instance();

  /// <li> Apply Kalman 4-constrain kinematic fit
  double bestChi2 = 9999.;
  if(fDo_fit4c)
  {
    // * Run over all gamma pairs and find the pair with the best chi2
    kkmfit->init();
    kkmfit->AddTrack(0, wKm1);
    kkmfit->AddTrack(1, wKm2);
    kkmfit->AddTrack(2, wKp);
    kkmfit->AddTrack(3, wpip);
    kkmfit->AddFourMomentum(0, ecms);
    if(kkmfit->Fit())
    {
      double chi2 = kkmfit->chisq();
      if(chi2 < bestChi2)
      {
        bestChi2    = chi2;
        results.Km1 = kkmfit->pfit(0);
        results.Km2 = kkmfit->pfit(1);
        results.Kp  = kkmfit->pfit(2);
        results.pip = kkmfit->pfit(3);
      }
    }
    log << MSG::INFO << " chisq = " << bestChi2 << endmsg;
  }

  /// **Apply cut**: fit4c passed and ChiSq less than fMaxChiSq.
  if(fDo_fit4c && bestChi2 < fMaxChiSq)
  {
    results.comb1.D0  = results.Km1 + results.pip;
    results.comb1.phi = results.Km2 + results.Kp;
    results.comb2.D0  = results.Km2 + results.pip;
    results.comb2.phi = results.Km1 + results.Kp;

    fChi_4C  = bestChi2;

    double m1 = abs(results.comb1.phi.m() - mphi);
    double m2 = abs(results.comb2.phi.m() - mphi);
    if(m1 < m2)
    {
      fMD0_4C  = results.comb1.D0.m();
      fMphi_4C = results.comb1.phi.m();
      fPD0_4C  = ThreeMom(results.comb1.D0);
      fPphi_4C = ThreeMom(results.comb1.phi);
      if(fCheckMC)
      {
        fMC_4C_mD0  = results.comb1.D0.m();
        fMC_4C_mphi = results.comb1.phi.m();
      }
    }
    else
    {
      fMD0_4C  = results.comb2.D0.m();
      fMphi_4C = results.comb2.phi.m();
      fPD0_4C  = ThreeMom(results.comb2.D0);
      fPphi_4C = ThreeMom(results.comb2.phi);
      if(fCheckMC)
      {
        fMC_4C_mD0  = results.comb2.D0.m();
        fMC_4C_mphi = results.comb2.phi.m();
      }
    }

    // * WRITE pi^0 information from EMCal ("fit4c" branch) *
    fTupleFit4C->write(); // "fit4c" branch
    fNCut4++;             // ChiSq has to be less than 200 and fit4c has to be passed
  }

  /// <li> Get MC truth
  bool writeMC = (fMC_4C_mphi < 100.);
  if(fCheckMC && eventHeader->runNumber() < 0 && writeMC)
  {
    fRunid = eventHeader->runNumber();
    fEvtid = eventHeader->eventNumber();
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
    if(!mcParticleCol)
      std::cout << "Could not retrieve McParticelCol" << std::endl;
    else
    {
      fNparticles = 0;
      bool doNotInclude(true);
      int  indexOffset = -1;
      bool incPdcy(false);
      int  rootIndex(-1);

      Event::McParticleCol::iterator it = mcParticleCol->begin();
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
        fPDG[fNparticles] = (*it)->particleProperty();
        if((*it)->mother().particleProperty() == incPid)
          fMother[fNparticles] = (*it)->mother().trackIndex() - rootIndex;
        else
          fMother[fNparticles] = (*it)->mother().trackIndex() - rootIndex - 1;
        if((*it)->particleProperty() == incPid) fMother[fNparticles] = 0;
        fNparticles++;
      }
      fTupleMC->write();
    }
  }

  /// </ol>
  return StatusCode::SUCCESS;
}

// * ========================== * //
// * -------- FINALIZE -------- * //
// * ========================== * //
/// Inherited `finalize` method of `Algorithm`. This function is only called once, after running over all events. Prints the flow chart to the terminal, so **make sure you save this output!**
StatusCode D0phi_3Kpi::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  cout << "Resulting FLOW CHART:" << endl;
  cout << "  Total number of events: " << fNCut0 << endl;
  cout << "  Pass N charged tracks:  " << fNCut1 << endl;
  cout << "  Pass zero net charge    " << fNCut2 << endl;
  cout << "  Pass PID:               " << fNCut3 << endl;
  cout << "  Pass 4C Kalman fit:     " << fNCut4 << endl;
  cout << endl;

  log << MSG::INFO << "Successfully returned from finalize()" << endmsg;
  return StatusCode::SUCCESS;
}