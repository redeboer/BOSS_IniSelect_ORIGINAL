#include "IniSelect/Algorithms/TrackSelector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "DstEvent/TofHitStatus.h"
#include "GaudiKernel/Bootstrap.h"
#include "IniSelect/Algorithms/TrackSelector.h"
#include "IniSelect/Exceptions/AlgorithmResult.h"
#include "IniSelect/Functions/String.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Handlers/ParticleIdentifier.h"
#include "IniSelect/TrackCollections/CandidateIter.h"
#include "IniSelect/TrackCollections/CandidateIterMC.h"
#include "TMath.h"
#include "TString.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"
#include <cmath>
#include <iostream>

using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using namespace IniSelect;
using namespace IniSelect::String;
using namespace std;

/// Constructor for the `TrackSelector` base algorithm.
/// Here, you should declare properties: give them a name, assign a parameter (data member of `TrackSelector`), and if required a documentation string. Note that you should define the paramters themselves in the header (TrackSelector/TrackSelector.h) and that you should assign the values in `share/jopOptions_TrackSelector.txt`. Algorithms should inherit from Gaudi's `Algorithm` class. See class `GaudiKernel`s [`Algorithm`][https://dayabay.bnl.gov/dox/GaudiKernel/html/classAlgorithm.html] for more details.
TrackSelector::TrackSelector(const string& name, ISvcLocator* pSvcLocator) :
  /// * Construct `Algorithm` objects.
  Algorithm(name, pSvcLocator),
  fLog(msgSvc(), name),
  /// * Construct objects for file access.
  fInputFile(eventSvc()),
  fNTupleBooker(ntupleSvc()),
  /// * Construct `NTuple::Tuple`s containers used in base class
  fNTuple_mult("mult", "Event multiplicities"),
  fNTuple_vertex("vertex", "Primary vertex (interaction point)"),
  fNTuple_charged("charged", "Charged track info"),
  fNTuple_neutral("neutral", "Charged track info"),
  fNTuple_dedx("dedx", "dE/dx of all charged tracks"),
  fNTuple_TofEC("TofEC", "End cap ToF of all tracks"),
  fNTuple_TofIB("TofIB", "Inner barrel ToF of all tracks"),
  fNTuple_TofOB("TofOB", "Outer barrel ToF of all tracks"),
  fNTuple_PID("PID", "Particle Identification parameters"),
  fNTuple_photon("photon", "Kinematics of selected photons"),
  fNTuple_cuts("_cutvalues"),
  fNTuple_topology("topology", "Monte Carlo truth for TopoAna package"),
  /// * Construct counters (in essence a `CutObject` without cuts).
  fCutFlow_NetChargeOK("N_netcharge_OK",
                       "Number of events where the total charge detected in the detectors was 0"),
  fCutFlow_NChargedOK("N_charged_OK", "Number of events that had exactly 4 charged tracks"),
  fCutFlow_NFitOK("N_Fit_OK", "Number of combinations where where the kinematic fit worked"),
  fCutFlow_NPIDnumberOK("N_PID_OK",
                        "Number of events that had exactly 2 K-, 1 K+ and 1 pi+ PID tracks"),
  fCut_GammaAngle("gamma_angle"),
  fCut_GammaPhi("gamma_phi"),
  fCut_GammaTheta("gamma_theta"),
  /// * Construct `CutObject`s. The `"cut_<parameter>_min/max"` properties determine cuts on certain parameters.
  fCut_Vxy("vertex_xy"),
  fCut_Vz("vertex_z"),
  fCut_Rxy("declen_xy"),
  fCut_Rz("declen_z"),
  fCut_CosTheta("costheta"),
  fCut_PhotonEnergy("PhotonEnergy"),
  fCut_PIDChiSq("PIDChiSq"),
  fCut_PIDProb("PIDProb"),
  /// * Set default values for create switches (`fCreateChargedCollection` and `fCreateNeutralCollection`)
  fCreateChargedCollection(false),
  fCreateNeutralCollection(false),
  fPostConstructed(false)
{
  LOG_FUNCTION();
}

/// Rather dubious construction, but this method is required and **has to be called at the end of each derived constructor**. The reason for that this method is necessary is that a Gaudi `Algorithm` requires properties to have been declared by the time the `Algorithm` has been constructed.
void TrackSelector::PostConstructor()
{
  DeclareSwitches();
  DeclareCuts();
  fPostConstructed = true;
}

/// Declare properties for each `JobSwitch`.
/// This method has been added to the `TrackSelector`, and not to the `JobSwitch` class, because it requires the `Algorithm::decalareProperty` method.
void TrackSelector::DeclareSwitches()
{
  fLog << MSG::INFO << "Assigning job switches to NTuple containers:" << endmsg;
  list<JobSwitch*>::iterator it = JobSwitch::gJobSwitches.begin();
  for(; it != JobSwitch::gJobSwitches.end(); ++it)
  {
    declareProperty((*it)->Name().c_str(), (*it)->fValue);
    fLog << MSG::INFO << "  added property \"" << (*it)->Name() << "\"" << endmsg;
  }
}

/// Declare corresponding job properties for each `CutObject`.
/// Each `CutObject` has two corresponding job properties: a minimum and a maximum. These two corresponding properties are declared as `"cut_<name>_min/max" respectively so that they can be set in the job options file.
/// @remark This method has been added to the `TrackSelector`, and not to the `CutObject` class, because it requires the `Algorithm::decalareProperty` method.
void TrackSelector::DeclareCuts()
{
  fLog << MSG::INFO << "Declaring cuts for NTuple \"" << fNTuple_cuts.Name() << "\":" << endmsg;
  list<CutObject*>::iterator cut = CutObject::gCutObjects.begin();
  for(; cut != CutObject::gCutObjects.end(); ++cut)
  {
    /// -# Declare a `"cut_<name>_min"` property.
    declareProperty((*cut)->NameMin(), (*cut)->min);
    /// -# Declare a `"cut_<name>_max"` property.
    declareProperty((*cut)->NameMax(), (*cut)->max);
    fLog << MSG::INFO << "  added \"cut_" << (*cut)->Name() << "_min/max\"" << endmsg;
  }
  /// -# Also set `JobSwitches` of `"_cutvalues"` to `true`, because they have to be written no matter what the job option files says.
  fNTuple_cuts.SetWriteSwitch(true);
}

/// (Inherited) `initialize` step of `Algorithm`.
/// This function is called once in the beginning *of each run*. Define and load NTuples here. The `NTuples` will become the `TTree`s in the eventual ROOT file, the added `NTuple::Item`s will be the branches of those trees.
StatusCode TrackSelector::initialize()
{
  LOG_FUNCTION();
  try
  {
    AssertPostConstructed();
    OverwriteCreateBits();
    fNTupleBooker.BookAll();
    AddNTupleItems();
    AddAdditionalNTupleItems();
    ConfigureCandidateSelection();
  }
  catch(const AlgorithmFailure& e)
  {
    e.Print();
    return StatusCode::FAILURE;
  }
  catch(const AlgorithmSuccess& e)
  {
    return StatusCode::SUCCESS;
  }
}

/// Method that has to be called before anything can be done in the `initialize` step.
void TrackSelector::AssertPostConstructed() const
{
  if(fPostConstructed) return;
  std::cout << "FATAL ERROR: PostConstructor has not been called in constructor of derived class "
               "of TrackSelector"
            << std::endl;
  throw AlgorithmSuccess();
}

void TrackSelector::OverwriteCreateBits()
{
  fCreateNeutralCollection |= fNTuple_neutral.DoWrite();
  fCreateChargedCollection |= fNTuple_charged.DoWrite();
}

void TrackSelector::AddNTupleItems()
{
  AddNTupleItems_mult();
  AddNTupleItems_vertex();
  AddNTupleItems_charged();
  AddNTupleItems_neutral();
  AddNTupleItems_Tof();
  AddNTupleItems_dedx(fNTuple_dedx);
  AddNTupleItems_MCTruth(fNTuple_topology);
  AddNTupleItems_PID();
}

void TrackSelector::AddNTupleItems_mult()
{
  /// `"mult"`: Multiplicities of the total event
  /// * `"Ntotal"`:   Total number of events per track.
  /// * `"Ncharge"`:  Number of charged tracks.
  /// * `"Nneutral"`: Number of charged tracks.
  /// * `"NgoodNeutral"`: Number of 'good' neutral tracks (if performed).
  /// * `"NgoodCharged"`: Number of 'good' charged tracks (if performed).
  /// * `"N_<particle>"`: Multiplicity for the particles you selected.
  fNTuple_mult.AddItem<int>("Ntotal");
  fNTuple_mult.AddItem<int>("Ncharge");
  fNTuple_mult.AddItem<int>("Nneutral");
  if(fCreateNeutralCollection) fNTuple_mult.AddItem<int>("NgoodNeutral");
  if(fCreateChargedCollection) fNTuple_mult.AddItem<int>("NgoodCharged");
  ChargedCandidateIter it(fFinalState.GetCandidates());
  while(CandidateTracks<EvtRecTrack>* coll = it.Next())
    fNTuple_mult.AddItem<int>(Form("N_%s", coll->GetPdtName()));
}

void TrackSelector::AddNTupleItems_vertex()
{
  /// `"vertex"`: Primary vertex info (interaction point)
  /// * `"vx0"`: \f$x\f$ coordinate of the interaction point.
  /// * `"vy0"`: \f$y\f$ coordinate of the interaction point.
  /// * `"vz0"`: \f$z\f$ coordinate of the interaction point.
  fNTuple_vertex.AddItem<double>("vx0");
  fNTuple_vertex.AddItem<double>("vy0");
  fNTuple_vertex.AddItem<double>("vz0");
}

void TrackSelector::AddNTupleItems_charged()
{
  /// `"charged"`: Charged track info: secondary vertact and distance to IP.
  /// * `"vx"`:    \f$x\f$ coordinate of the secondary vertex as determined by MDC.
  /// * `"vy"`:    \f$y\f$ coordinate of the secondary vertex as determined by MDC.
  /// * `"vz"`:    \f$z\f$ coordinate of the secondary vertex as determined by MDC.
  /// * `"vr"`:    Distance from origin in \f$xy\f$ plane.
  /// * `"rvxy"`:  Nearest distance to interaction point in \f$xy\f$ plane.
  /// * `"rvz"`:   Nearest distance to interaction point in \f$z\f$ direction. @todo Get explanation of geometry (angle) definitions in an event.
  /// * `"rvphi"`: Angle in the \f$xy\f$plane.
  /// * `"phi"`:   Helix angle of the particle.
  /// * `"p"`:     Momentum \f$p\f$ of the track.
  fNTuple_charged.AddItem<double>("vx");
  fNTuple_charged.AddItem<double>("vy");
  fNTuple_charged.AddItem<double>("vz");
  fNTuple_charged.AddItem<double>("vr");
  fNTuple_charged.AddItem<double>("rvxy");
  fNTuple_charged.AddItem<double>("rvz");
  fNTuple_charged.AddItem<double>("rvphi");
  fNTuple_charged.AddItem<double>("phi");
  fNTuple_charged.AddItem<double>("p");
}

void TrackSelector::AddNTupleItems_neutral()
{
  /// `"neutral"`: Neutral track info.
  /// * `"E"`: Energy of the neutral track as determined by the EM calorimeter.
  /// * `"x"`: \f$x\f$-coordinate of the neutral track according to the EMC.
  /// * `"y"`: \f$y\f$-coordinate of the neutral track according to the EMC.
  /// * `"z"`: \f$z\f$-coordinate of the neutral track according to the EMC.
  /// * `"phi"`: \f$\phi\f$-angle of the neutral track according to the EMC.
  /// * `"theta"`: \f$\theta\f$-angle of the neutral track according to the EMC.
  /// * `"time"`: Time of the neutral track according to the EMC. @todo Investigate what this parameter precisely means.
  fNTuple_neutral.AddItem<double>("E");
  fNTuple_neutral.AddItem<double>("x");
  fNTuple_neutral.AddItem<double>("y");
  fNTuple_neutral.AddItem<double>("z");
  fNTuple_neutral.AddItem<double>("phi");
  fNTuple_neutral.AddItem<double>("theta");
  fNTuple_neutral.AddItem<double>("time");
}

void TrackSelector::AddNTupleItems_Tof()
{
  AddNTupleItems_Tof(fNTuple_TofEC);
  AddNTupleItems_Tof(fNTuple_TofIB);
  AddNTupleItems_Tof(fNTuple_TofOB);
}

void TrackSelector::AddNTupleItems_Tof(NTupleContainer& tuple)
{
  /// * `"p"`:      Momentum of the track as reconstructed by MDC.
  /// * `"tof"`:    Time of flight.
  /// * `"path"`:   Path length.
  /// * `"cntr"`:   ToF counter ID.
  /// * `"ph"`:     ToF pulse height.
  /// * `"zrhit"`:  Track extrapolate \f$Z\f$ or \f$R\f$ Hit position.
  /// * `"qual"`:   Data quality of reconstruction.
  /// * `"tof_e"`:  Difference with ToF in electron hypothesis.
  /// * `"tof_mu"`: Difference with ToF in muon hypothesis.
  /// * `"tof_pi"`: Difference with ToF in charged pion hypothesis.
  /// * `"tof_K"`:  Difference with ToF in charged kaon hypothesis.
  /// * `"tof_p"`:  Difference with ToF in proton hypothesis.
  if(!tuple.DoWrite()) return;
  tuple.AddItem<double>("p");
  tuple.AddItem<double>("tof");
  tuple.AddItem<double>("path");
  tuple.AddItem<double>("cntr");
  tuple.AddItem<double>("ph");
  tuple.AddItem<double>("zrhit");
  tuple.AddItem<double>("qual");
  tuple.AddItem<double>("tof_e");
  tuple.AddItem<double>("tof_mu");
  tuple.AddItem<double>("tof_pi");
  tuple.AddItem<double>("tof_K");
  tuple.AddItem<double>("tof_p");
}

void TrackSelector::AddNTupleItems_dedx(NTupleContainer& tuple)
{
  /// This function encapsulates the `addItem` procedure for the \f$dE/dx\f$ energy loss branch (`"dedx"`). This method allows you to perform the same booking method for different types of charged particles (for instance 'all charged particles', kaons, and pions).
  /// * `"Ngoodhits"`:  Number of good \f$dE/dx\f$ hits (excluding overflow).
  /// * `"Ntotalhits"`: Number of good \f$dE/dx\f$ hits (including overflow).
  /// * `"chie"`:       \f$\chi^2\f$ in case of electron ("number of \f$\sigma\f$ from \f$e^\pm\f$").
  /// * `"chik"`:       \f$\chi^2\f$ in case of kaon ("number of \f$\sigma\f$ from \f$K^\pm\f$").
  /// * `"chimu"`:      \f$\chi^2\f$ in case of muon ("number of \f$\sigma\f$ from \f$\mu^\pm\f$").
  /// * `"chip"`:       \f$\chi^2\f$ in case of proton ("number of \f$\sigma\f$ from \f$p^\pm\f$").
  /// * `"chipi"`:      \f$\chi^2\f$ in case of pion ("number of \f$\sigma\f$ from \f$\pi^\pm\f$").
  /// * `"normPH"`:     Normalized pulse height.
  /// * `"p"`:          Momentum of the track as reconstructed by MDC.
  /// * `"probPH"`:     Most probable pulse height from truncated mean.
  if(!tuple.DoWrite()) return;
  tuple.AddItem<int>("Ngoodhits");
  tuple.AddItem<int>("Ntotalhits");
  tuple.AddItem<double>("chie");
  tuple.AddItem<double>("chik");
  tuple.AddItem<double>("chimu");
  tuple.AddItem<double>("chip");
  tuple.AddItem<double>("chipi");
  tuple.AddItem<double>("normPH");
  tuple.AddItem<double>("p");
  tuple.AddItem<double>("probPH");
}

void TrackSelector::AddNTupleItems_MCTruth(NTupleContainer& tuple)
{
  /// This function encapsulates the `addItem` procedure for the MC truth branches for the TopoAna package. Have a look at [this page](https://besiii.gitbook.io/boss/packages/analysis/topoana#preparing-initial-event-selection) as for why these fields are required.
  /// * `"runID"`: Run number ID.
  /// * `"evtID"`: Rvent number ID.
  /// * `"index"`: Index that is necessary for loading the following his one is necessary for loading following two items, because they are arrays.
  /// * `"particle"`: PDG code for the particle in this array.
  /// * `"mother"`: Track index of the mother particle.
  if(!tuple.DoWrite()) return;
  tuple.AddItem<int>("runID");
  tuple.AddItem<int>("evtID");
  NTuple::Item<int>& index = *tuple.AddItem<int>("index", 0, 100);
  tuple.AddIndexedItem<int>("particle", index);
  tuple.AddIndexedItem<int>("mother", index);
}

void TrackSelector::AddNTupleItems_PID()
{
  /// `"PID"`: Track PID information.
  /// * `"p"`:        Momentum of the track as reconstructed by MDC.
  /// * `"cost"`:     Theta angle of the track.
  /// * `"chiToFIB"`: \f$\chi^2\f$ of the inner barrel ToF of the track.
  /// * `"chiToFEC"`: \f$\chi^2\f$ of the end cap ToF of the track.
  /// * `"chiToFOB"`: \f$\chi^2\f$ of the outer barrel ToF of the track.
  /// * `"chidEdx"`:  \f$\chi^2\f$ of the energy loss \f$dE/dx\f$ of the identified track.
  /// * `"prob_K"`:   Probability that the track is from a kaon according to the probability method.
  /// * `"prob_e"`:   Probability that the track is from a electron according to the probability method.
  /// * `"prob_mu"`:  Probability that the track is from a muon according to the probability method.
  /// * `"prob_p"`:   Probability that the track is from a proton according to the probability method.
  /// * `"prob_pi"`:  Probability that the track is from a pion according to the probability method.
  fNTuple_PID.AddItem<double>("p");
  fNTuple_PID.AddItem<double>("cost");
  fNTuple_PID.AddItem<double>("chiToFIB");
  fNTuple_PID.AddItem<double>("chiToFEC");
  fNTuple_PID.AddItem<double>("chiToFOB");
  fNTuple_PID.AddItem<double>("chidEdx");
  fNTuple_PID.AddItem<double>("prob_K");
  fNTuple_PID.AddItem<double>("prob_e");
  fNTuple_PID.AddItem<double>("prob_mu");
  fNTuple_PID.AddItem<double>("prob_p");
  fNTuple_PID.AddItem<double>("prob_pi");
}

void TrackSelector::AddNTupleItems_photon()
{
  /// `"photon"`: Information of the selected photons
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
  catch(const AlgorithmFailure& e)
  {
    e.Print();
    return StatusCode::FAILURE;
  }
  catch(const AlgorithmSuccess& e)
  {
    return StatusCode::SUCCESS;
  }
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
    texp[j]     = 10 * path / beta / Physics::SpeedOfLight; // hypothesis ToF
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
  CandidateIter it(fFinalState.GetCandidates());
  while(CandidateTracks<EvtRecTrack>* coll = it.Next())
    fNTuple_mult.GetItem<int>(Form("N_%s", coll->GetPdtName())) = coll->GetNTracks();
  fNTuple_mult.Write();
}

void TrackSelector::PrintMultiplicities()
{
  LOG_FUNCTION();
  // fLog << MSG::INFO;
  CandidateIter it(fFinalState.GetCandidates());
  while(CandidateTracks<EvtRecTrack>* coll = it.Next())
    cout << "N_" << coll->GetPdtName() << " = " << coll->GetNTracks() << "  ";
  cout << endl;
}

/// **PID cut**: apply a strict cut on the number of the selected particles.
void TrackSelector::CutPID()
{
  LOG_FUNCTION();
  ChargedCandidateIter it(fFinalState.GetCandidates());
  while(CandidateTracks<EvtRecTrack>* coll = it.Next())
    if(coll->FailsMultiplicityCut()) throw AlgorithmSuccess();
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
       << fFinalState.GetCandidates().GetNCharged() << endmsg;
  if(fChargedTracks.size() != fFinalState.GetCandidates().GetNCharged()) throw AlgorithmSuccess();
  ++fCutFlow_NChargedOK;
}

void TrackSelector::SelectChargedCandidates()
{
  LOG_FUNCTION();
  ParticleIdentifier::Initialize();
  ConfigurePID();

  fLog << MSG::DEBUG << "Will identify particles: ";
  ChargedCandidateIter it(fFinalState.GetCandidates());
  while(CandidateTracks<EvtRecTrack>* coll = it.Next())
  {
    fLog << MSG::DEBUG << coll->GetPdtName() << "  ";
    ParticleIdentifier::SetParticleToIdentify(coll->GetPdgCode());
  }
  fLog << MSG::DEBUG << endmsg;

  fFinalState.GetCandidates().ClearCharged();
  fLog << MSG::DEBUG << "Identified: ";
  for(fTrackIter = fChargedTracks.begin(); fTrackIter != fChargedTracks.end(); ++fTrackIter)
  {
    string particleName = ParticleIdentifier::FindMostProbable(*fTrackIter, fCut_PIDProb);
    if(particleName.compare("") == 0) continue;
    fLog << MSG::DEBUG << particleName << "  ";
    if(!fFinalState.GetCandidates().HasCandidate(particleName)) continue;
    WritePIDInformation();
    fFinalState.GetCandidates().AddCandidate(*fTrackIter, particleName);
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
  fFinalState.GetCandidates().ClearNeutral();
  for(fTrackIter = fNeutralTracks.begin(); fTrackIter != fNeutralTracks.end(); ++fTrackIter)
  {
    AngleDifferences smallestAngles = FindSmallestPhotonAngles();
    smallestAngles.ConvertToDegrees();
    WritePhotonKinematics(smallestAngles);
    if(CutPhotonAngles(smallestAngles)) continue;
    fFinalState.GetCandidates().GetPhotons().AddTrack(*fTrackIter);
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

/// Is called at the end *of the entire process*.
/// Writes total cut flow to terminal and to the output file.
StatusCode TrackSelector::finalize()
{
  try
  {
    LOG_FUNCTION();
    NTupleContainer::PrintFilledTuples();
    AddAndWriteCuts();
    CutObject::PrintAll();
  }
  catch(const AlgorithmFailure& e)
  {
    e.Print();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// Write all cuts to a branch called "_cutvalues".
/// There will only be one entry per output file: a `C`-style array of size 3. Entry 0 of that array is the `min` value, entry 1 is the `max` value, and entry 2 is the recorded. As opposed to earlier versions of the `TrackSelector` (where the `"_cutvalues"` `TTree` contained 3 entries), we now use a `C`-style array (written through `NTuple::addIndexedItem`), because it allows to add up the number of counts when you have split up your jobs, which makes your output contain more than ROOT file. If you load these files as a `TChain`, you can get the total count of the `CutObject` by adding up all '2nd entries' (the counts) of each branch.
/// @see BOSSOutputLoader::PrintCuts
void TrackSelector::AddAndWriteCuts()
{
  list<CutObject*>::iterator cut   = CutObject::gCutObjects.begin();
  NTuple::Item<int>&         index = *fNTuple_cuts.AddItem<int>("index", 0, 3);
  for(; cut != CutObject::gCutObjects.end(); ++cut)
  {
    NTuple::Array<double>& array = *fNTuple_cuts.AddIndexedItem<double>((*cut)->Name(), index);

    index        = 0;
    array[index] = (*cut)->min;
    index        = 1;
    array[index] = (*cut)->max;
    index        = 2;
    array[index] = (int)((*cut)->counter);
  }
  ++index; // to make the array size 3
  fNTuple_cuts.Write();
}

void TrackSelector::PutParticleInCorrectVector(Event::McParticle* mcParticle)
{
  int pdgCode = mcParticle->particleProperty();

  ChargedCandidateIterMC it(fFinalState.GetCandidatesMC());
  while(CandidateTracks<Event::McParticle>* coll = it.Next())
  {
    if(coll->GetPdgCode() == pdgCode)
    {
      coll->AddTrack(mcParticle);
      return;
    }
  }
  fLog << MSG::DEBUG << "PDG code " << pdgCode << " does not exist in CandidateSelectionMC"
       << endmsg;
}

/// Is called at the end *of the entire process*.
/// Writes total cut flow to terminal and to the output file.
StatusCode TrackSelector::finalize()
{
  try
  {
    LOG_FUNCTION();
    NTupleContainer::PrintFilledTuples();
    AddAndWriteCuts();
    CutObject::PrintAll();
  }
  catch(const AlgorithmFailure& e)
  {
    e.Print();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// Write all cuts to a branch called "_cutvalues".
/// There will only be one entry per output file: a `C`-style array of size 3. Entry 0 of that array is the `min` value, entry 1 is the `max` value, and entry 2 is the recorded. As opposed to earlier versions of the `TrackSelector` (where the `"_cutvalues"` `TTree` contained 3 entries), we now use a `C`-style array (written through `NTuple::addIndexedItem`), because it allows to add up the number of counts when you have split up your jobs, which makes your output contain more than ROOT file. If you load these files as a `TChain`, you can get the total count of the `CutObject` by adding up all '2nd entries' (the counts) of each branch.
/// @see BOSSOutputLoader::PrintCuts
void TrackSelector::AddAndWriteCuts()
{
  list<CutObject*>::iterator cut   = CutObject::gCutObjects.begin();
  NTuple::Item<int>&         index = *fNTuple_cuts.AddItem<int>("index", 0, 3);
  for(; cut != CutObject::gCutObjects.end(); ++cut)
  {
    NTuple::Array<double>& array = *fNTuple_cuts.AddIndexedItem<double>((*cut)->Name(), index);

    index        = 0;
    array[index] = (*cut)->min;
    index        = 1;
    array[index] = (*cut)->max;
    index        = 2;
    array[index] = (int)((*cut)->counter);
  }
  ++index; // to make the array size 3
  fNTuple_cuts.Write();
}

void TrackSelector::PutParticleInCorrectVector(Event::McParticle* mcParticle)
{
  int pdgCode = mcParticle->particleProperty();

  ChargedCandidateIterMC it(fFinalState.GetCandidatesMC());
  while(CandidateTracks<Event::McParticle>* coll = it.Next())
  {
    if(coll->GetPdgCode() == pdgCode)
    {
      coll->AddTrack(mcParticle);
      return;
    }
  }
  fLog << MSG::DEBUG << "PDG code " << pdgCode << " does not exist in CandidateSelectionMC"
       << endmsg;
}