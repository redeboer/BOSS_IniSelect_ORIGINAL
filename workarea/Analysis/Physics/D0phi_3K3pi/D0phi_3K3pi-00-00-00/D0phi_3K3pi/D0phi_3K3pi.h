#ifndef Physics_Analysis_D0phi_3K3pi_H
#define Physics_Analysis_D0phi_3K3pi_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "D0phi_3K3pi/KKFitResult_D0phi_3K3pi.h"
#include "TrackSelector/TrackSelector.h"

// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_packages
/// @{

/// Analyse \f$ J/\psi \rightarrow D^0\phi \rightarrow K^-\pi^+ \pi^-\pi^+ \f$ events.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 6th, 2019
class D0phi_3K3pi : public TrackSelector
{
public:
  /// @name Constructors
  ///@{
  D0phi_3K3pi(const std::string& name, ISvcLocator* pSvcLocator);
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
  std::vector<Event::McParticle*> fMcPionNeg; ///< Vector containing true \f$\pi^-\f$.
  std::vector<Event::McParticle*> fMcPionPos; ///< Vector containing true \f$\pi^+\f$.

  std::vector<Event::McParticle*>::iterator fMcKaonNeg1Iter;
  ///< Iterator for looping over the MC collection of negative kaons (1st occurence).
  std::vector<Event::McParticle*>::iterator fMcKaonNeg2Iter;
  ///< Iterator for looping over the MC collection of negative kaons (2st occurence).
  std::vector<Event::McParticle*>::iterator fMcKaonPosIter;
  ///< Iterator for looping over the MC collection of positive kaons.
  std::vector<Event::McParticle*>::iterator fMcPionNegIter;
  ///< Iterator for looping over the MC collection of negative pions.
  std::vector<Event::McParticle*>::iterator fMcPionPos1Iter;
  ///< Iterator for looping over the MC collection of positive pions (1st occurence).
  std::vector<Event::McParticle*>::iterator fMcPionPos2Iter;
  ///< Iterator for looping over the MC collection of positive pions (2st occurence).

  std::vector<EvtRecTrack*> fKaonNeg;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$K^-\f$.
  std::vector<EvtRecTrack*> fKaonPos;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$K^+\f$.
  std::vector<EvtRecTrack*> fPionNeg;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$\pi^-\f$.
  std::vector<EvtRecTrack*> fPionPos;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$\pi^+\f$.

  std::vector<EvtRecTrack*>::iterator fKaonNeg1Iter;
  ///< Iterator for looping over the collection of negative kaons (1st occurence).
  std::vector<EvtRecTrack*>::iterator fKaonNeg2Iter;
  ///< Iterator for looping over the collection of negative kaons (2st occurence).
  std::vector<EvtRecTrack*>::iterator fKaonPosIter;
  ///< Iterator for looping over the collection of positive kaons.
  std::vector<EvtRecTrack*>::iterator fPionNegIter;
  ///< Iterator for looping over the collection of negative pions.
  std::vector<EvtRecTrack*>::iterator fPionPos1Iter;
  ///< Iterator for looping over the collection of positive pions (1st occurence).
  std::vector<EvtRecTrack*>::iterator fPionPos2Iter;
  ///< Iterator for looping over the collection of positive pions (2st occurence).
  ///@}

  /// @name NTuples (eventual TTrees)
  ///@{
  NTupleContainer fNTuple_dedx_K;
  ///< `NTuple::Tuple` container for the \f$dE/dx\f$ of kaons.
  NTupleContainer fNTuple_dedx_pi;
  ///< `NTuple::Tuple` container for the \f$dE/dx\f$ of pions.
  NTupleContainer fNTuple_fit4c_all;
  ///< `NTuple::Tuple` container for the 4-constraint fit branch containing *all* combinations.
  NTupleContainer fNTuple_fit4c_best;
  ///< `NTuple::Tuple` container for the 4-constraint fit branch containing only the *best*
  ///< combination.
  NTupleContainer fNTuple_fit_mc;
  ///< `NTuple::Tuple` container for the 4-constraint fit of MC truth.
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