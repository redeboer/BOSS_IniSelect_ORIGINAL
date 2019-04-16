// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "D0omega_K4pi/D0omega_K4pi.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "VertexFit/KalmanKinematicFit.h"
#include <string>
#include <utility>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using namespace TSGlobals;

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //

/// Constructor for the `D0omega_K4pi` algorithm, derived from `TrackSelector`.
/// Here, you should declare properties: give them a name, assign a parameter (data member of `D0omega_K4pi`), and if required a documentation string. Note that you should define the paramters themselves in the header (D0omega_K4pi/D0omega_K4pi.h) and that you should assign the values in `share/D0omega_K4pi.txt`.
D0omega_K4pi::D0omega_K4pi(const std::string& name, ISvcLocator* pSvcLocator) :
  /// * Construct base algorithm `TrackSelector`.
  TrackSelector(name, pSvcLocator),
  /// * Construct `NTuple::Tuple` containers used in derived classes.
  fNTuple_dedx_K("dedx_K", "dE/dx of the kaons"),
  fNTuple_dedx_pi("dedx_pi", "dE/dx of the pions"),
  fNTuple_fit4c_all("fit4c_all", "4-constraint fit information (CMS 4-momentum)"),
  fNTuple_fit4c_best(
    "fit4c_best",
    "4-constraint fit information of the invariant masses closest to the reconstructed particles"),
  fNTuple_fit_mc("fit_mc", "Fake fit information according to MC truth"),
  fNTuple_photon("photon", "Kinematics of selected photons"),
  /// * Construct counters (in essence a `CutObject` without cuts).
  fCutFlow_NChargedOK("N_charged_OK", "Number of events that had exactly 4 charged tracks"),
  fCutFlow_NFitOK("N_Fit_OK", "Number of combinations where where the kinematic fit worked"),
  fCutFlow_NPIDnumberOK("N_PID_OK",
                        "Number of events that had exactly 2 K-, 1 K+ and 1 pi+ PID tracks"),
  /// * Construct additional `CutObject`s that are specific for the `rhopi_pipig` package.
  fCut_GammaAngle("gamma_angle"),
  fCut_GammaPhi("gamma_phi"),
  fCut_GammaTheta("gamma_theta")
{
  PrintFunctionName("D0omega_K4pi", __func__);
  PostConstructor();
  fCreateChargedCollection = true;
  fCreateNeutralCollection = true;
}

// * =============================== * //
// * ------- INITIALIZE STEP ------- * //
// * =============================== * //

/// (Inherited) `initialize` step of `Algorithm`.
/// This function is called only once, when the algorithm is initialised.
/// @remark Define and load `NTuple`s here. Other `NTuple`s have already been defined in the TrackSelector::initilize` step prior to this this method.
StatusCode D0omega_K4pi::initialize_rest()
{
  PrintFunctionName("D0omega_K4pi", __func__);
  try
  {
    AddNTuples_mult_sel();
    AddNTuples_dedx();
    AddNTuples_fit();
    AddNTuples_photon();
    AddAdditionalNTuples_topology();
  }
  catch(...)
  {
  } /// @todo Try to catch the `StatusCode`. Note that this is problematic, because you actually want to catch the `enum` within the class `StatusCode`...
  return StatusCode::SUCCESS;
}

/// `"mult_select"`: Multiplicities of selected particles.
void D0omega_K4pi::AddNTuples_mult_sel()
{
  fNTuple_mult_sel.AddItem<int>("NPhotons"); /// * `"NPhotons"`: Number of \f$\gamma\f$.
  fNTuple_mult_sel.AddItem<int>("NKaonPos"); /// * `"NKaonPos"`: Number of \f$K^+\f$.
  fNTuple_mult_sel.AddItem<int>("NKaonNeg"); /// * `"NKaonNeg"`: Number of \f$K^-\f$.
  fNTuple_mult_sel.AddItem<int>("NPionPos"); /// * `"NPionPos"`: Number of \f$\pi^-\f$.
}

/// `"dedx_K"` and `"dedx_pi"`: energy loss \f$dE/dx\f$ PID branch. See `TrackSelector::AddNTupleItems_Dedx` for more info.
void D0omega_K4pi::AddNTuples_dedx()
{
  AddNTupleItems_Dedx(fNTuple_dedx_K);
  AddNTupleItems_Dedx(fNTuple_dedx_pi);
}

/// `"fit4c_*"`: Kinematic fit results. See `TrackSelector::AddNTupleItems_Fit` for more info.
void D0omega_K4pi::AddNTuples_fit()
{
  AddNTupleItems_Fit(fNTuple_fit4c_all);
  AddNTupleItems_Fit(fNTuple_fit4c_best);
  AddNTupleItems_Fit(fNTuple_fit_mc);
}

/// `"photon"`: Information of the selected photons
void D0omega_K4pi::AddNTuples_photon()
{
  /// * `"E"`: Energy of the photon.
  /// * `"py"`: \f$x\f$ component of the 4-momentum of the photon (computed from the detected angles).
  /// * `"py"`: \f$y\f$ component of the 4-momentum of the photon (computed from the detected angles).
  /// * `"pz"`: \f$z\f$ component of the 4-momentum of the photon (computed from the detected angles).
  /// * `"phi"`:   Smallest \f$\phi\f$ angle between the photon and the nearest charged pion.
  /// * `"theta"`: Smallest \f$\theta\f$ angle between the photon and the nearest charged pion.
  /// * `"angle"`: Smallest angle between the photon and the nearest charged pion.
  fNTuple_photon.AddItem<double>("E");
  fNTuple_photon.AddItem<double>("px");
  fNTuple_photon.AddItem<double>("py");
  fNTuple_photon.AddItem<double>("pz");
  fNTuple_photon.AddItem<double>("smallest_phi");
  fNTuple_photon.AddItem<double>("smallest_theta");
  fNTuple_photon.AddItem<double>("smallest_angle");
}

/// `"topology"`: Add *additional* mass branches to the MC truth branch for the `topoana` package
void D0omega_K4pi::AddAdditionalNTuples_topology()
{
  /// * `"chi2"`: \f$\chi^2\f$ of the Kinematic kalman fit.
  /// * `"mpi0"`: Invariant mass of the \f$\pi^0\f$ candidate in \f$D^0 \to K^-\pi^+\pi^0\f$.
  /// * `"mD0"`:  Invariant mass of the \f$D^0\f$ candidate in the \f$J/\psi \to D^0\omega\f$.
  /// * `"momega"`: Invariant mass of the \f$\omega\f$ candidate in the \f$J/\psi \to D^0\omega\f$.
  /// * `"pD0"`:  Momentum of the \f$D^0\f$ candidate in the \f$J/\psi \to D^0\omega\f$.
  /// * `"pomega"`: Momentum of the \f$\omega\f$ candidate in the \f$J/\psi \to D^0\omega\f$.
  fNTuple_topology.AddItem<double>("chi2");
  fNTuple_topology.AddItem<double>("mpi0");
  fNTuple_topology.AddItem<double>("mD0");
  fNTuple_topology.AddItem<double>("momega");
  fNTuple_topology.AddItem<double>("pD0");
  fNTuple_topology.AddItem<double>("pomega");
  fNTuple_topology.AddItem<double>("ppi0");
  fNTuple_topology.AddItem<double>("pK-");
  fNTuple_topology.AddItem<double>("ppi-");
  fNTuple_topology.AddItem<double>("ppi+1");
  fNTuple_topology.AddItem<double>("ppi+2");
  fNTuple_topology.AddItem<double>("fDalitzOmega_pi-pi+");
  fNTuple_topology.AddItem<double>("fDalitzOmega_pi0pi-");
  fNTuple_topology.AddItem<double>("fDalitzOmega_pi0pi+");
  fNTuple_topology.AddItem<double>("fRelativePhotonAngle");
}

/// This function encapsulates the `addItem` procedure for the fit branches.
void D0omega_K4pi::AddNTupleItems_Fit(NTupleContainer& tuple)
{
  if(!tuple.DoWrite()) return;
  /// * `"mpi0"`:  Invariant mass for \f$gamma\gamma\f$ (\f$\pi^0\f$).
  /// * `"mD0"`:   Invariant mass for \f$K^-\pi^+\f$ (\f$D^0\f$).
  /// * `"momega"`:  Invariant mass for \f$K^+K^+\f$ (\f$\omega\f$).
  /// * `"mJpsi"`: Invariant mass for \f$D^0 \omega \f$ (\f$J/\psi\f$).
  /// * `"pD0"`:   3-momentum mass for the combination \f$K^- \pi^+\f$ (\f$D^0\f$ candidate).
  /// * `"pomega"`:  3-momentum mass for the combination \f$K^+ K^+ \f$ (\f$\omega\f$ candidate).
  /// * `"chisq"`: \f$\chi^2\f$ of the Kalman kinematic fit.
  tuple.AddItem<double>("mpi0");
  tuple.AddItem<double>("mD0");
  tuple.AddItem<double>("momega");
  tuple.AddItem<double>("mJpsi");
  tuple.AddItem<double>("pD0");
  tuple.AddItem<double>("pomega");
  tuple.AddItem<double>("chisq");
}

// * ============================ * //
// * ------- EXECUTE STEP ------- * //
// * ============================ * //

/// Inherited `execute` method of the `Algorithm` that is called *for each event*.
StatusCode D0omega_K4pi::execute_rest()
{
  PrintFunctionName("D0omega_K4pi", __func__);
  try
  {
    CutNumberOfChargedParticles();
    CreateChargedTrackSelections();
    CreateNeutralTrackSelections();
    WriteMultiplicities();
    CutPID();
    WriteDedxOfSelectedParticles();
    FindBestKinematicFit();
  }
  catch(...)
  {}
  return StatusCode::SUCCESS;
}

/// **Charged track cut**: Apply a strict cut on the number of particles -- only **4 charged tracks in total**.
void D0omega_K4pi::CutNumberOfChargedParticles()
{
  if(fChargedTracks.size() != 4) throw StatusCode::SUCCESS;
  ++fCutFlow_NChargedOK;
}

/// Create selections of **charged** tracks: \f$K^-\f$, \f$\pi^+\f$, and \f$\pi^+\f$.
void D0omega_K4pi::CreateChargedTrackSelections()
{
  fKaonNeg.clear();
  fPionPos.clear();
  fPionNeg.clear();
  for(fTrackIter = fChargedTracks.begin(); fTrackIter != fChargedTracks.end(); ++fTrackIter)
  {
    if(!InitializePID()) continue;
    WritePIDInformation();
    CategorizeTrack(*fTrackIter);
  }
}

/// Specialized initialise PID method for D0omega_K4pi. @see TrackSelector::InitializePID.
bool D0omega_K4pi::InitializePID()
{
  return TrackSelector::InitializePID(
    /// * Use **probability method**.
    fPIDInstance->methodProbability(),
    /// * Use \f$dE/dx\f$ and the three ToF detectors. Since data reconstructed with BOSS 7.0.4, `ParticleID::useTofCorr()` should be used for ToF instead of e.g. `useTof1`.
    fPIDInstance->useDedx() | fPIDInstance->useTof1() | fPIDInstance->useTof2() |
      fPIDInstance->useTofE(),
    /// * Identify only pions and kaons.
    fPIDInstance->onlyPion() | fPIDInstance->onlyKaon(),
    /// * Use \f$\chi^2 > 4.0\f$.
    4.0);
}

/// Identify type of charged particle and add to related vector.
bool D0omega_K4pi::CategorizeTrack(EvtRecTrack* track)
{
  fTrackKal = track->mdcKalTrack();
  if(fPIDInstance->probPion() > fPIDInstance->probKaon())
  {
    if(fCut_PIDProb.FailsMin(fPIDInstance->probPion())) return false;
    RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
    if(fTrackKal->charge() > 0)
      fPionPos.push_back(track);
    else
      fPionNeg.push_back(track);
  }
  else
  {
    if(fCut_PIDProb.FailsMin(fPIDInstance->probKaon())) return false;
    RecMdcKalTrack::setPidType(RecMdcKalTrack::kaon);
    if(fTrackKal->charge() < 0) fKaonNeg.push_back(track);
  }
}

/// Create selection **neutral** tracks (photons).
void D0omega_K4pi::CreateNeutralTrackSelections()
{
  fPhotons.clear();
  for(fTrackIter = fNeutralTracks.begin(); fTrackIter != fNeutralTracks.end(); ++fTrackIter)
  {
    AngleDifferences smallestAngles = FindSmallestPhotonAngles();
    smallestAngles.ConvertToDegrees();
    WritePhotonKinematics(smallestAngles);
    if(CutPhotonAngles(smallestAngles)) continue;
    fPhotons.push_back(*fTrackIter);
  }
}

/// Find angle differences with nearest charged track.
AngleDifferences D0omega_K4pi::FindSmallestPhotonAngles()
{
  GetEmcPosition();
  AngleDifferences smallestAngles;
  for(fTrackIter2 = fChargedTracks.begin(); fTrackIter2 != fChargedTracks.end(); ++fTrackIter2)
  {
    if(!GetExtendedEmcPosition()) continue;
    AngleDifferences angles(fExtendedEmcPosition, fEmcPosition);
    if(angles.IsSmaller(smallestAngles)) smallestAngles = angles;
  }
  return smallestAngles;
}

void D0omega_K4pi::GetEmcPosition()
{
  fTrackEMC    = (*fTrackIter)->emcShower();
  fEmcPosition = fTrackEMC->position();
}

bool D0omega_K4pi::GetExtendedEmcPosition()
{
  if(!(*fTrackIter2)->isExtTrackValid()) return false;
  fTrackExt = (*fTrackIter2)->extTrack();
  if(fTrackExt->emcVolumeNumber() == -1) return false;
  fExtendedEmcPosition = fTrackExt->emcPosition();
  return true;
}

/// *Write* photon info (`"photon"` branch).
void D0omega_K4pi::WritePhotonKinematics(const AngleDifferences& angles)
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
bool D0omega_K4pi::CutPhotonAngles(const AngleDifferences& angles)
{
  if(!fCut_GammaAngle.FailsCut(angles.GetAbsoluteAngle())) return false;
  if(!fCut_GammaPhi.FailsCut(angles.GetAbsolutePhi())) return false;
  if(!fCut_GammaTheta.FailsCut(angles.GetAbsoluteTheta())) return false;
  return true;
}

/// **Write** the multiplicities of the selected particles.
void D0omega_K4pi::WriteMultiplicities()
{
  PrintMultiplicities();
  if(!fNTuple_mult_sel.DoWrite()) return;
  fNTuple_mult_sel.GetItem<int>("NKaonNeg") = fKaonNeg.size();
  fNTuple_mult_sel.GetItem<int>("NPhoton")  = fPhotons.size();
  fNTuple_mult_sel.GetItem<int>("NPionNeg") = fPionNeg.size();
  fNTuple_mult_sel.GetItem<int>("NPionPos") = fPionPos.size();
  fNTuple_mult_sel.Write();
}

/// Print multiplicities of the selected particles.
void D0omega_K4pi::PrintMultiplicities()
{
  fLog << MSG::INFO << "N_{K^-} = " << fKaonNeg.size() << ", "
       << "N_{\pi^+} = " << fPionPos.size() << ", "
       << "N_{\pi^-} = " << fPionNeg.size() << ", "
       << "N_{\gamma} = " << fPhotons.size() << endmsg;
}

/// **PID cut**: apply a strict cut on the number of the selected particles.
void D0omega_K4pi::CutPID()
{
  /// Only continue if:
  if(fKaonNeg.size() != 1) throw StatusCode::SUCCESS; /// * 1 negative kaons
  if(fPhotons.size() < 2) throw StatusCode::SUCCESS;  /// * at least 2 photons (\f$\gamma\f$'s)
  if(fPionNeg.size() != 1) throw StatusCode::SUCCESS; /// * 1 negative pion
  if(fPionPos.size() != 2) throw StatusCode::SUCCESS; /// * 2 positive pions
  ++fCutFlow_NPIDnumberOK;                            /// Then increase cut flow counter for PID.
  std::cout << "N_{K^-} = " << fKaonNeg.size() << ", "
            << "N_{\pi^+} = " << fPionPos.size() << ", "
            << "N_{\pi^-} = " << fPionNeg.size() << ", "
            << "N_{\gamma} = " << fPhotons.size() << std::endl;
  std::cout << "PID selection passed for (run, event) = (" << fEventHeader->runNumber() << ", "
            << fEventHeader->eventNumber() << ")" << std::endl;
}

/// **Write** \f$dE/dx\f$ PID information (`"dedx_*"` branchs).
void D0omega_K4pi::WriteDedxOfSelectedParticles()
{
  WriteDedxInfoForVector(fKaonNeg, fNTuple_dedx_K);
  WriteDedxInfoForVector(fPionNeg, fNTuple_dedx_pi);
  WriteDedxInfoForVector(fPionPos, fNTuple_dedx_pi);
}

/// Specification of what should be written to the fit `NTuple`.
void D0omega_K4pi::SetFitNTuple(KKFitResult* fitresults, NTupleContainer& tuple)
{
  /// @remark This function is called in `TrackSelector::WriteFitResults`.
  /// -# Convert to the derived object of `KKFitResult` designed for this package. @remark This cast is required and cannot be solved using virtual methods, because of the specific structure of each analysis.
  KKFitResult_D0omega_K4pi* fit = dynamic_cast<KKFitResult_D0omega_K4pi*>(fitresults);

  /// -# @warning Terminate if cast failed.
  if(!fit)
  {
    std::cout << "FATAL ERROR: Dynamic cast failed" << std::endl;
    throw StatusCode::FAILURE;
  }

  /// -# Set the `NTuple::Item`s.
  tuple.GetItem<double>("chisq")  = fit->fChiSquared;
  tuple.GetItem<double>("mD0")    = fit->fM_D0;
  tuple.GetItem<double>("mJpsi")  = fit->fM_Jpsi;
  tuple.GetItem<double>("momega") = fit->fM_omega;
  tuple.GetItem<double>("mpi0")   = fit->fM_pi0;
  tuple.GetItem<double>("pD0")    = fit->fP_D0;
  tuple.GetItem<double>("pomega") = fit->fP_omega;

  tuple.GetItem<double>("ppi0")  = fit->fP_pi0;
  tuple.GetItem<double>("pK-")   = fit->fP_Km;
  tuple.GetItem<double>("ppi-")  = fit->fP_pim;
  tuple.GetItem<double>("ppi+1") = fit->fP_pip1;
  tuple.GetItem<double>("ppi+2") = fit->fP_pip2;

  tuple.GetItem<double>("fDalitzOmega_pi-pi+")  = fit->fDalitzOmega_pimpip;
  tuple.GetItem<double>("fDalitzOmega_pi0pi-")  = fit->fDalitzOmega_pi0pim;
  tuple.GetItem<double>("fDalitzOmega_pi0pi+")  = fit->fDalitzOmega_pi0pip;
  tuple.GetItem<double>("fRelativePhotonAngle") = fit->fRelativePhotonAngle;
}

/// Perform a Kalman **4-constraint** Kalman kinematic fit for all combinations and decide the combinations that results in the 'best' result.
/// The 'best' result is defined as the combination that has the smallest value of: \f$m_{K^-K^+}-m_{\omega}\f$ (that is the combination for which the invariant mass of the \f$K^-\pi^+\f$ is closest to \f$\omega\f$). See `D0omega_K4pi::MeasureForBestFit` for the definition of this measure.
/// @todo Decide whether 4-constraints is indeed suitable. *For more information, see [the page on primary and secondary vertex fits on the Offline Software Pages](https://docbes3.ihep.ac.cn/~offlinesoftware/index.php/Vertex_Fit) (requires login).*
void D0omega_K4pi::FindBestKinematicFit()
{
  if(!fNTuple_fit4c_all.DoWrite() && !fNTuple_fit4c_best.DoWrite()) return;
  ResetBestKalmanFit();
  DoKinematicFitForAllCombinations();
  if(!fBestKalmanFit.HasResults()) return;
  ++fCutFlow_NFitOK;
  WriteFitResults(&fBestKalmanFit, fNTuple_fit4c_best);
  WriteBestFitWithMcTruth();
}

void D0omega_K4pi::ResetBestKalmanFit()
{
  fBestKalmanFit = KKFitResult_D0omega_K4pi();
  fBestKalmanFit.ResetBestCompareValue();
}

void D0omega_K4pi::DoKinematicFitForAllCombinations()
{
  int count     = 0;
  fKaonNegIter  = fKaonNeg.begin();
  fPionNegIter  = fPionNeg.begin();
  fPionPos1Iter = fPionPos.begin();
  fPionPos2Iter = fPionPos1Iter + 1;
  for(fPhoton1Iter = fPhotons.begin(); fPhoton1Iter != fPhotons.end(); ++fPhoton1Iter)
  {
    for(fPhoton2Iter = fPhoton1Iter + 1; fPhoton2Iter != fPhotons.end(); ++fPhoton2Iter)
    {
      ++count;
      std::cout << "  fitting combination " << count << "..." << std::endl;
      BuildVertexParameter();
      DoVertexFit();
      DoKinematicFit();
      WriteFitResults(&fCurrentKalmanFit, fNTuple_fit4c_all);
      if(fCurrentKalmanFit.IsBetter()) fBestKalmanFit = fCurrentKalmanFit;
    }
  }
}

void D0omega_K4pi::BuildVertexParameter()
{
  HepPoint3D   vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  double       bx  = 1E+6;
  double       by  = 1E+6;
  double       bz  = 1E+6;
  Evx[0][0]        = bx * bx;
  Evx[1][1]        = by * by;
  Evx[2][2]        = bz * bz;
  fVertexParameter = VertexParameter();
  fVertexParameter.setVx(vx);
  fVertexParameter.setEvx(Evx);
}

void D0omega_K4pi::DoVertexFit()
{
  InitializeVertexFit();
  AddTracksToVertexFit();
  AddVertexToVertexFit();
  if(!fVertexFit->Fit(0))
  {
    fLog << MSG::WARNING << "vertex fit failed" << endmsg;
    return;
  }
  fVertexFit->Swim(0);
}

void D0omega_K4pi::InitializeVertexFit()
{
  fVertexFit = VertexFit::instance();
  fVertexFit->init();
}

void D0omega_K4pi::AddTracksToVertexFit()
{
  fVertexFit->AddTrack(0, BuildWTrackParameter(*fKaonNegIter, gM_K));
  fVertexFit->AddTrack(1, BuildWTrackParameter(*fPionNegIter, gM_pi));
  fVertexFit->AddTrack(2, BuildWTrackParameter(*fPionPos1Iter, gM_pi));
  fVertexFit->AddTrack(3, BuildWTrackParameter(*fPionPos2Iter, gM_pi));
}

void D0omega_K4pi::AddVertexToVertexFit()
{
  fVertexFit->AddVertex(0, fVertexParameter, 0, 1);
}

void D0omega_K4pi::DoKinematicFit()
{
  if(fVertexFit) return;
  InitializeKinematicFit();
  AddTracksToKinematicFit();
  AddConstraintsToKinematicFit();
  if(!fKalmanKinematicFit->Fit())
  {
    std::cout << "  fit failed: chisq = " << fKalmanKinematicFit->chisq() << std::endl;
    return;
  }
  ExtractFitResults();
}

void D0omega_K4pi::InitializeKinematicFit()
{
  fKalmanKinematicFit = KalmanKinematicFit::instance();
  fKalmanKinematicFit->init();
}

void D0omega_K4pi::AddTracksToKinematicFit()
{
  fKalmanKinematicFit->AddTrack(0, 0., (*fPhoton1Iter)->emcShower()); // gamma (1st occurrence)
  fKalmanKinematicFit->AddTrack(1, 0., (*fPhoton2Iter)->emcShower()); // gamma (2nd occurence)
  fKalmanKinematicFit->AddTrack(2, fVertexFit->wtrk(0));              // K-
  fKalmanKinematicFit->AddTrack(3, fVertexFit->wtrk(1));              // pi-
  fKalmanKinematicFit->AddTrack(4, fVertexFit->wtrk(2));              // pi+ (1st occurrence)
  fKalmanKinematicFit->AddTrack(5, fVertexFit->wtrk(3));              // pi+ (2nd occurrence)
}

void D0omega_K4pi::AddConstraintsToKinematicFit()
{
  fKalmanKinematicFit->AddFourMomentum(0, gEcmsVec);  /// 4C: CMS energy and 3-momentum
  fKalmanKinematicFit->AddResonance(1, gM_pi0, 0, 1); /// 1C: \f$\pi^0\f$ resonance
}

void D0omega_K4pi::ExtractFitResults()
{
  fCurrentKalmanFit = KKFitResult_D0omega_K4pi(fKalmanKinematicFit);
  fCurrentKalmanFit.SetRunAndEventNumber(fEventHeader);
}

void D0omega_K4pi::WriteBestFitWithMcTruth()
{
  std::cout << "  Result Kalman fit for (run, event) = ";
  std::cout << fEventHeader->runNumber() << ", ";
  std::cout << fEventHeader->eventNumber() << "):" << std::endl;
  std::cout << "    chi2     = " << fBestKalmanFit.fChiSquared << std::endl
            << "    m_pi0    = " << fBestKalmanFit.fM_pi0 << std::endl
            << "    m_D0     = " << fBestKalmanFit.fM_D0 << std::endl
            << "    m_omega  = " << fBestKalmanFit.fM_omega << std::endl
            << "    p_D0     = " << fBestKalmanFit.fP_D0 << std::endl
            << "    p_omega  = " << fBestKalmanFit.fP_omega << std::endl;
  CreateMCTruthCollection();
  SetAdditionalNtupleItems_topology();
  WriteMCTruthForTopoAna(fNTuple_topology);
}

void D0omega_K4pi::SetAdditionalNtupleItems_topology()
{
  fNTuple_topology.GetItem<double>("chi2")   = fBestKalmanFit.fChiSquared;
  fNTuple_topology.GetItem<double>("mpi0")   = fBestKalmanFit.fM_pi0;
  fNTuple_topology.GetItem<double>("mD0")    = fBestKalmanFit.fM_D0;
  fNTuple_topology.GetItem<double>("momega") = fBestKalmanFit.fM_omega;
  fNTuple_topology.GetItem<double>("pD0")    = fBestKalmanFit.fP_D0;
  fNTuple_topology.GetItem<double>("pomega") = fBestKalmanFit.fP_omega;

  fNTuple_topology.GetItem<double>("ppi0")  = fBestKalmanFit.fP_pi0;
  fNTuple_topology.GetItem<double>("pK-")   = fBestKalmanFit.fP_Km;
  fNTuple_topology.GetItem<double>("ppi-")  = fBestKalmanFit.fP_pim;
  fNTuple_topology.GetItem<double>("ppi+1") = fBestKalmanFit.fP_pip1;
  fNTuple_topology.GetItem<double>("ppi+2") = fBestKalmanFit.fP_pip2;

  fNTuple_topology.GetItem<double>("fDalitzOmega_pi-pi+")  = fBestKalmanFit.fDalitzOmega_pimpip;
  fNTuple_topology.GetItem<double>("fDalitzOmega_pi0pi-")  = fBestKalmanFit.fDalitzOmega_pi0pim;
  fNTuple_topology.GetItem<double>("fDalitzOmega_pi0pi+")  = fBestKalmanFit.fDalitzOmega_pi0pip;
  fNTuple_topology.GetItem<double>("fRelativePhotonAngle") = fBestKalmanFit.fRelativePhotonAngle;
}

void D0omega_K4pi::CreateMCTruthSelection()
{
  if(fEventHeader->runNumber() >= 0) return; // negative run number means MC data
  if(!fNTuple_fit_mc.DoWrite()) return;
  fMcKaonNeg.clear();
  fMcPionNeg.clear();
  fMcPionPos.clear();
  std::vector<Event::McParticle*>::iterator it;
  for(it = fMcParticles.begin(); it != fMcParticles.end(); ++it) PutParticleInCorrectVector(*it);
}

void D0omega_K4pi::PutParticleInCorrectVector(Event::McParticle* mcParticle)
{
  switch(mcParticle->particleProperty())
  {
    case -321: fMcKaonNeg.push_back(mcParticle); break;
    case -211: fMcPionNeg.push_back(mcParticle); break;
    case 211: fMcPionPos.push_back(mcParticle); break;
    default:
      fLog << MSG::DEBUG << "No switch case defined for McParticle "
           << mcParticle->particleProperty() << endmsg;
  }
}

// * ============================= * //
// * ------- FINALIZE STEP ------- * //
// * ============================= * //

/// Currently does nothing.
/// See `TrackSelector::finalize` for what else is done when finalising.
StatusCode D0omega_K4pi::finalize_rest()
{
  PrintFunctionName("D0omega_K4pi", __func__);
  return StatusCode::SUCCESS;
}