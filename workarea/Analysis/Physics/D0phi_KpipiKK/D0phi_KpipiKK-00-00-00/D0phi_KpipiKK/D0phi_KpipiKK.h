#ifndef Physics_Analysis_D0phi_KpipiKK_H
#define Physics_Analysis_D0phi_KpipiKK_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "D0phi_KpipiKK/KKFitResult_D0phi_KpipiKK.h"
#include "TrackSelector/TrackSelector.h"

// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_packages
/// @{

/// Analyse \f$ J/\psi \rightarrow D^0\phi \rightarrow K^-\pi^+\pi^0 \pi^-\pi^+ \f$ events.
/// This package has been created as an extension of the `D0phi_KpiKK` package. The motivation for
/// reconstructing the \f$D^0\f$ meson through the decay channel \f$D^0 \to K^-\pi^+\pi^0 K^-K^+\f$
/// (extra \f$\pi^0\f$) is that (1) its branching ratio is much larger (\f$(14.2 \pm 0.5)\%\f$
/// versus \f$(3.89 \pm 0.04)\%\f$ for \f$D^0 \to K^-\pi^+ K^-K^+\f$) and (2) that the extra
/// \f$\pi^0\f$ excludes the \f$f_0(1790)\f$ that we see as a large backround in the `D0phi_KpiKK`
/// package (due to parity). See [\f$D^0\f$ PDG
/// listing](http://pdg.lbl.gov/2018/listings/rpp2018-list-D-zero.pdf).
/// @todo Design another package for \f$K^-\pi^+\pi^-\pi^+\pi^- \pi^-\pi^+\f$ (branching fraction
/// \f$(8.11 \pm 0.15)\%\f$, see [\f$D^0\f$ PDG
/// listing](http://pdg.lbl.gov/2018/listings/rpp2018-list-D-zero.pdf)). However, that does mean the
/// final state will become rather complicated: \f$ J/\psi \rightarrow D^0\phi \rightarrow
/// K^-\pi^+\pi^+\pi^- \pi^-\pi^+ \f$.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     March 29th, 2019
class D0phi_KpipiKK : public TrackSelector
{
public:
  /// @name Constructors
  ///@{
  D0phi_KpipiKK(const std::string& name, ISvcLocator* pSvcLocator);
  ///@}

  /// @name Derived Algorithm steps
  ///@{
  StatusCode initialize_rest();
  StatusCode execute_rest();
  StatusCode finalize_rest();
  ///@}

protected:
  /// @name Track collections and iterators
  ///@{
  std::vector<Event::McParticle*> fMcKaonNeg; ///< Vector containing true \f$K^-\f$.
  std::vector<Event::McParticle*> fMcKaonPos; ///< Vector containing true \f$K^+\f$.
  std::vector<Event::McParticle*> fMcPhotons; ///< Vector containing true \f$\gamma\f$'s.
  std::vector<Event::McParticle*> fMcPionPos; ///< Vector containing true \f$\pi^+\f$.

  std::vector<Event::McParticle*>::iterator fMcPhoton1Iter;
  ///< Iterator for looping over the MC collection of photons (1st occurence).
  std::vector<Event::McParticle*>::iterator fMcPhoton2Iter;
  ///< Iterator for looping over the MC collection of photons (2st occurence).
  std::vector<Event::McParticle*>::iterator fMcKaonNeg1Iter;
  ///< Iterator for looping over the MC collection of negative kaons (1st occurence).
  std::vector<Event::McParticle*>::iterator fMcKaonNeg2Iter;
  ///< Iterator for looping over the MC collection of negative kaons (2st occurence).
  std::vector<Event::McParticle*>::iterator fMcKaonPosIter;
  ///< Iterator for looping over the MC collection of positive kaons.
  std::vector<Event::McParticle*>::iterator fMcPionPosIter;
  ///< Iterator for looping over the MC collection of positive pions.

  std::vector<EvtRecTrack*> fKaonNeg;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$K^-\f$. @todo
  ///< Decide if this can be formulated in terms of some `fEvtRecTrackMap`.
  std::vector<EvtRecTrack*> fKaonPos;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$K^+\f$.
  std::vector<EvtRecTrack*> fPhotons;
  ///< Vector that contains a selection of pointers to neutral tracks identified as \f$\gamma\f$.
  std::vector<EvtRecTrack*> fPionPos;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$\pi^+\f$.

  std::vector<EvtRecTrack*>::iterator fPhoton1Iter;
  ///< Iterator for looping over the collection of photons (1st occurence).
  std::vector<EvtRecTrack*>::iterator fPhoton2Iter;
  ///< Iterator for looping over the collection of photons (2st occurence).
  std::vector<EvtRecTrack*>::iterator fKaonNeg1Iter;
  ///< Iterator for looping over the collection of negative kaons (1st occurence).
  std::vector<EvtRecTrack*>::iterator fKaonNeg2Iter;
  ///< Iterator for looping over the collection of negative kaons (2st occurence).
  std::vector<EvtRecTrack*>::iterator fKaonPosIter;
  ///< Iterator for looping over the collection of positive kaons.
  std::vector<EvtRecTrack*>::iterator fPionPosIter;
  ///< Iterator for looping over the collection of positive pions.
  ///@}

  /// @name NTuples (eventual TTrees)
  ///@{
  NTupleContainer fNTuple_dedx_K; ///< `NTuple::Tuple` container for the \f$dE/dx\f$ of kaons.
  NTupleContainer fNTuple_dedx_pi;
  ///< `NTuple::Tuple` container for the \f$dE/dx\f$ of pions.
  NTupleContainer fNTuple_fit4c_all;
  ///< `NTuple::Tuple` container for the 4-constraint fit branch containing *all* combinations.
  NTupleContainer fNTuple_fit4c_best;
  ///< `NTuple::Tuple` container for the 4-constraint fit branch containing only the *best*
  ///< combination.
  NTupleContainer fNTuple_fit_mc;
  ///< `NTuple::Tuple` container for the 4-constraint fit of MC truth.
  NTupleContainer fNTuple_photon;
  ///< `NTuple::Tuple` container for the photon branch.
  ///@}

  /// @name Counters and cut objects
  ///@{
  CutObject fCutFlow_NChargedOK;
  ///< **Cut flow counter**: total number of events that have exactly the number of charged tracks
  ///< we want.
  CutObject fCutFlow_NFitOK;
  ///< **Cut flow counter**: total number of events where there is at least one combination where
  ///< the kinematic fit worked.
  CutObject fCutFlow_NPIDnumberOK;
  ///< **Cut flow counter**: total number of events that that has exactly the identified tracks that
  ///< we want.
  CutObject fCutFlow_NetChargeOK;
  ///< **Cut flow counter**: total number of events where the measured netto charge was \f$0\f$.
  ///< This cut is used to exclude events where some charged tracks were not detected (an
  ///< \f$e^+e^-\f$ collision has \f$0\f$ net charge).
  CutObject fCutFlow_mD0_mphi;
  ///< **Cut flow counter**: number of events that passed the wide cut on both masses.
  CutObject fCutFlow_mD0_mphi_3sig;
  ///< **Cut flow counter**: number of events that passed the \f$3\sigma\f$ cut on both masses.
  CutObject fCut_mD0;
  ///< Loose cut on \f$m_{D^0\to K^-\pi^+}\f$ invariant mass.
  CutObject fCut_mD0_3sig;
  ///< \f$3\sigma\f$ cut on \f$m_{D^0\to K^-\pi^+}\f$ invariant mass.
  CutObject fCut_mphi;
  ///< Loose cut on \f$m_{\phi\to K^+K^-}\f$ invariant mass.
  CutObject fCut_mphi_3sig;
  ///< \f$3\sigma\f$ cut on \f$m_{\phi\to K^+K^-}\f$ invariant mass.
  CutObject fCut_GammaAngle;
  ///< Cut on angle between the photon and the nearest charged track *in degrees*.
  CutObject fCut_GammaPhi;
  ///< Cut on \f$\phi\f$ angle between the photon and the nearest charged track *in radians*.
  CutObject fCut_GammaTheta;
  ///< Cut on \f$\theta\f$ angle between the photon and the nearest charged track *in radians*.
  ///@}

private:
  /// @name NTuple methods
  ///@{
  void AddNTupleItems_Fit(NTupleContainer& tuple);
  void CreateMCTruthSelection();
  void SetFitNTuple(KKFitResult* fitresults, NTupleContainer& tuple);
  ///@}
};

/// @}
#endif