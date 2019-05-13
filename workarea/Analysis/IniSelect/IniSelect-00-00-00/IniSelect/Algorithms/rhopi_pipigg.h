#ifndef Physics_Analysis_rhopi_pipigg_H
#define Physics_Analysis_rhopi_pipigg_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "IniSelect/Algorithms/TrackSelector.h"
#include "IniSelect/Fit/KKFitResult_rhopi_pipigg.h"

// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_packages
/// @{

/// @b WIP Analyse \f$ J/\psi \rightarrow \rho^{0,\pm}\pi^{0,\mp} \rightarrow \pi^-\pi^+\gamma\gamma
/// \f$ events. Based on the original `RhopiAlg` package, but now making use of the `TrackSelector`
/// base algorithm and its `NTuple` booking procedure.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     December 27th, 2018
class rhopi_pipigg : public TrackSelector
{
public:
  /// @name Constructors
  ///@{
  rhopi_pipigg(const std::string& name, ISvcLocator* pSvcLocator);
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
  std::vector<Event::McParticle*> fMcPhotons; ///< Vector containing true \f$\gamma\f$'s.
  std::vector<Event::McParticle*> fMcPionNeg; ///< Vector containing true \f$\pi^-\f$.
  std::vector<Event::McParticle*> fMcPionPos; ///< Vector containing true \f$\pi^+\f$.
  std::vector<Event::McParticle*>::iterator
    fMcPhoton1Iter; ///< Iterator for looping over the MC collection of photons (1st occurence).
  std::vector<Event::McParticle*>::iterator
    fMcPhoton2Iter; ///< Iterator for looping over the MC collection of photons (2st occurence).
  std::vector<Event::McParticle*>::iterator
    fMcPionNegIter; ///< Iterator for looping over the MC collection of negative pions.
  std::vector<Event::McParticle*>::iterator
                            fMcPionPosIter; ///< Iterator for looping over the MC collection of positive pions.
  std::vector<EvtRecTrack*> fPhotons; ///< Vector that contains a selection of pointers to neutral
                                      ///< tracks identified as \f$\gamma\f$.
  std::vector<EvtRecTrack*> fPionNeg; ///< Vector that contains a selection of pointers to charged
                                      ///< tracks identified as \f$\pi^-\f$.
  std::vector<EvtRecTrack*> fPionPos; ///< Vector that contains a selection of pointers to charged
                                      ///< tracks identified as \f$\pi^+\f$.
  std::vector<EvtRecTrack*>::iterator
    fPhoton1Iter; ///< Iterator for looping over the collection of photons (1st occurence).
  std::vector<EvtRecTrack*>::iterator
    fPhoton2Iter; ///< Iterator for looping over the collection of photons (2st occurence).
  std::vector<EvtRecTrack*>::iterator
    fPionNegIter; ///< Iterator for looping over the collection of negative pions.
  std::vector<EvtRecTrack*>::iterator
    fPionPosIter; ///< Iterator for looping over the collection of positive pions.
  ///@}

  /// @name NTuples (eventual TTrees)
  ///@{
  NTupleContainer fNTuple_dedx_pi; ///< `NTuple::Tuple` container for the \f$dE/dx\f$ of pions.
  NTupleContainer fNTuple_fit4c;   ///< `NTuple::Tuple` container for the 4-constraint fit branch
                                   ///< containing *all* combinations.
  NTupleContainer fNTuple_fit5c;   ///< `NTuple::Tuple` container for the 5-constraint fit branch
                                   ///< containing *all* combinations.
  NTupleContainer
                  fNTuple_fit_mc; ///< `NTuple::Tuple` container for the 4-constraint fit of MC truth.
  NTupleContainer fNTuple_photon; ///< `NTuple::Tuple` container for the photon branch.
  ///@}

  /// @name Counters and cut objects
  ///@{
  CutObject fCutFlow_NChargedOK;   ///< **Cut flow counter**: total number of events that have at
                                   ///< least one charged track.
  CutObject fCutFlow_NFit4cOK;     ///< **Cut flow counter**: total number of events where the @b
                                   ///< 4-constraint fit was successful (no chi2 cut).
  CutObject fCutFlow_NFit5cOK;     ///< **Cut flow counter**: total number of events where the @b
                                   ///< 5-constraint fit was successful (no chi2 cut).
  CutObject fCutFlow_NNeutralOK;   ///< **Cut flow counter**: total number of events that have
                                   ///< exactly the number of neutral tracks we want.
  CutObject fCutFlow_NPIDnumberOK; ///< **Cut flow counter**: total number of events that that
                                   ///< has exactly the identified tracks that we want.
  CutObject fCutFlow_NetChargeOK;  ///< **Cut flow counter**: total number of events where the
                                   ///< measured netto charge was \f$0\f$. This cut is used to
                                   ///< exclude events where some charged tracks were not detected
                                   ///< (an \f$e^+e^-\f$ collision has \f$0\f$ net charge).
  CutObject fCutFlow_TopoAnaOK;    ///< **Cut flow counter**: total number of entries written to the
                                   ///< topoana branch.
  CutObject fCut_GammaAngle; ///< Cut on angle between the photon and the nearest charged track
                             ///< *in degrees*.
  CutObject fCut_GammaPhi;   ///< Cut on \f$\phi\f$ angle between the photon and the nearest charged
                             ///< track *in radians*.
  CutObject fCut_GammaTheta; ///< Cut on \f$\theta\f$ angle between the photon and the nearest
                             ///< charged track *in radians*.
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