// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "TrackSelector/TrackSelector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/Event.h"
#include "EventModel/EventModel.h"
#include "GaudiKernel/Bootstrap.h"
#include "TMath.h"
#include "TString.h"
#include "TrackSelector/Containers/NTupleTopoAna.h"
#include "TrackSelector/TSGlobals.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"
#include <cmath>

using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using namespace TSGlobals;

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //

/// Constructor for the `TrackSelector` base algorithm.
/// Here, you should declare properties: give them a name, assign a parameter (data member of `TrackSelector`), and if required a documentation string. Note that you should define the paramters themselves in the header (TrackSelector/TrackSelector.h) and that you should assign the values in `share/jopOptions_TrackSelector.txt`. Algorithms should inherit from Gaudi's `Algorithm` class. See class `GaudiKernel`s [`Algorithm`][https://dayabay.bnl.gov/dox/GaudiKernel/html/classAlgorithm.html] for more details.
TrackSelector::TrackSelector(const std::string& name, ISvcLocator* pSvcLocator) :
  /// * Construct `Algorithm` objects.
  Algorithm(name, pSvcLocator),
  fLog(msgSvc(), name),
  /// * Construct objects for file access.
  fEventHeader(eventSvc(), "/Event/EventHeader"),
  fMcParticleCol(eventSvc(), "/Event/MC/McParticleCol"),
  fEvtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent),
  fEvtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol),
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
  fNTuple_cuts("_cutvalues",
               "1st entry: min value, 2nd entry: max value, 3rd entry: number passed"),
  /// * Construct `NTuple::Tuple`s containers used in derived classes.
  fNTuple_mult_sel("mult_select", "Multiplicities of selected particles"),
  fNTuple_topology("topology", "Monte Carlo truth for TopoAna package"),
  /// * Construct counters (in essence a `CutObject` without cuts).
  fCutFlow_Nevents("N_events"),
  fCutFlow_NetChargeOK("N_netcharge_OK",
                       "Number of events where the total charge detected in the detectors was 0"),
  fCounter_Ntracks("N_tracks"),
  fCounter_Ncharged("N_charged"),
  fCounter_Nneutral("N_neutral"),
  fCounter_Nmdcvalid("N_MDCvalid"),
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
  PrintFunctionName("TrackSelector", __func__);
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
  std::list<JobSwitch*>::iterator it = JobSwitch::gJobSwitches.begin();
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
  std::list<CutObject*>::iterator cut = CutObject::gCutObjects.begin();
  for(; cut != CutObject::gCutObjects.end(); ++cut)
  {
    /// -# Declare a `"cut_<name>_min"` property.
    declareProperty((*cut)->NameMin(), (*cut)->min);
    /// -# Declare a `"cut_<name>_max"` property.
    declareProperty((*cut)->NameMax(), (*cut)->max);
    fLog << MSG::INFO << "  added \"cut_" << (*cut)->Name() << "_min/max\"" << endmsg;
  }
  /// -# Also set `JobSwitches` of `"_cutvalues"` to `true`, because they have to be written no
  /// matter what the job option files says.
  fNTuple_cuts.SetWriteSwitch(true);
}

// * =============================== * //
// * ------- INITIALIZE STEP ------- * //
// * =============================== * //

/// (Inherited) `initialize` step of `Algorithm`.
/// This function is called once in the beginning *of each run*. Define and load NTuples here. The `NTuples` will become the `TTree`s in the eventual ROOT file, the added `NTuple::Item`s will be the branches of those trees.
StatusCode TrackSelector::initialize()
{
  PrintFunctionName("TrackSelector", __func__);
  try
  {
    AssertPostConstructed();
    OverwriteCreateBits();
    BookNTuples();
    AddNTupleItems_mult();
    AddNTupleItems_vertex();
    AddNTupleItems_charged();
    AddNTupleItems_neutral();
    AddNTupleItems_Tof();
    AddNTupleItems_dedx(fNTuple_dedx);
    AddNTupleItems_MCTruth(fNTuple_topology);
    AddNTupleItems_PID();
    initialize_rest();
  }
  catch(...)
  {
  } /// @todo Try to catch the `StatusCode`. Note that this is problematic, because you actually want to catch the `enum` within the class `StatusCode`...
  return StatusCode::SUCCESS;
}

/// Print function for debugging purposes.
/// This function has been implemented in the base class to standardise terminal output.
/// @remark In the derived classes, place this function at the beginning of each algorithm step for debugging purposes, using the format `PrintFunctionName("<class name>", __func__)`.
void TrackSelector::PrintFunctionName(const char* class_name, const char* function_name)
{
  fLog << MSG::DEBUG << "\n\n\n\n===>> " << class_name << "::" << function_name << " <<===\n"
       << endmsg;
}

/// Method that has to be called before anything can be done in the `initialize` step.
void TrackSelector::AssertPostConstructed() const
{
  if(fPostConstructed) return;
  std::cout << "FATAL ERROR: PostConstructor has not been called in constructor of derived class "
               "of TrackSelector"
            << std::endl;
  throw StatusCode::FAILURE;
}

/// Go over all instances of `NTupleContainer` and book them using `BookNTuple`.
void TrackSelector::BookNTuples()
{
  fLog << MSG::INFO << "Booking " << NTupleContainer::instances.size() << " NTuples:" << endmsg;
  std::map<std::string, NTupleContainer*>::iterator it = NTupleContainer::instances.begin();
  for(it; it != NTupleContainer::instances.end(); ++it) BookNTuple(*it->second);
}

/// Helper function that allows you to relate the `NTupleContainer` argument `tuple` to the output file (i.e. to 'book' it).
void TrackSelector::BookNTuple(NTupleContainer& tuple)
{
  /// -# **Abort** if the `"write_"` `JobSwitch` option has been set to `false`.
  if(!tuple.DoWrite()) return;
  /// -# Form a string for booking in the file. Typically: `"FILE1/<tree name>"`.
  const char* bookName = Form("FILE1/%s", tuple.Name().c_str());
  /// -# Attempt to get this `NTuple::Tuple` from file the file.
  NTuplePtr nt(ntupleSvc(), bookName);
  if(nt) fLog << MSG::INFO << "  loaded NTuple \"" << tuple.Name() << "\" from FILE1" << endmsg;
  /// -# If not available in file, book a new one.
  else
  {
    fLog << MSG::INFO << "  booked NTuple \"" << tuple.Name() << "\"" << endmsg;
    nt = ntupleSvc()->book(bookName, CLID_ColumnWiseTuple, tuple.Description());
    if(!nt)
      fLog << MSG::WARNING << "  --> cannot book N-tuple: " << long(nt) << " (\"" << tuple.Name()
           << "\")" << endmsg;
  }
  /// -# Import this NTuplePtr to the `tuple` object.
  tuple.SetTuplePtr(nt);
}

void TrackSelector::OverwriteCreateBits()
{
  fCreateNeutralCollection |= fNTuple_neutral.DoWrite();
  fCreateChargedCollection |= fNTuple_charged.DoWrite();
}

/// `"mult"`: Multiplicities of the total event
void TrackSelector::AddNTupleItems_mult()
{
  /// * `"Ntotal"`:   Total number of events per track.
  /// * `"Ncharge"`:  Number of charged tracks.
  /// * `"Nneutral"`: Number of charged tracks.
  /// * `"NgoodNeutral"`: Number of 'good' neutral tracks (if performed).
  /// * `"NgoodCharged"`: Number of 'good' charged tracks (if performed).
  fNTuple_mult.AddItem<int>("Ntotal");
  fNTuple_mult.AddItem<int>("Ncharge");
  fNTuple_mult.AddItem<int>("Nneutral");
  if(fCreateNeutralCollection) fNTuple_mult.AddItem<int>("NgoodNeutral");
  if(fCreateChargedCollection) fNTuple_mult.AddItem<int>("NgoodCharged");
}

/// `"vertex"`: Primary vertex info (interaction point)
void TrackSelector::AddNTupleItems_vertex()
{
  /// * `"vx0"`: \f$x\f$ coordinate of the interaction point.
  /// * `"vy0"`: \f$y\f$ coordinate of the interaction point.
  /// * `"vz0"`: \f$z\f$ coordinate of the interaction point.
  fNTuple_vertex.AddItem<double>("vx0");
  fNTuple_vertex.AddItem<double>("vy0");
  fNTuple_vertex.AddItem<double>("vz0");
}

/// `"charged"`: Charged track info: secondary vertact and distance to IP.
void TrackSelector::AddNTupleItems_charged()
{
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

/// `"neutral"`: Neutral track info.
void TrackSelector::AddNTupleItems_neutral()
{
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

/// This function encapsulates the `addItem` procedure for the \f$dE/dx\f$ energy loss branch (`"dedx"`). This method allows you to perform the same booking method for different types of charged particles (for instance 'all charged particles', kaons, and pions).
void TrackSelector::AddNTupleItems_dedx(NTupleContainer& tuple)
{
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

/// This function encapsulates the `addItem` procedure for the MC truth branches for the TopoAna package. Have a look at [this page](https://besiii.gitbook.io/boss/packages/analysis/topoana#preparing-initial-event-selection) as for why these fields are required.
void TrackSelector::AddNTupleItems_MCTruth(NTupleContainer& tuple)
{
  if(!tuple.DoWrite()) return;
  tuple.AddItem<int>("runID"); /// * `"runID"`: Run number ID.
  tuple.AddItem<int>("evtID"); /// * `"evtID"`: Rvent number ID.
  NTuple::Item<int>& index = *tuple.AddItem<int>(
    "index", 0, 100); /// * `"index"`: Index that is necessary for loading the following his one is
                      /// necessary for loading following two items, because they are arrays.
  tuple.AddIndexedItem<int>("particle",
                            index); /// * `"particle"`: PDG code for the particle in this array.
  tuple.AddIndexedItem<int>("mother", index); /// * `"mother"`: Track index of the mother particle.
}

/// `"PID"`: Track PID information.
void TrackSelector::AddNTupleItems_PID()
{
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

// * ============================ * //
// * ------- EXECUTE STEP ------- * //
// * ============================ * //

/// This method is called **for each event**.
StatusCode TrackSelector::execute()
{
  PrintFunctionName("TrackSelector", __func__);
  try
  {
    LoadDstFile();
    SetVertexOrigin();
    CreateCollections();
    WriteMultiplicities();
    WriteVertexInfo();
    execute_rest();
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
  fPIDInstance = ParticleID::instance();
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
  /// <li> @b Abort if `fCreateNeutralCollection` has been set to `false`. This is decided in the
  /// derived class.
  if(!fCreateNeutralCollection) return;

  /// <li> Clear `fNeutralTracks` collection `vector`.
  fNeutralTracks.clear();

  /// <li> @ Abort if there are no charged tracks in the `fEvtRecEvent` track collection.
  if(!fEvtRecEvent->totalNeutral())

    /// <li> Loop over the neutral tracks in the loaded `fEvtRecEvent` track collection. The second
    /// part of the set of reconstructed events consists of the neutral tracks, that is, the photons
    /// detected by the EMC (by clustering EMC crystal energies).
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
  fNTuple_mult.Write();
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

// * ============================= * //
// * ------- FINALIZE STEP ------- * //
// * ============================= * //

/// Is called at the end *of the entire process*.
/// Writes total cut flow to terminal and to the output file.
StatusCode TrackSelector::finalize()
{
  PrintFunctionName("TrackSelector", __func__);
  finalize_rest();
  NTupleContainer::PrintFilledTuples();
  AddAndWriteCuts();
  CutObject::PrintAll();
  return StatusCode::SUCCESS;
}

/// Write all cuts to a branch called "_cutvalues".
/// There will only be one entry per output file: a `C`-style array of size 3. Entry 0 of that array is the `min` value, entry 1 is the `max` value, and entry 2 is the recorded. As opposed to earlier versions of the `TrackSelector` (where the `"_cutvalues"` `TTree` contained 3 entries), we now use a `C`-style array (written through `NTuple::addIndexedItem`), because it allows to add up the number of counts when you have split up your jobs, which makes your output contain more than ROOT file. If you load these files as a `TChain`, you can get the total count of the `CutObject` by adding up all '2nd entries' (the counts) of each branch.
/// @see BOSSOutputLoader::PrintCuts
void TrackSelector::AddAndWriteCuts()
{
  std::list<CutObject*>::iterator cut   = CutObject::gCutObjects.begin();
  NTuple::Item<int>&              index = *fNTuple_cuts.AddItem<int>("index", 0, 3);
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

// * ===================================== * //
// * -------- FOR DERIVED CLASSES -------- * //
// * ===================================== * //

/// **Net charge cut**: Apply a strict cut on the total charge detected in the detectors. If this charge is not \f$0\f$, this means some charged tracks have not been detected.
void TrackSelector::CutZeroNetCharge()
{
  if(fNetChargeMDC) throw StatusCode::SUCCESS;
  ++fCutFlow_NetChargeOK;
}

/// Create a preselection of **Monte Carlo truth** tracks.
/// This method is used in `TrackSelector::execute` only. It is used to fill the `fMcParticles` `vector` with a selection of `McParticle` pointers. This collection starts with the initial cluster (e.g. \f$J/\psi\f$) and continues with the rest of the decay chain. Only then is it possible to use `CreateMCTruthSelection`, so it is called at the end.
/// @see `fMcParticles`
/// @see `WriteMCTruthForTopoAna`
/// @see `CreateMCTruthSelection`
/// @return Returns `true` if a collection of `Event::McParticle`s was successfully created.
bool TrackSelector::CreateMCTruthCollection()
{
  /// <ol>
  /// <li> @b Abort if input file is not MC generated (that is, if the run number is not negative).
  if(fEventHeader->runNumber() >= 0) return false;

  /// <li> @b Abort if `"write_topology"` job switch has been set to `false`.
  if(!fNTuple_topology.DoWrite()) return false;

  /// <li> Clear `fMcParticles` vector.
  fMcParticles.clear();

  /// <li> Load `McParticelCol` from `"/Event/MC/McParticleCol"` directory in `"FILE1"` input file
  /// and @b abort if does not exist.
  fMcParticleCol = SmartDataPtr<Event::McParticleCol>(eventSvc(), "/Event/MC/McParticleCol");
  if(!fMcParticleCol)
  {
    fLog << MSG::ERROR << "Could not retrieve McParticelCol" << endmsg;
    return false;
  }

  /// <li> Loop over collection of MC particles (`Event::McParticleCol`). For more info on the data
  /// available in `McParticle`, see
  /// [here](http://bes3.to.infn.it/Boss/7.0.2/html/McParticle_8h-source.html). Only add to
  /// `fMcParticles` if the `McParticle` satisfies:
  bool doNotInclude(true); // only start recording if set to false in the loop
  for(Event::McParticleCol::iterator it = fMcParticleCol->begin(); it != fMcParticleCol->end();
      ++it)
  {
    /// <ul>
    /// <li> @b Skip if the track is not a primary particle (has no mother). The initial meson towhich the beam is tuned is included, because its mother is a `cluster` or `string`.
    if((*it)->primaryParticle()) continue;
    /// <li> @b Skip if the track is not from the generator. This means that it is simulated in the detectors, but did not come from the event generator.
    if(!(*it)->decayFromGenerator()) continue;
    /// <li> Only start recording *after* we have passed the initial simulation `cluster` (code 91) or `string` (code 92). The next particle after this cluster or string will be the meson to which the beam is tuned (e.g. \f$J/\psi\f$). @see `NTupleTopoAna::IsInitialCluster`.
    if(doNotInclude && NTupleTopoAna::IsJPsi(*it)) doNotInclude = false;
    if(doNotInclude) continue;
    /// <li> Add the pointer to the `fMcParticles` collection vector for use in the derived algorithms.
    fMcParticles.push_back(*it);
    /// </ul>
  }

  /// <li> *(For the derived class:)*<br> Create selections of specific MC truth particles
  /// using `CreateMCTruthSelection`. Will not be performed if not specified in the derived
  /// algorithm.
  CreateMCTruthSelection();
  return true;

  /// </ol>
}

/// Encapsulates a `for` loop of the writing procedure for \f$dE/dx\f$ energy loss information.
/// This method allows you to write \f$dE/dx\f$ information for any selection of charged tracks. Just feed it a vector that contains such a collection of `EvtRecTrack` pointers.
void TrackSelector::WriteDedxInfoForVector(const std::vector<EvtRecTrack*>& vector,
                                           NTupleContainer&                 tuple)
{
  if(!tuple.DoWrite()) return;
  fLog << MSG::DEBUG << "Writing \"" << tuple.Name() << "\" info" << endmsg;
  for(std::vector<EvtRecTrack*>::const_iterator it = vector.begin(); it != vector.end(); ++it)
    WriteDedxInfo(*it, tuple);
}

/// This `virtual` method has been declared in the base algorithm to standardise the writing of a fit tuple.
/// @remark You should redefine the virtual `SetFitNTuple` method in your derived algorithm to specify what should be stored to this fit `NTuple`.
void TrackSelector::WriteFitResults(KKFitResult* fitresults, NTupleContainer& tuple)
{
  if(!tuple.DoWrite()) return;
  if(!fitresults) return;
  if(!fitresults->HasResults()) return;
  SetFitNTuple(fitresults, tuple);
  tuple.Write();
  fLog << MSG::DEBUG << "Writing fit results \"" << tuple.Name() << "\"" << endmsg;
}

/// Write an `NTuple` containing branches that are required for the `TopoAna` method.
/// @warning This method can be called only after `fMcParticles` has been filled using `CreateMCTruthCollection`.
/// @see `CreateMCTruthCollection`
bool TrackSelector::WriteMCTruthForTopoAna(NTupleContainer& tuple)
{
  if(fEventHeader->runNumber() >= 0) return false;
  if(!tuple.DoWrite()) return false;
  if(!fMcParticles.size()) return false;

  fLog << MSG::DEBUG << "Writing TopoAna NTuple \"" << tuple.Name() << "\" with "
       << fMcParticles.size() << " particles" << endmsg;
  tuple.GetItem<int>("runID") = fEventHeader->runNumber();
  tuple.GetItem<int>("evtID") = fEventHeader->eventNumber();

  /// -# The `trackIndex` of the first particle is to be the offset for the array index, because
  /// this entry should have array index `0`. See
  /// [here](https://besiii.gitbook.io/boss/besiii-software-system/packages/analysis/topoana#structure-of-the-event-mcparticlecol-collection)
  /// for more information on using `indexOffset`.
  std::vector<Event::McParticle*>::iterator it = fMcParticles.begin();
  int                                       indexOffset((*it)->trackIndex());

  /// -# Loop over the remainder of `fMcParticles` and store the daughters
  NTuple::Item<int>&  index    = tuple.GetItem<int>("index");
  NTuple::Array<int>& particle = tuple.GetArray<int>("particle");
  NTuple::Array<int>& mother   = tuple.GetArray<int>("mother");
  for(; it != fMcParticles.end(); ++it)
  {
    particle[index] = (*it)->particleProperty();
    mother[index]   = (*it)->mother().trackIndex() - indexOffset;
    if(!NTupleTopoAna::IsFromJPsi(*it)) --mother[index];
    if(NTupleTopoAna::IsJPsi(*it)) mother[index] = 0;
    ++index;
  }

  /// -# @b Write `NTuple` if `write` has been set to `true`.
  tuple.Write();
  return true;
}

/// Encapsulates the proces of writing PID info.
/// This allows you to write the PID information after the particle selection as well.
void TrackSelector::WritePIDInformation()
{
  /// -# @b Abort if the 'write `JobSwitch`' has been set to `false`.
  if(!fNTuple_PID.DoWrite()) return;

  /// -# @b Abort if there is no PID instance.
  if(!fPIDInstance) return;

  /// -# @b Abort if there is no `fTrackMDC`.
  if(!fTrackMDC) return;

  /// -# Get PID info and set the `NTuple::Item`s.
  fLog << MSG::DEBUG << "Writing PID information" << endmsg;
  fTrackMDC                               = (*fTrackIter)->mdcTrack();
  fNTuple_PID.GetItem<double>("p")        = fTrackMDC->p();
  fNTuple_PID.GetItem<double>("cost")     = cos(fTrackMDC->theta());
  fNTuple_PID.GetItem<double>("chiToFEC") = fPIDInstance->chiTofE(2);
  fNTuple_PID.GetItem<double>("chiToFIB") = fPIDInstance->chiTof1(2);
  fNTuple_PID.GetItem<double>("chiToFOB") = fPIDInstance->chiTof2(2);
  fNTuple_PID.GetItem<double>("chidEdx")  = fPIDInstance->chiDedx(2);
  fNTuple_PID.GetItem<double>("prob_K")   = fPIDInstance->probKaon();
  fNTuple_PID.GetItem<double>("prob_e")   = fPIDInstance->probElectron();
  fNTuple_PID.GetItem<double>("prob_mu")  = fPIDInstance->probMuon();
  fNTuple_PID.GetItem<double>("prob_p")   = fPIDInstance->probProton();
  fNTuple_PID.GetItem<double>("prob_pi")  = fPIDInstance->probPion();

  /// -# @b Write PID info.
  fNTuple_PID.Write();
}

/// Compute a 'momentum' for a neutral track.
/// The momentum is computed from the neutral track (photon) energy and from the location (angles) where it was detected in the EMC.
HepLorentzVector TrackSelector::ComputeMomentum(EvtRecTrack* track)
{
  fTrackEMC   = track->emcShower();
  double eraw = fTrackEMC->energy();
  double phi  = fTrackEMC->phi();
  double the  = fTrackEMC->theta();

  HepLorentzVector ptrk(eraw * sin(the) * cos(phi), // px
                        eraw * sin(the) * sin(phi), // py
                        eraw * cos(the),            // pz
                        eraw);
  // ptrk = ptrk.boost(-0.011, 0, 0); // boost to center-of-mass frame
  return ptrk;
}

/// Method that standardizes the initialisation of the particle identification system.
/// Define here *as general as possible*, but use in the derived subalgorithms. See http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html for more info.
ParticleID* TrackSelector::InitializePID(const int method, const int pidsys, const int pidcase,
                                         const double chimin)
{

  // * Initialise PID sub-system and set method: probability, likelihood, or neuron network
  fPIDInstance->init();
  fPIDInstance->setMethod(method);
  fPIDInstance->setChiMinCut(chimin);
  fPIDInstance->setRecTrack(*fTrackIter);

  // * Choose ID system and which particles to use
  fPIDInstance->usePidSys(pidsys);
  fPIDInstance->identify(pidcase);

  // * Perform PID
  fPIDInstance->calculate();
  if(!(fPIDInstance->IsPidInfoValid())) return nullptr;
  return fPIDInstance;
}

/// Check whether a decayed MC truth particle comes from a particle with PDG code `mother`.
bool TrackSelector::IsDecay(Event::McParticle* particle, const int mother) const
{
  if(!particle) return false;
  if(particle->primaryParticle()) return false;
  if(particle->mother().particleProperty() == mother) return true;
  return false;
}

/// Check whether a decayed MC truth particle has PDG code `pdg` and comes from a particle with PDG code `mother`.
bool TrackSelector::IsDecay(Event::McParticle* particle, const int mother, const int pdg) const
{
  if(!particle) return false;
  if(!IsDecay(particle, mother)) return false;
  if(particle->particleProperty() == pdg) return true;
  return false;
}