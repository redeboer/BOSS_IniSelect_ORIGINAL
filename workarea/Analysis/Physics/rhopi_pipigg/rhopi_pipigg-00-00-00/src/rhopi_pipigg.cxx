// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "rhopi_pipigg/rhopi_pipigg.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "TrackSelector/TSGlobals.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include <float.h> // for DBL_MAX
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

/// Constructor for the `rhopi_pipigg` algorithm, derived from `TrackSelector`.
/// Here, you should declare properties: give them a name, assign a parameter (data member of
/// `rhopi_pipigg`), and if required a documentation string. Note that you should define the
/// paramters themselves in the header (rhopi_pipigg/rhopi_pipigg.h) and that you should assign the
/// values in `share/rhopi_pipigg.txt`.
rhopi_pipigg::rhopi_pipigg(const std::string& name, ISvcLocator* pSvcLocator) :
  /// * Construct base algorithm `TrackSelector`.
  TrackSelector(name, pSvcLocator),
  /// * Construct `NTuple::Tuple` containers used in derived classes.
  fNTuple_dedx_pi("dedx_pi", "dE/dx of the pions"),
  fNTuple_fit4c("fit4c", "4-constraint fit information (CMS 4-momentum)"),
  fNTuple_fit5c("fit5c", "5-constraint fit information (CMS 4-momentum + pi0 resonance)"),
  fNTuple_fit_mc("fit_mc", "Fake fit information according to MC truth"),
  fNTuple_photon("photon", "Kinematics of selected photons"),
  /// * Construct `CutObject`s. The `"cut_<parameter>_min/max"` properties determine cuts on certain
  /// parameters.
  fCutFlow_NChargedOK("N_charged_OK", "Number of events that had at least 1 charged track"),
  fCutFlow_NFit4cOK(
    "N_fit4c_OK", "Number of events where the 4-constraint kinematic was successful (no chi2 cut)"),
  fCutFlow_NFit5cOK(
    "N_fit5c_OK", "Number of events where the 5-constraint kinematic was successful (no chi2 cut)"),
  fCutFlow_NetChargeOK("N_netcharge_OK",
                       "Number of events where the total charge detected in the detectors was 0"),
  fCutFlow_NNeutralOK("N_neutral_OK", "Number of events that had at least 2 neutral tracks"),
  fCutFlow_NPIDnumberOK("N_PID_OK",
                        "Number of events that had exactly 2 K-, 1 K+ and 1 pi+ PID tracks"),
  fCutFlow_TopoAnaOK(
    "N_TopoAna_OK",
    "Number of entries that have been written to the branch for the topoana package"),
  /// * Construct additional `CutObject`s that are specific for the `rhopi_pipig` package.
  fCut_GammaAngle("gamma_angle"),
  fCut_GammaPhi("gamma_phi"),
  fCut_GammaTheta("gamma_theta")
{
  PrintFunctionName("rhopi_pipigg", __func__);
  PostConstructor();
  fCreateChargedCollection = true;
  fCreateNeutralCollection = true;
}

// * =============================== * //
// * ------- ALGORITHM STEPS ------- * //
// * =============================== * //

/// (Inherited) `initialize` step of `Algorithm`.
/// This function is called only once, when the algorithm is initialised.
/// @remark Define and load `NTuple`s here. Other `NTuple`s have already been defined in the
/// `TrackSelector::initilize` step prior to this this method.
StatusCode rhopi_pipigg::initialize_rest()
{
  PrintFunctionName("rhopi_pipigg", __func__);
  /// <ol type="A">
  /// <li> `"mult_select"`: Multiplicities of selected particles
  /// <ol>
  fNTuple_mult_sel.AddItem<int>("NPhotons"); /// <li> `"NPhotons"`: Number of \f$\gamma\f$'s.
  fNTuple_mult_sel.AddItem<int>("NPionNeg"); /// <li> `"NPionNeg"`: Number of \f$\pi^-\f$.
  fNTuple_mult_sel.AddItem<int>("NPionPos"); /// <li> `"NPionPos"`: Number of \f$\pi^+\f$.
  /// </ol>

  /// <li> `"dedx_pi"`: energy loss \f$dE/dx\f$ PID branch. See `TrackSelector::AddNTupleItems_Dedx`
  /// for more info.
  AddNTupleItems_Dedx(fNTuple_dedx_pi);

  /// <li> `"fit4c"` and `"fit5c"`: results of the Kalman kinematic fit results. See
  /// `TrackSelector::AddNTupleItems_Fit` for more info.
  AddNTupleItems_Fit(fNTuple_fit4c);
  AddNTupleItems_Fit(fNTuple_fit5c);
  AddNTupleItems_Fit(fNTuple_fit_mc);

  /// <li> `"photon"`: information of the selected photons
  /// <ol>
  fNTuple_photon.AddItem<double>("E");  /// <li> `"E"`:  Energy of the photon.
  fNTuple_photon.AddItem<double>("px"); /// <li> `"px"`: \f$x\f$ component of the 4-momentum of the
                                        /// photon (computed from the detected angles).
  fNTuple_photon.AddItem<double>("py"); /// <li> `"py"`: \f$y\f$ component of the 4-momentum of the
                                        /// photon (computed from the detected angles).
  fNTuple_photon.AddItem<double>("pz"); /// <li> `"pz"`: \f$z\f$ component of the 4-momentum of the
                                        /// photon (computed from the detected angles).
  fNTuple_photon.AddItem<double>(
    "smallest_phi"); /// <li> `"phi"`:   Smallest \f$\phi\f$ angle between the photon and the
                     /// nearest charged pion.
  fNTuple_photon.AddItem<double>(
    "smallest_theta"); /// <li> `"theta"`: Smallest \f$\theta\f$ angle between the photon and the
                       /// nearest charged pion.
  fNTuple_photon.AddItem<double>("smallest_angle"); /// <li> `"angle"`: Smallest angle between the
                                                    /// photon and the nearest charged pion.
                                                    /// </ol>

  /// </ol>
  return StatusCode::SUCCESS;
}

/// Inherited `execute` method of the `Algorithm`. This function is called *for each event*.
StatusCode rhopi_pipigg::execute_rest()
{
  PrintFunctionName("rhopi_pipigg", __func__);
  /// <ol type="A">
  /// <li> **Charged track cut**: Apply a strict cut on the number of particles. Only **2 charged
  /// tracks in total**.
  if(fChargedTracks.size() != 2) return StatusCode::SUCCESS;
  ++fCutFlow_NChargedOK;

  /// <li> **Net charge cut**: Apply a strict cut on the total charge detected in the detectors. If
  /// this charge is not \f$0\f$, this means some charged tracks have not been detected.
  if(fNetChargeMDC) return StatusCode::SUCCESS;
  ++fCutFlow_NetChargeOK;

  /// <li> Create selection of **charged** tracks.
  // * Clear vectors of selected particles *
  fPionNeg.clear();
  fPionPos.clear();

  // * Loop over charged tracks *
  for(fTrackIter = fChargedTracks.begin(); fTrackIter != fChargedTracks.end(); ++fTrackIter)
  {
    /// <ol>
    /// <li> Initialise PID and skip if it fails:
    /// <ul>
    if(!InitializePID(
         /// <li> use **probability method**
         fPIDInstance->methodProbability(),
         /// <li> use \f$dE/dx\f$ and the three ToF detectors. Since BOSS 7.0.4,
         /// `ParticleID::useTofCorr` should be used for ToF instead of e.g. `useTof1`.
         fPIDInstance->useDedx() | fPIDInstance->useTof1() | fPIDInstance->useTof2() |
           fPIDInstance->useTofE(),
         /// <li> identify only pions
         fPIDInstance->onlyPion(),
         /// <li> use \f$\chi^2 > 4.0\f$
         4.0))
      continue;
    /// </ul>

    /// <li> **Write** Particle Identification information of all tracks
    WritePIDInformation();

    /// <li> Identify type of charged particle and add to related vector: this package: only pions.
    fTrackKal = (*fTrackIter)->mdcKalTrack();
    if(fCut_PIDProb.FailsMin(fPIDInstance->probPion()))
      continue; /// A cut is then applied on whether the probability to be a pion (or kaon) is at
                /// least `fCut_PIDProb_min` (see eventual settings in `rhopi_pipigg.txt`).
    RecMdcKalTrack::setPidType(RecMdcKalTrack::pion); /// Finally, the particle ID of the
                                                      /// `RecMdcKalTrack` object is set to pion
    if(fTrackKal->charge() > 0)
      fPionPos.push_back(
        *fTrackIter); /// and the pions (\f$\pm\f$) are added to the respective vector of pions.
    else
      fPionNeg.push_back(*fTrackIter);

    /// </ol>
  }

  /// <li> Create selection **neutral** tracks (photons)
  fPhotons.clear();
  for(fTrackIter = fNeutralTracks.begin(); fTrackIter != fNeutralTracks.end(); ++fTrackIter)
  {

    /// <ol>
    /// <li> Get EM calorimeter info.
    fTrackEMC         = (*fTrackIter)->emcShower();
    Hep3Vector emcpos = fTrackEMC->position();

    /// <li> Find angle differences with nearest charged pion.
    double smallestTheta = DBL_MAX; // start value for difference in theta
    double smallestPhi   = DBL_MAX; // start value for difference in phi
    double smallestAngle = DBL_MAX; // start value for difference in angle (?)
    for(fPionNegIter = fChargedTracks.begin(); fPionNegIter != fChargedTracks.end(); ++fPionNegIter)
    {
      /// * Get the extension object from MDC to EMC.
      if(!(*fPionNegIter)->isExtTrackValid()) continue;
      fTrackExt = (*fPionNegIter)->extTrack();
      if(fTrackExt->emcVolumeNumber() == -1) continue;
      Hep3Vector extpos(fTrackExt->emcPosition());

      /// * Get angles in @b radians.
      // double cosTheta = extpos.cosTheta(emcpos);
      double angle  = extpos.angle(emcpos);
      double dTheta = extpos.theta() - emcpos.theta();
      double dPhi   = extpos.deltaPhi(emcpos);
      dTheta        = fmod(dTheta + CLHEP::twopi + CLHEP::twopi + pi, CLHEP::twopi) - CLHEP::pi;
      dPhi          = fmod(dPhi + CLHEP::twopi + CLHEP::twopi + pi, CLHEP::twopi) - CLHEP::pi;
      if(angle < smallestAngle)
      {
        smallestAngle = angle;
        smallestTheta = dTheta;
        smallestPhi   = dPhi;
      }
    }

    /// <li> Convert angles from radians to degrees.
    smallestTheta *= (180. / (CLHEP::pi));
    smallestPhi *= (180. / (CLHEP::pi));
    smallestAngle *= (180. / (CLHEP::pi));

    /// <li> @b Write photon info (`"photon"` branch).
    if(fNTuple_photon.DoWrite())
    {
      double           eraw  = fTrackEMC->energy();
      double           phi   = fTrackEMC->phi();
      double           theta = fTrackEMC->theta();
      HepLorentzVector four_mom(eraw * sin(theta) * cos(phi), eraw * sin(theta) * sin(phi),
                                eraw * cos(theta), eraw);
      fNTuple_photon.GetItem<double>("E")              = four_mom.e();
      fNTuple_photon.GetItem<double>("px")             = four_mom.px();
      fNTuple_photon.GetItem<double>("py")             = four_mom.py();
      fNTuple_photon.GetItem<double>("pz")             = four_mom.pz();
      fNTuple_photon.GetItem<double>("smallest_phi")   = smallestTheta;
      fNTuple_photon.GetItem<double>("smallest_theta") = smallestPhi;
      fNTuple_photon.GetItem<double>("smallest_angle") = smallestAngle;
      fNTuple_photon.Write();
    }

    /// <li> Apply angle cut on the photons: you do not want to photons to be too close to any
    /// charged track to avoid noise from EMC showers that came from a charged track.
    if(fCut_GammaTheta.FailsCut(fabs(smallestTheta)) && fCut_GammaPhi.FailsCut(fabs(smallestPhi)))
      continue;
    if(fCut_GammaAngle.FailsCut(fabs(smallestAngle))) continue;

    /// <li> Add photon track to vector for gammas
    fPhotons.push_back(*fTrackIter);

    /// </ol>
  }

  /// <li> **PID cut**: apply a strict cut on the number of the selected particles. Only continue
  /// if: <ol>
  if(fPhotons.size() < 2) return StatusCode::SUCCESS;  /// <li> at least 2 photons (\f$\gamma\f$'s)
  if(fPionNeg.size() != 1) return StatusCode::SUCCESS; /// <li> 1 negative pion (\f$\pi^-\f$)
  if(fPionPos.size() != 1) return StatusCode::SUCCESS; /// <li> 1 positive pion (\f$\pi^+\f$)
  /// </ol>
  ++fCutFlow_NPIDnumberOK;

  /// <li> Create selection of MC truth particles by looping over the collection of MC particles
  /// created in `TrackSelector::execute()`. See
  /// [here](http://home.fnal.gov/~mrenna/lutp0613man2/node44.html) for a list of PDG codes.
  if(fEventHeader->runNumber() < 0 && fNTuple_fit_mc.DoWrite())
  {
    fMcPhotons.clear();
    fMcPionNeg.clear();
    fMcPionPos.clear();
    std::vector<Event::McParticle*>::iterator it;
    for(it = fMcParticles.begin(); it != fMcParticles.end(); ++it)
    {
      switch((*it)->particleProperty())
      {
        case 22: fMcPhotons.push_back(*it); break;
        case -211: fMcPionNeg.push_back(*it); break;
        case 211: fMcPionPos.push_back(*it); break;
        default:
          fLog << MSG::DEBUG << "No switch case defined for McParticle "
               << (*it)->particleProperty() << endmsg;
      }
    }
  }

  /// <li> **Write** the multiplicities of the selected particles.
  fLog << MSG::DEBUG << "N_\gamma = " << fPhotons.size() << ", "
       << "N_{\pi^-} = " << fPionNeg.size() << ", "
       << "N_{\pi^+} = " << fPionPos.size() << endmsg;
  if(fNTuple_mult_sel.DoWrite())
  {
    fNTuple_mult_sel.GetItem<int>("NPhotons") = fPhotons.size();
    fNTuple_mult_sel.GetItem<int>("NPionNeg") = fPionNeg.size();
    fNTuple_mult_sel.GetItem<int>("NPionPos") = fPionPos.size();
    fNTuple_mult_sel.Write();
  }

  /// <li> **Write** \f$dE/dx\f$ PID information (`"dedx_pi"` branch)
  if(fNTuple_dedx.DoWrite())
  {
    WriteDedxInfoForVector(fPionNeg, fNTuple_dedx_pi);
    WriteDedxInfoForVector(fPionPos, fNTuple_dedx_pi);
  }

  /// <li> Loop over MC truth of final decay products.
  for(fMcPhoton1Iter = fMcPhotons.begin(); fMcPhoton1Iter != fMcPhotons.end(); ++fMcPhoton1Iter)
    for(fMcPhoton2Iter = fMcPhotons.begin(); fMcPhoton2Iter != fMcPhotons.end(); ++fMcPhoton2Iter)
      for(fMcPionNegIter = fMcPionNeg.begin(); fMcPionNegIter != fMcPionNeg.end(); ++fMcPionNegIter)
        for(fMcPionPosIter = fMcPionPos.begin(); fMcPionPosIter != fMcPionPos.end();
            ++fMcPionPosIter)
        {
          /// <ol>
          /// <li> Only continue if the two photons are different.
          if(fMcPhoton1Iter == fMcPhoton2Iter) continue;

          /// <li> Check topology: only consider that combination which comes from \f$J/\psi
          /// \rightarrow \pi^+\pi^-\gamma\gamma\f$.
          if(!IsDecay(*fMcPhoton1Iter, 111, 22)) continue; // D0  --> K-
          if(!IsDecay(*fMcPhoton2Iter, 111, 22)) continue; // D0  --> pi+
          // if(!(IsDecay(*fMcPionNegIter, , -211) || IsDecay(*fMcPionNegIter, , -211))) continue;
          // // phi --> K- if(!(IsDecay(*fMcPionPosIter, ,  211) || IsDecay(*fMcPionPosIter, ,
          // 211))) continue; // phi --> K+

          /// <li> Write 'fake' fit results, that is, momenta of the particles reconstructed from MC
          /// truth.
          KKFitResult_rhopi_pipigg fitresult(*fMcPhoton1Iter, *fMcPhoton2Iter, *fMcPionNegIter,
                                             *fMcPionPosIter);
          WriteFitResults(&fitresult, fNTuple_fit_mc);
          /// </ol>
        }

  /// <li> Perform Kalman **4-constraint** Kalman kinematic fit for all combinations
  /// <ol>
  if(fNTuple_fit4c.DoWrite())
  {
    /// <li> Reset best fit parameters (see `KKFitResult_rhopi_pipigg`).
    KKFitResult_rhopi_pipigg bestKalmanFit;
    bestKalmanFit.ResetBestCompareValue();
    /// <li> Loop over all combinations of \f$\gamma\f$, \f$K^-\f$, \f$K^+\f$, and \f$\pi^+\f$.
    /// <ol>
    bool printfit{true};
    int  count = 0;
    for(fPhoton1Iter = fPhotons.begin(); fPhoton1Iter != fPhotons.end(); ++fPhoton1Iter)
      for(fPhoton2Iter = fPhotons.begin(); fPhoton2Iter != fPhotons.end(); ++fPhoton2Iter)
        for(fPionPosIter = fPionPos.begin(); fPionPosIter != fPionPos.end(); ++fPionPosIter)
          for(fPionPosIter = fPionPos.begin(); fPionPosIter != fPionPos.end(); ++fPionPosIter)
          {
            /// <li> Only continue if we are not dealing with the same kaon.
            if(fPhoton1Iter == fPhoton2Iter) continue;
            fLog << MSG::INFO << "  fitting combination " << count << "..." << endmsg;

            /// <li> Get Kalman tracks.
            RecMdcKalTrack* pimTrk = (*fPionNegIter)->mdcKalTrack();
            RecMdcKalTrack* pipTrk = (*fPionPosIter)->mdcKalTrack();

            /// <li> Get W-tracks.
            WTrackParameter wvpimTrk(Mass::pi, pimTrk->getZHelix(), pimTrk->getZError());
            WTrackParameter wvpipTrk(Mass::pi, pipTrk->getZHelix(), pipTrk->getZError());

            /// <li> Initiate vertex fit.
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

            /// <li> Test vertex fit.
            VertexFit* vtxfit = VertexFit::instance();
            vtxfit->init();
            vtxfit->AddTrack(0, wvpimTrk);
            vtxfit->AddTrack(1, wvpipTrk);
            vtxfit->AddVertex(0, vxpar, 0, 1);
            if(!vtxfit->Fit(0))
            {
              fLog << MSG::WARNING << "vertex fit failed" << endmsg;
              continue;
            }
            vtxfit->Swim(0);

            /// <li> Get Kalman kinematic fit for this combination and store if better than previous
            /// one.
            KalmanKinematicFit* kkmfit = KalmanKinematicFit::instance();
            kkmfit->init();
            kkmfit->AddTrack(0, vtxfit->wtrk(0));                  // pi-
            kkmfit->AddTrack(1, vtxfit->wtrk(1));                  // pi+
            kkmfit->AddTrack(2, 0., (*fPhoton1Iter)->emcShower()); // gamma (1st occurrence)
            kkmfit->AddTrack(3, 0., (*fPhoton2Iter)->emcShower()); // gamma (2nd occurence)
            kkmfit->AddFourMomentum(0, gEcmsVec); // 4 constraints: CMS energy and 3-momentum
            if(kkmfit->Fit())
            {
              /// <ol>
              /// <li> Apply max. \f$\chi^2\f$ cut (determined by `fCut_PIDChiSq_max`).
              if(fCut_PIDChiSq.FailsMax(kkmfit->chisq())) continue;
              /// <li> Construct fit result object for this combintation.
              KKFitResult_rhopi_pipigg fitresult(kkmfit);
              /// <li> @b Write results of the Kalman kinematic fit.
              WriteFitResults(&fitresult, fNTuple_fit4c);
              ++fCutFlow_NFit4cOK;
              /// <li> @b Write MC truth collection for the `topoana` package.
              if(CreateMCTruthCollection() && WriteMCTruthForTopoAna(fNTuple_topology))
                ++fCutFlow_TopoAnaOK;
              /// </ol>
            }
            ++count;
          }
    /// </ol>
  }
  /// </ol>

  /// <li> Perform Kalman **5-constraint** Kalman kinematic fit for all combinations.
  /// <ol>
  if(fNTuple_fit5c.DoWrite())
  {
    /// <li> Reset best fit parameters.
    double              bestFitMeasure = 1e5;
    KalmanKinematicFit* bestKalmanFit  = nullptr;
    /// <li> Loop over all combinations of pi-, pi+, and gamma.
    /// <ol>
    for(fPhoton1Iter = fPhotons.begin(); fPhoton1Iter != fPhotons.end(); ++fPhoton1Iter)
      for(fPhoton2Iter = fPhotons.begin(); fPhoton2Iter != fPhotons.end(); ++fPhoton2Iter)
        for(fPionPosIter = fPionPos.begin(); fPionPosIter != fPionPos.end(); ++fPionPosIter)
          for(fPionPosIter = fPionPos.begin(); fPionPosIter != fPionPos.end(); ++fPionPosIter)
          {
            /// <li> Only continue if we are not dealing with the same kaon.
            if(fPhoton1Iter == fPhoton2Iter) continue;

            /// <li> Get Kalman tracks.
            RecMdcKalTrack* pimTrk = (*fPionNegIter)->mdcKalTrack();
            RecMdcKalTrack* pipTrk = (*fPionPosIter)->mdcKalTrack();

            /// <li> Get W-tracks.
            WTrackParameter wvpimTrk(Mass::pi, pimTrk->getZHelix(), pimTrk->getZError());
            WTrackParameter wvpipTrk(Mass::pi, pipTrk->getZHelix(), pipTrk->getZError());

            /// <li> Initiate vertex fit.
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

            /// <li> Test vertex fit.
            VertexFit* vtxfit = VertexFit::instance();
            vtxfit->init();
            vtxfit->AddTrack(0, wvpimTrk);
            vtxfit->AddTrack(1, wvpipTrk);
            vtxfit->AddVertex(0, vxpar, 0, 1);
            if(!vtxfit->Fit(0))
            {
              fLog << MSG::WARNING << "vertex fit failed" << endmsg;
              continue;
            }
            vtxfit->Swim(0);

            /// <li> Get Kalman kinematic fit for this combination and store if better than previous
            /// one.
            KalmanKinematicFit* kkmfit = KalmanKinematicFit::instance();
            kkmfit->init();
            kkmfit->AddTrack(0, vtxfit->wtrk(0));                  // pi-
            kkmfit->AddTrack(1, vtxfit->wtrk(1));                  // pi+
            kkmfit->AddTrack(2, 0., (*fPhoton1Iter)->emcShower()); // gamma (1st occurrence)
            kkmfit->AddTrack(3, 0., (*fPhoton2Iter)->emcShower()); // gamma (2nd occurence)
            kkmfit->AddResonance(0, Mass::pi0, 2,
                                 3);              /// @remark 5th constraint: \f$\pi^0\f$ resonance
            kkmfit->AddFourMomentum(1, gEcmsVec); // 4 constraints: CMS energy and momentum
            if(kkmfit->Fit())
            {
              /// <ol>
              /// <li> Apply max. \f$\chi^2\f$ cut (determined by `fCut_PIDChiSq_max`).
              if(fCut_PIDChiSq.FailsMax(kkmfit->chisq())) continue;
              /// <li> Construct fit result object for this combintation.
              KKFitResult_rhopi_pipigg fitresult(kkmfit);
              /// <li> @b Write results of the Kalman kinematic fit.
              WriteFitResults(&fitresult, fNTuple_fit5c);
              ++fCutFlow_NFit5cOK;
              /// </ol>
            }
          }
    /// </ol>
  }
  /// </ol>

  /// </ol>
  return StatusCode::SUCCESS;
}

/// Currently does nothing.
/// See `TrackSelector::finalize` for what else is done when finalising.
StatusCode rhopi_pipigg::finalize_rest()
{
  PrintFunctionName("rhopi_pipigg", __func__);
  return StatusCode::SUCCESS;
}

// * ============================== * //
// * ------- NTUPLE METHODS ------- * //
// * ============================== * //

/// This function encapsulates the `addItem` procedure for the fit branches.
void rhopi_pipigg::AddNTupleItems_Fit(NTupleContainer& tuple)
{
  if(!tuple.DoWrite()) return;
  tuple.AddItem<double>(
    "mpi0"); /// * `"mpi0"`:       Invariant mass for \f$\pi^0 \to \gamma\gamma\f$ candidate.
  tuple.AddItem<double>(
    "mrho0"); /// * `"mrho0"`:      Invariant mass for \f$\rho^0 \to \pi^-\pi^+\f$ candidate.
  tuple.AddItem<double>(
    "mrho-"); /// * `"mrho-"`:      Invariant mass for \f$\rho^- \to \pi^0\pi^-\f$ candidate.
  tuple.AddItem<double>(
    "mrho+"); /// * `"mrho+"`:      Invariant mass for \f$\rho^+ \to \pi^0\pi^+\f$ candidate.
  tuple.AddItem<double>(
    "mJpsi_rho0"); /// * `"mJpsi_rho0"`: Invariant mass for \f$J/\psi \to \rho^0\pi^0\f$ candidate.
  tuple.AddItem<double>(
    "mJpsi_rho-"); /// * `"mJpsi_rho-"`: Invariant mass for \f$J/\psi \to \rho^-\pi^+\f$ candidate.
  tuple.AddItem<double>(
    "mJpsi_rho+"); /// * `"mJpsi_rho+"`: Invariant mass for \f$J/\psi \to \rho^+\pi^-\f$ candidate.
  tuple.AddItem<double>("chisq"); /// * `"chisq"`:      \f$\chi^2\f$ of the Kalman kinematic fit.
}

/// Specification of `TrackSelector::CreateMCTruthSelection`.
/// Create selection of MC truth particles by looping over the collection of MC particles created by
/// `TrackSelector::CreateMCTruthCollection()`.
void rhopi_pipigg::CreateMCTruthSelection()
{
  /// -# @b Abort if input file is not from a Monte Carlo simulation (that is, if the run number is
  /// not negative).
  if(fEventHeader->runNumber() >= 0) return;
  /// -# @b Abort if `"write_fit_mc"`, has been set to `false`.
  if(!fNTuple_fit_mc.DoWrite()) return;
  /// -# Clear MC truth particle selections.
  fMcPhotons.clear();
  fMcPionNeg.clear();
  fMcPionPos.clear();
  /// -# Loop over `fMcParticles` collection of MC truth particles and fill the selections.
  std::vector<Event::McParticle*>::iterator it;
  for(it = fMcParticles.begin(); it != fMcParticles.end(); ++it)
  {
    switch((*it)->particleProperty())
    {
      case 22: fMcPhotons.push_back(*it); break;
      case -211: fMcPionNeg.push_back(*it); break;
      case 211: fMcPionPos.push_back(*it); break;
      default:
        fLog << MSG::DEBUG << "No switch case defined for McParticle " << (*it)->particleProperty()
             << endmsg;
    }
  }
}

/// Specification of what should be written to the fit `NTuple`.
/// This function is called in `TrackSelector::WriteFitResults`.
void rhopi_pipigg::SetFitNTuple(KKFitResult* fitresults, NTupleContainer& tuple)
{
  /// -# Convert to the derived object of `KKFitResult` designed for this package. @remark This cast
  /// is required and cannot be solved using virtual methods, because of the specific structure of
  /// each analysis.
  KKFitResult_rhopi_pipigg* fit = dynamic_cast<KKFitResult_rhopi_pipigg*>(fitresults);

  /// -# @warning Terminate if cast failed.
  if(!fit)
  {
    std::cout << "FATAL ERROR: Dynamic cast failed" << std::endl;
    std::terminate();
  }

  /// -# Set the `NTuple::Item`s.
  tuple.GetItem<double>("mpi0")       = fit->fM_pi0;
  tuple.GetItem<double>("mrho0")      = fit->fM_rho0;
  tuple.GetItem<double>("mrho-")      = fit->fM_rhom;
  tuple.GetItem<double>("mrho+")      = fit->fM_rhop;
  tuple.GetItem<double>("mJpsi_rho0") = fit->fM_JpsiRho0;
  tuple.GetItem<double>("mJpsi_rho-") = fit->fM_JpsiRhom;
  tuple.GetItem<double>("mJpsi_rho+") = fit->fM_JpsiRhop;
  tuple.GetItem<double>("chisq")      = fit->fChiSquared;
}