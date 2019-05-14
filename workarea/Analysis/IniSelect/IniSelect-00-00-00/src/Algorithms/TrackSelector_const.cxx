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
  fNTuple_cuts("_cutvalues",
               "1st entry: min value, 2nd entry: max value, 3rd entry: number passed"),
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
  /// -# Also set `JobSwitches` of `"_cutvalues"` to `true`, because they have to be written no
  /// matter what the job option files says.
  fNTuple_cuts.SetWriteSwitch(true);
}