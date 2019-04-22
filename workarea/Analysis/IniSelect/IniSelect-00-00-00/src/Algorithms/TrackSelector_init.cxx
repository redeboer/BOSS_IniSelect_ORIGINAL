#include "IniSelect/Algorithms/TrackSelector.h"

#include "DstEvent/TofHitStatus.h"
#include "GaudiKernel/Bootstrap.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Handlers/ParticleIdentifier.h"
#include "TMath.h"
#include "TString.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"
#include <cmath>

using namespace IniSelect;

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
    AddNTupleItems();
    AddAdditionalNTupleItems();
    ConfigureParticleSelection();
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
  if(!tuple.DoWrite())
  {
    fLog << MSG::DEBUG << "  NTuple \"" << tuple.Name() << "\" will not be written" << endmsg;
    return;
  }
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
  CandidateTracks<EvtRecTrack>* coll = fParticleSel.FirstParticle();
  while(coll)
  {
    fNTuple_mult.AddItem<int>(Form("N_%s", coll->GetPdtName()));
    coll = fParticleSel.NextCharged();
  }
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