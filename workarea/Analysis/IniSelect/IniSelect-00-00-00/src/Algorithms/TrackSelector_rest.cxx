#include "IniSelect/Algorithms/TrackSelector.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "DstEvent/TofHitStatus.h"
#include "GaudiKernel/Bootstrap.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Handlers/ParticleIdentifier.h"
#include "TMath.h"
#include "TString.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"
#include <cmath>

using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
using namespace IniSelect;

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
  if(!fInputFile.IsMonteCarlo()) return false;

  /// <li> @b Abort if `"write_topology"` job switch has been set to `false`.
  if(!fNTuple_topology.DoWrite()) return false;

  /// <li> Clear `fMcParticles` vector.
  fMcParticles.clear();

  // /// <li> Load `McParticelCol` from `"/Event/MC/McParticleCol"` directory in `"FILE1"` input file
  // /// and @b abort if does not exist.
  // if(!fInputFile.LoadMcCollection())
  // {
  //   fLog << MSG::ERROR << "Could not retrieve McParticelCol" << endmsg;
  //   return false;
  // }

  /// <li> Loop over collection of MC particles (`Event::McParticleCol`). For more info on the data
  /// available in `McParticle`, see
  /// [here](http://bes3.to.infn.it/Boss/7.0.2/html/McParticle_8h-source.html). Only add to
  /// `fMcParticles` if the `McParticle` satisfies:
  bool doNotInclude(true); // only start recording if set to false in the loop
  McTrackIter iter(fInputFile);
  while(Event::McParticle* particle = iter.Next())
  {
    /// <ul>
    /// <li> @b Skip if the track is not a primary particle (has no mother). The initial meson towhich the beam is tuned is included, because its mother is a `cluster` or `string`.
    if(particle->primaryParticle()) continue;
    /// <li> @b Skip if the track is not from the generator. This means that it is simulated in the detectors, but did not come from the event generator.
    if(!particle->decayFromGenerator()) continue;
    /// <li> Only start recording *after* we have passed the initial simulation `cluster` (code 91) or `string` (code 92). The next particle after this cluster or string will be the meson to which the beam is tuned (e.g. \f$J/\psi\f$). @see `McTruth::IsInitialCluster`.
    if(doNotInclude && McTruth::IsJPsi(particle)) doNotInclude = false;
    if(doNotInclude) continue;
    /// <li> Add the pointer to the `fMcParticles` collection vector for use in the derived algorithms.
    fMcParticles.push_back(particle);
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
  if(!fInputFile.IsMonteCarlo()) return false;
  if(!tuple.DoWrite()) return false;
  if(!fMcParticles.size()) return false;

  fLog << MSG::DEBUG << "Writing TopoAna NTuple \"" << tuple.Name() << "\" with "
       << fMcParticles.size() << " particles" << endmsg;
  tuple.GetItem<int>("runID") = fInputFile.RunNumber();
  tuple.GetItem<int>("evtID") = fInputFile.EventNumber();

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
    if(!McTruth::IsFromJPsi(*it)) --mother[index];
    if(McTruth::IsJPsi(*it)) mother[index] = 0;
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