// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "IniSelect/Algorithms/D0omega_K4pi.h"
#include "IniSelect/Exceptions/KinematicFitFailed.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Handlers/KinematicFitter.h"
#include "IniSelect/Handlers/ParticleIdentifier.h"
#include "IniSelect/Handlers/VertexFitter.h"
#include <string>
#include <utility>

using namespace IniSelect;
using namespace std;

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //

/// Constructor for the `D0omega_K4pi` algorithm, derived from `TrackSelector`.
/// Here, you should declare properties: give them a name, assign a parameter (data member of `D0omega_K4pi`), and if required a documentation string. Note that you should define the paramters themselves in the header (D0omega_K4pi/D0omega_K4pi.h) and that you should assign the values in `share/D0omega_K4pi.txt`.
D0omega_K4pi::D0omega_K4pi(const string& name, ISvcLocator* pSvcLocator) :
  TrackSelector(name, pSvcLocator),
  fNTuple_fit4c_all("fit4c_all", "4-constraint fit information (CMS 4-momentum)"),
  fNTuple_fit4c_best(
    "fit4c_best",
    "4-constraint fit information of the invariant masses closest to the reconstructed particles"),
  fNTuple_fit_mc("fit_mc", "Fake fit information according to MC truth")
{
  LOG_FUNCTION();
  PostConstructor();
  fCreateChargedCollection = true;
  fCreateNeutralCollection = true;
}

// * =============================== * //
// * ------- INITIALIZE STEP ------- * //
// * =============================== * //

void D0omega_K4pi::ConfigureCandidateSelection()
{
  LOG_FUNCTION();
  fFinalState.SetFinalState("K- pi+ pi- pi+ g g");
}

void D0omega_K4pi::AddAdditionalNTupleItems()
{
  LOG_FUNCTION();
  AddNTupleItems_dedx();
  AddNTupleItems_fit();
  AddAdditionalNTuples_topology();
}

void D0omega_K4pi::AddNTupleItems_fit()
{
  /// `"fit4c_*"`: Kinematic fit results. See `TrackSelector::AddNTupleItems_fit` for more info.
  AddNTupleItems_fit(fNTuple_fit4c_all);
  AddNTupleItems_fit(fNTuple_fit4c_best);
  AddNTupleItems_fit(fNTuple_fit_mc);
}

void D0omega_K4pi::AddNTupleItems_fit(NTupleContainer& tuple)
{
  if(!tuple.DoWrite()) return;
  /// This function encapsulates the `addItem` procedure for the fit branches.
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
  tuple.AddItem<double>("ppi0");
  tuple.AddItem<double>("pK-");
  tuple.AddItem<double>("ppi-");
  tuple.AddItem<double>("ppi+1");
  tuple.AddItem<double>("ppi+2");
  tuple.AddItem<double>("fDalitzOmega_pi-pi+");
  tuple.AddItem<double>("fDalitzOmega_pi0pi-");
  tuple.AddItem<double>("fDalitzOmega_pi0pi+");
  tuple.AddItem<double>("fRelativePhotonAngle");
}

void D0omega_K4pi::AddAdditionalNTuples_topology()
{
  /// `"topology"`: Add *additional* mass branches to the MC truth branch for the `topoana` package
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

// * ============================ * //
// * ------- EXECUTE STEP ------- * //
// * ============================ * //

void D0omega_K4pi::ConfigurePID()
{
  ParticleIdentifier::UseProbabilityMethod();
  ParticleIdentifier::UseDedx();
  ParticleIdentifier::UseTofIB();
  ParticleIdentifier::UseTofOB();
  ParticleIdentifier::UseTofE();
  ParticleIdentifier::SetMinimalChi2(4.);
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
    cout << "FATAL ERROR: Dynamic cast failed" << endl;
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
  LOG_FUNCTION();
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
  LOG_FUNCTION();
  int count = 0;
  do
  {
    ++count;
    cout << "  combination " << count << ": ";
    cout << fFinalState.GetCandidates().GetPhotons().GetCandidate(0) << ", ";
    cout << fFinalState.GetCandidates().GetPhotons().GetCandidate(1) << endl;
    fLog << MSG::INFO << "  combination " << count << ": " << endmsg;
    try
    {
      DoVertexFit();
      DoKinematicFit();
      ExtractFitResults();
    }
    catch(const KinematicFitFailed& e)
    {
      cout << "  " << e.GetMessage() << endl;
      continue;
    }
    WriteFitResults(&fCurrentKalmanFit, fNTuple_fit4c_all);
    if(fCurrentKalmanFit.IsBetter()) fBestKalmanFit = fCurrentKalmanFit;
  } while(fFinalState.GetCandidates().NextPhotonCombination());
}

void D0omega_K4pi::DoVertexFit()
{
  VertexFitter::Initialize();
  VertexFitter::AddTracks(fFinalState.GetCandidates());
  VertexFitter::AddCleanVertex();
  VertexFitter::FitAndSwim();
}

void D0omega_K4pi::DoKinematicFit()
{
  if(!VertexFitter::IsSuccessful()) return;
  KinematicFitter::Initialize();
  KinematicFitter::AddTracks(fFinalState.GetCandidates());
  KinematicFitter::AddConstraintCMS();
  KinematicFitter::AddResonance(Mass::pi0, 0, 1);
  KinematicFitter::Fit();
}

void D0omega_K4pi::ExtractFitResults()
{
  if(KinematicFitter::GetFit())
  {
    cout << "numberWTrack:     " << KinematicFitter::GetFit()->numberWTrack() << endl;
    cout << "numberGammaShape: " << KinematicFitter::GetFit()->numberGammaShape() << endl;
  }
  fCurrentKalmanFit = KKFitResult_D0omega_K4pi(KinematicFitter::GetFit());
  fCurrentKalmanFit.SetRunNumber(fInputFile.RunNumber());
  fCurrentKalmanFit.SetEventNumber(fInputFile.EventNumber());
}

void D0omega_K4pi::WriteBestFitWithMcTruth()
{
  cout << "  Result Kalman fit for (run, event) = ";
  cout << fInputFile.RunNumber() << ", ";
  cout << fInputFile.EventNumber() << "):" << endl;
  cout << "    chi2     = " << fBestKalmanFit.fChiSquared << endl
       << "    m_pi0    = " << fBestKalmanFit.fM_pi0 << endl
       << "    m_D0     = " << fBestKalmanFit.fM_D0 << endl
       << "    m_omega  = " << fBestKalmanFit.fM_omega << endl
       << "    p_D0     = " << fBestKalmanFit.fP_D0 << endl
       << "    p_pi0    = " << fBestKalmanFit.fP_pi0 << endl;
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
  if(!fInputFile.IsMonteCarlo()) return;
  if(!fNTuple_fit_mc.DoWrite()) return;
  fFinalState.GetCandidatesMC().ClearCharged();
  vector<Event::McParticle*>::iterator it;
  for(it = fMcParticles.begin(); it != fMcParticles.end(); ++it) PutParticleInCorrectVector(*it);
}