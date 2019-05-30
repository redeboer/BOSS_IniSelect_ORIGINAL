// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
#include "D0omega_K4pi/D0omega_K4pi.h"
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

HepLorentzVector ecms(0.011 * Ecms, 0, 0, Ecms);

const int incPid1 = 91;
const int incPid2 = 92;
const int incPid  = 443;

// * Typedefs * //
typedef vector<int>              Vint;
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

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //
/// Constructor for the `D0omega_K4pi` algorithm.
/// Here, you should declare properties: give them a name, assign a parameter (data member of `D0omega_K4pi`), and if required a documentation string. Note that you should define the paramters themselves in the header (D0omega_K4pi/D0omega_K4pi.h) and that you should assign the values in `share/jopOptions_D0omega_K4pi.txt`. Algorithms should inherit from Gaudi's `Algorithm` class. See https://dayabay.bnl.gov/dox/GaudiKernel/html/classAlgorithm.html.
D0omega_K4pi::D0omega_K4pi(const string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
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
  declareProperty("CheckMCtruth", fCheckMC);
  declareProperty("CheckVertex", fCheckVertex);
  declareProperty("CheckPhoton", fCheckPhoton);
  declareProperty("CheckDedx", fCheckDedx);
  declareProperty("CheckTof", fCheckTof);
  declareProperty("CheckPID", fCheckPID);
  declareProperty("CheckEtot", fCheckEtot);
}

// * ========================== * //
// * ------- INITIALIZE ------- * //
// * ========================== * //
/// (Inherited) `initialize` step of `Algorithm`. This function is called only once in the beginning. **Define and load `NTuple`s here.**
StatusCode D0omega_K4pi::initialize()
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

  /// <tr><td colspan="2">**`NTuple "photon"`: Photon kinematics**</td></tr>
  if(fCheckVertex)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/photon");
    if(nt)
      fTupleAngles = nt;
    else
    {
      fTupleAngles = ntupleSvc()->book("FILE1/photon", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleAngles)
      {
        fTupleAngles->addItem("dthe", fDeltaTheta);
        /// <tr><td>`"dthe"`</td><td>\f$\theta\f$ angle difference with nearest charged track (degrees)</td></tr>
        fTupleAngles->addItem("dphi", fDeltaPhi);
        /// <tr><td>`"dphi"`</td><td>\f$\phi\f$ angle difference with nearest charged track (degrees)</td></tr>
        fTupleAngles->addItem("dang", fDeltaAngle);
        /// <tr><td>`"dang"`</td><td>Angle difference with nearest charged track</td></tr>
        fTupleAngles->addItem("eraw", fEraw);
        /// <tr><td>`"eraw"`</td><td>Energy of the photon</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleAngles) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "etot"`: Energy branch**</td></tr>
  if(fCheckEtot)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/etot");
    if(nt)
      fTupleMgg = nt;
    else
    {
      fTupleMgg = ntupleSvc()->book("FILE1/etot", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleMgg)
      {
        fTupleMgg->addItem("m2gg", fMtoGG);
        /// <tr><td>`"m2gg"`</td><td>Invariant mass of the two gammas</td></tr>
        fTupleMgg->addItem("etot", fEtot);
        /// <tr><td>`"etot"`</td><td>Total energy of \f$\pi^+\f$, \f$\pi^-\f$ and the two gammas</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleMgg) << endmsg;
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
        fTupleFit4C->addItem("momega", fMomega_4C);
        /// <tr><td>`"momega"`</td><td>Reconstructed invariant \f$\pi^0\f$ mass</td></tr>
        fTupleFit4C->addItem("ppi0", fPpi0_4C);
        /// <tr><td>`"mpi0"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
        fTupleFit4C->addItem("pD0", fPD0_4C);
        /// <tr><td>`"mD0"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
        fTupleFit4C->addItem("pomega", fPomega_4C);
        /// <tr><td>`"momega"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleFit4C) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  /// <tr><td colspan="2">**`NTuple "fit5c"`:  5-constraint fit branch**</td></tr>
  if(fDo_fit5c)
  {
    NTuplePtr nt(ntupleSvc(), "FILE1/fit5c");
    if(nt)
      fTupleFit5C = nt;
    else
    {
      fTupleFit5C = ntupleSvc()->book("FILE1/fit5c", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if(fTupleFit5C)
      {
        fTupleFit5C->addItem("chi2", fChi_5C);
        /// <tr><td>`"chi2"` </td><td>\f$\chi^2\f$ of the Kalman kinematic fit</td></tr>
        fTupleFit5C->addItem("mpi0", fMpi0_5C);
        /// <tr><td>`"mpi0"`</td><td>Reconstructed invariant \f$\pi^0\f$ mass</td></tr>
        fTupleFit5C->addItem("mD0", fMD0_5C);
        /// <tr><td>`"mD0"`</td><td>Reconstructed invariant \f$\pi^0\f$ mass</td></tr>
        fTupleFit5C->addItem("momega", fMomega_5C);
        /// <tr><td>`"momega"`</td><td>Reconstructed invariant \f$\pi^0\f$ mass</td></tr>
        fTupleFit5C->addItem("ppi0", fPpi0_5C);
        /// <tr><td>`"mpi0"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
        fTupleFit5C->addItem("pD0", fPD0_5C);
        /// <tr><td>`"mD0"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
        fTupleFit5C->addItem("pomega", fPomega_5C);
        /// <tr><td>`"momega"`</td><td>Reconstructed 3-momentum \f$\pi^0\f$ mass</td></tr>
        fTupleFit5C->addItem("fcos", fFcos);
        /// <tr><td>`"fcos"`</td><td>\f$E/|\vec{p}|\f$ ratio for \f$\pi^0\f$ candidate</td></tr>
        fTupleFit5C->addItem("elow", fElow);
        /// <tr><td>`"elow"`</td><td>Lowest energy of the two photons</td></tr>
      }
      else
      {
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(fTupleFit5C) << endmsg;
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
        fTupleMC->addItem("momega_fit4c", fMC_4C_momega);
        fTupleMC->addItem("mD0_fit5c", fMC_5C_mD0);
        fTupleMC->addItem("momega_fit5c", fMC_5C_momega);
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
        fTupleCutFlow->addItem("Pass_N_gammas", fNCut3);
        fTupleCutFlow->addItem("Pass_PID", fNCut4);
        fTupleCutFlow->addItem("Pass_4C_Kalman_fit", fNCut5);
        fTupleCutFlow->addItem("Pass_5C_Kalman_fit", fNCut6);
        fNCut0 = 0;
        fNCut1 = 0;
        fNCut2 = 0;
        fNCut3 = 0;
        fNCut4 = 0;
        fNCut5 = 0;
        fNCut6 = 0;
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
StatusCode D0omega_K4pi::execute()
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
    fMC_4C_mD0    = 9999.;
    fMC_4C_momega = 9999.;
    fMC_5C_mD0    = 9999.;
    fMC_5C_momega = 9999.;
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

  /// <li> LOOP OVER NEUTRAL TRACKS: select photons
  /// ** Uses `fNCut3` counter**: number of good photons has to be 2 at least.
  /// The second part of the set of reconstructed events consists of the neutral tracks, that is, the photons detected by the EMC (by clustering EMC crystal energies). Each neutral track is paired with each charged track and if their angle is smaller than a certain value (here, 200), the photon track is stored as 'good photon' (added to `iGam`).
  Vint iGam;
  for(int i = evtRecEvent->totalCharged(); i < evtRecEvent->totalTracks(); ++i)
  {

    // * Get track
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower* emcTrk = (*itTrk)->emcShower();
    Hep3Vector    emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());

    // * Find the theta, phi, and angle difference with nearest charged track
    double dthe = 200.; // start value for difference in theta
    double dphi = 200.; // start value for difference in phi
    double dang = 200.; // start value for difference in angle (?)
    for(int j = 0; j < evtRecEvent->totalCharged(); j++)
    {
      EvtRecTrackIterator jtTrk = evtRecTrkCol->begin() + j;
      if(!(*jtTrk)->isExtTrackValid()) continue;
      RecExtTrack* extTrk = (*jtTrk)->extTrack();
      if(extTrk->emcVolumeNumber() == -1) continue;
      Hep3Vector extpos = extTrk->emcPosition();
      // double ctht = extpos.cosTheta(emcpos);
      double angd = extpos.angle(emcpos);
      double thed = extpos.theta() - emcpos.theta();
      double phid = extpos.deltaPhi(emcpos);
      thed        = fmod(thed + fivepi, CLHEP::twopi) - CLHEP::pi;
      phid        = fmod(phid + fivepi, CLHEP::twopi) - CLHEP::pi;
      if(angd < dang)
      {
        dang = angd;
        dthe = thed;
        dphi = phid;
      }
    }

    // * Apply angle cut
    if(dang >= 200) continue;
    double eraw = emcTrk->energy();
    dthe        = dthe * DegToRad;
    dphi        = dphi * DegToRad;
    dang        = dang * DegToRad;

    // * WRITE photon info ("photon" branch)
    if(fCheckPhoton)
    {
      fDeltaTheta = dthe;    // theta difference with nearest charged track (degrees)
      fDeltaPhi   = dphi;    // phi difference with nearest charged track (degrees)
      fDeltaAngle = dang;    // angle difference with nearest charged track
      fEraw       = eraw;    // energy of the photon
      fTupleAngles->write(); // "photon" branch
    }

    // * Apply photon cuts
    if(eraw < fEnergyThreshold) continue;
    if((fabs(dthe) < fGammaThetaCut) && (fabs(dphi) < fGammaPhiCut)) continue;
    if(fabs(dang) < fGammaAngleCut) continue;

    // * Add photon track to vector
    iGam.push_back(i);
  }

  // * Finish Good Photon Selection *
  int nGam = iGam.size();
  log << MSG::DEBUG << "Number of good photons: " << nGam << "/" << evtRecEvent->totalNeutral()
      << endmsg;
  if(nGam < 2) return StatusCode::SUCCESS;
  fNCut3++;

  /// <li> Check charged track dEdx PID information
  if(fCheckDedx)
  {
    for(int i = 0; i < nGood; ++i)
    {

      // * Get track *
      EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i];
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if(!(*itTrk)->isMdcDedxValid()) continue;
      RecMdcTrack* mdcTrk  = (*itTrk)->mdcTrack();
      RecMdcDedx*  dedxTrk = (*itTrk)->mdcDedx();

      // * WRITE energy loss PID info ("dedx" branch) *
      fPtrack = mdcTrk->p();             // momentum of the track
      fChi2e  = dedxTrk->chiE();         // chi2 in case of electron
      fChi2mu = dedxTrk->chiMu();        // chi2 in case of muon
      fChi2pi = dedxTrk->chiPi();        // chi2 in case of pion
      fChi2k  = dedxTrk->chiK();         // chi2 in case of kaon
      fChi2p  = dedxTrk->chiP();         // chi2 in case of proton
      fProbPH = dedxTrk->probPH();       // most probable pulse height from truncated mean
      fNormPH = dedxTrk->normPH();       // normalized pulse height
      fGhit   = dedxTrk->numGoodHits();  // number of good hits
      fThit   = dedxTrk->numTotalHits(); // total number of hits
      fTupleDedx->write();               // "dedx" branch
    }
  }

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

  /// <li> Get 4-momentum for each photon
  Vp4 pGam;
  for(int i = 0; i < nGam; ++i)
  {
    EvtRecTrackIterator itTrk  = evtRecTrkCol->begin() + iGam[i];
    RecEmcShower*       emcTrk = (*itTrk)->emcShower();
    double              eraw   = emcTrk->energy();
    double              phi    = emcTrk->phi();
    double              the    = emcTrk->theta();
    HepLorentzVector    ptrk;
    ptrk.setPx(eraw * sin(the) * cos(phi));
    ptrk.setPy(eraw * sin(the) * sin(phi));
    ptrk.setPz(eraw * cos(the));
    ptrk.setE(eraw);
    ptrk = ptrk.boost(-0.011, 0, 0); // boost to cms
    pGam.push_back(ptrk);
  }
  // cout << "before pid" << endl;

  /// <li> Get 4-momentum for each charged track
  Vint        ipip, ipim, iKm; // vector of indices of good tracks
  Vp4         ppip, ppim, pKm; // vector of momenta
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
        ptrk = ptrk.boost(-0.011, 0, 0); // boost to cms
        ppip.push_back(ptrk);
      }
      else
      {
        ipim.push_back(iGood[i]);
        HepLorentzVector ptrk;
        ptrk.setPx(mdcKalTrk->px());
        ptrk.setPy(mdcKalTrk->py());
        ptrk.setPz(mdcKalTrk->pz());
        double p3 = ptrk.mag();
        ptrk.setE(sqrt(p3 * p3 + mpi * mpi));
        ptrk = ptrk.boost(-0.011, 0, 0); // boost to cms
        ppim.push_back(ptrk);
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
        ptrk = ptrk.boost(-0.011, 0, 0); // boost to cms
        pKm.push_back(ptrk);
      }
    }
  }

  /// **Apply cut**: PID
  if(iKm.size() != 1) return SUCCESS;
  if(ipim.size() != 1) return SUCCESS;
  if(ipip.size() != 2) return SUCCESS;
  fNCut4++;

  /// <li> Loop over each gamma pair and store total energy
  if(fCheckEtot)
  {
    HepLorentzVector pTot;
    for(int i = 0; i < nGam - 1; ++i)
    {
      for(int j = i + 1; j < nGam; j++)
      {
        HepLorentzVector p2g = pGam[i] + pGam[j];
        pTot                 = ppip[0] + ppim[0];
        pTot += p2g;

        // * WRITE total energy and pi^0 candidate inv. mass ("etot" branch) *
        fMtoGG = p2g.m();   // invariant mass of the two gammas
        fEtot  = pTot.e();  // total energy of pi^+, pi^ and the two gammas
        fTupleMgg->write(); // "etot" branch
      }
    }
  }

  RecMdcKalTrack* KmTrk   = (*(evtRecTrkCol->begin() + iKm[0]))->mdcKalTrack();
  RecMdcKalTrack* pimTrk  = (*(evtRecTrkCol->begin() + ipim[0]))->mdcKalTrack();
  RecMdcKalTrack* pip1Trk = (*(evtRecTrkCol->begin() + ipip[0]))->mdcKalTrack();
  RecMdcKalTrack* pip2Trk = (*(evtRecTrkCol->begin() + ipip[1]))->mdcKalTrack();

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

  VertexFit* vtxfit = VertexFit::instance();
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

  KalmanKinematicFit* kkmfit = KalmanKinematicFit::instance();

  /// <li> Apply Kalman 4-constrain kinematic fit
  if(fDo_fit4c)
  {
    // * Run over all gamma pairs and find the pair with the best chi2
    double bestChi2 = 9999.;
    for(int i = 0; i < nGam - 1; ++i)
    {
      RecEmcShower* g1Trk = (*(evtRecTrkCol->begin() + iGam[i]))->emcShower();
      for(int j = i + 1; j < nGam; j++)
      {
        RecEmcShower* g2Trk = (*(evtRecTrkCol->begin() + iGam[j]))->emcShower();
        kkmfit->init();
        kkmfit->AddTrack(0, wKm);         // neg. kaon
        kkmfit->AddTrack(1, wpim);        // neg. pion
        kkmfit->AddTrack(2, wpip1);       // pos. pion 1
        kkmfit->AddTrack(3, wpip2);       // pos. pion 2
        kkmfit->AddTrack(4, 0.0, g1Trk);  // gamma track 1
        kkmfit->AddTrack(5, 0.0, g2Trk);  // gamma track 2
        kkmfit->AddFourMomentum(0, ecms); // 4 constraints: CMS energy and momentum
        if(kkmfit->Fit())
        {
          double chi2 = kkmfit->chisq();
          if(chi2 < bestChi2)
          {
            bestChi2     = chi2;
            results.Km   = kkmfit->pfit(0);
            results.pim  = kkmfit->pfit(1);
            results.pip1 = kkmfit->pfit(2);
            results.pip2 = kkmfit->pfit(3);
            results.g1   = kkmfit->pfit(4);
            results.g2   = kkmfit->pfit(5);
            results.pi0  = results.g1 + results.g2;
          }
        }
      }
    }

    log << MSG::INFO << " chisq = " << bestChi2 << endmsg;

    /// **Apply cut**: fit4c passed and ChiSq less than fMaxChiSq.
    if(bestChi2 < fMaxChiSq)
    {
      results.comb1.D0    = results.pip1 + results.Km;
      results.comb1.omega = results.pip2 + results.pim + results.pi0;
      results.comb2.D0    = results.pip2 + results.Km;
      results.comb2.omega = results.pip1 + results.pim + results.pi0;

      fChi_4C  = bestChi2;
      fMpi0_4C = results.pi0.m();
      fPpi0_4C = results.pi0.rho();

      double m1 = abs(results.comb1.omega.m() - momega);
      double m2 = abs(results.comb2.omega.m() - momega);
      if(m1 < m2)
      {
        fMD0_4C    = results.comb1.D0.m();
        fMomega_4C = results.comb1.omega.m();
        fPD0_4C    = results.comb1.D0.rho();
        fPomega_4C = results.comb1.omega.rho();
        if(fCheckMC)
        {
          fMC_4C_mD0    = results.comb1.D0.m();
          fMC_4C_momega = results.comb1.omega.m();
        }
      }
      else
      {
        fMD0_4C    = results.comb2.D0.m();
        fMomega_4C = results.comb2.omega.m();
        fPD0_4C    = results.comb2.D0.rho();
        fPomega_4C = results.comb2.omega.rho();
        if(fCheckMC)
        {
          fMC_4C_mD0    = results.comb2.D0.m();
          fMC_4C_momega = results.comb2.omega.m();
        }
      }

      // * WRITE pi^0 information from EMCal ("fit4c" branch) *
      fTupleFit4C->write(); // "fit4c" branch
      fNCut5++;              // ChiSq has to be less than 200 and fit4c has to be passed
    }
  }

  /// <li> Apply Kalman kinematic fit
  if(fDo_fit5c)
  {
    // * Find the best combination over all possible pi+ pi- gamma gamma pair
    double bestChi2 = 9999.;
    for(int i = 0; i < nGam - 1; ++i)
    {
      RecEmcShower* g1Trk = (*(evtRecTrkCol->begin() + iGam[i]))->emcShower();
      for(int j = i + 1; j < nGam; j++)
      {
        RecEmcShower* g2Trk = (*(evtRecTrkCol->begin() + iGam[j]))->emcShower();
        kkmfit->init();
        kkmfit->AddTrack(0, wKm);            // neg. kaon
        kkmfit->AddTrack(1, wpim);           // neg. pion
        kkmfit->AddTrack(2, wpip1);          // pos. pion 1
        kkmfit->AddTrack(3, wpip2);          // pos. pion 2
        kkmfit->AddTrack(4, 0.0, g1Trk);     // gamma track 1
        kkmfit->AddTrack(5, 0.0, g2Trk);     // gamma track 2
        kkmfit->AddFourMomentum(0, ecms);    // 4 constraints: CMS energy and momentum
        kkmfit->AddResonance(1, mpi0, 4, 5); // 5th constraint: pi0 resonance
        if(!kkmfit->Fit(0)) continue;
        if(!kkmfit->Fit(1)) continue;
        if(kkmfit->Fit())
        {
          double chi2 = kkmfit->chisq();
          if(chi2 < bestChi2)
          {
            bestChi2     = chi2;
            results.Km   = kkmfit->pfit(0);
            results.pim  = kkmfit->pfit(1);
            results.pip1 = kkmfit->pfit(2);
            results.pip2 = kkmfit->pfit(3);
            results.g1   = kkmfit->pfit(4);
            results.g2   = kkmfit->pfit(5);
            results.pi0  = results.g1 + results.g2;
          }
        }
      }
    }

    log << MSG::INFO << " chisq = " << bestChi2 << endmsg;

    /// **Apply cut**: fit5c passed and ChiSq less than fMaxChiSq.
    if(bestChi2 < fMaxChiSq)
    {
      results.comb1.D0    = results.pip1 + results.Km;
      results.comb1.omega = results.pip2 + results.pim + results.pi0;
      results.comb2.D0    = results.pip2 + results.Km;
      results.comb2.omega = results.pip1 + results.pim + results.pi0;

      fChi_5C  = bestChi2;
      fMpi0_5C = results.pi0.m();
      fPpi0_5C = results.pi0.rho();

      double m1 = abs(results.comb1.omega.m() - momega);
      double m2 = abs(results.comb2.omega.m() - momega);
      if(m1 < m2)
      {
        fMD0_5C    = results.comb1.D0.m();
        fMomega_5C = results.comb1.omega.m();
        fPD0_5C    = results.comb1.D0.rho();
        fPomega_5C = results.comb1.omega.rho();
        if(fCheckMC)
        {
          fMC_5C_mD0    = results.comb1.D0.m();
          fMC_5C_momega = results.comb1.omega.m();
        }
      }
      else
      {
        fMD0_5C    = results.comb2.D0.m();
        fMomega_5C = results.comb2.omega.m();
        fPD0_5C    = results.comb2.D0.rho();
        fPomega_5C = results.comb2.omega.rho();
        if(fCheckMC)
        {
          fMC_5C_mD0    = results.comb2.D0.m();
          fMC_5C_momega = results.comb2.omega.m();
        }
      }

      // * Photon kinematics * //
      double eg1 = results.g1.e();
      double eg2 = results.g2.e();
      fFcos      = (eg1 - eg2) / results.pi0.rho(); // E/p ratio for pi^0 candidate
      fElow      = (eg1 < eg2) ? eg1 : eg2;         // lowest energy of the two gammas

      // * WRITE pi^0 information from EMCal ("fit5c" branch) *
      fTupleFit5C->write(); // "fit5c" branch
      fNCut6++;              // ChiSq has to be less than 200 and fit5c has to be passed
    }
  }

  /// <li> Get MC truth
  bool writeMC = (fMC_4C_momega < 100.) || (fMC_5C_momega < 100.);
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
StatusCode D0omega_K4pi::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  cout << "Resulting FLOW CHART:" << endl;
  cout << "  Total number of events: " << fNCut0 << endl;
  cout << "  Pass N charged tracks:  " << fNCut1 << endl;
  cout << "  Pass zero net charge    " << fNCut2 << endl;
  cout << "  Pass N gammas:          " << fNCut3 << endl;
  cout << "  Pass PID:               " << fNCut4 << endl;
  cout << "  Pass 4C Kalman fit:     " << fNCut5 << endl;
  cout << "  Pass 5C Kalman fit:     " << fNCut6 << endl;
  cout << endl;

  log << MSG::INFO << "Successfully returned from finalize()" << endmsg;
  return StatusCode::SUCCESS;
}