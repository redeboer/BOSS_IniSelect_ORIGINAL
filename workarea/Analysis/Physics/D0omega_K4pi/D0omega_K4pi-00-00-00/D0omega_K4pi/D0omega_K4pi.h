#ifndef Physics_Analysis_D0omega_K4pi_H
#define Physics_Analysis_D0omega_K4pi_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "D0omega_K4pi/KKFitResult_D0omega_K4pi.h"
#include "TrackSelector/Containers/AngleDifferences.h"
#include "TrackSelector/TrackSelector.h"
#include "VertexFit/VertexFit.h"

// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_packages
/// @{

/// Analyse \f$ J/\psi \rightarrow D^0\omega \rightarrow K^-\pi^+\pi^0 \pi^-\pi^+ \f$ events.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 10th, 2019
class D0omega_K4pi : public TrackSelector
{
public:
  /// @name Constructors
  ///@{
  D0omega_K4pi(const std::string& name, ISvcLocator* pSvcLocator);
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
  std::vector<Event::McParticle*> fMcKaonNeg;
  ///< Vector containing true \f$K^-\f$.
  std::vector<Event::McParticle*> fMcPhotons;
  ///< Vector containing true \f$\gamma\f$'s.
  std::vector<Event::McParticle*> fMcPionNeg;
  ///< Vector containing true \f$\pi^-\f$.
  std::vector<Event::McParticle*> fMcPionPos;
  ///< Vector containing true \f$\pi^+\f$.
  std::vector<Event::McParticle*>::iterator fMcKaonNegIter;
  ///< Iterator for looping over the MC collection of negative kaons.
  std::vector<Event::McParticle*>::iterator fMcPhoton1Iter;
  ///< Iterator for looping over the MC collection of photons (1st occurence).
  std::vector<Event::McParticle*>::iterator fMcPhoton2Iter;
  ///< Iterator for looping over the MC collection of photons (2st occurence).
  std::vector<Event::McParticle*>::iterator fMcPionNegIter;
  ///< Iterator for looping over the MC collection of positive pions.
  std::vector<Event::McParticle*>::iterator fMcPionPos1Iter;
  ///< Iterator for looping over the MC collection of positive pions (1st occurence).
  std::vector<Event::McParticle*>::iterator fMcPionPos2Iter;
  ///< Iterator for looping over the MC collection of positive pions (2st occurence).
  std::vector<EvtRecTrack*> fKaonNeg;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$K^-\f$.
  std::vector<EvtRecTrack*> fPhotons;
  ///< Vector that contains a selection of pointers to neutral tracks identified as \f$\gamma\f$.
  std::vector<EvtRecTrack*> fPionNeg;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$\pi^-\f$.
  std::vector<EvtRecTrack*> fPionPos;
  ///< Vector that contains a selection of pointers to charged tracks identified as \f$\pi^+\f$.
  std::vector<EvtRecTrack*>::iterator fTrackIter2;
  std::vector<EvtRecTrack*>::iterator fKaonNegIter;
  ///< Iterator for looping over the collection of negative kaons.
  std::vector<EvtRecTrack*>::iterator fPhoton1Iter;
  ///< Iterator for looping over the collection of photons (1st occurence).
  std::vector<EvtRecTrack*>::iterator fPhoton2Iter;
  ///< Iterator for looping over the collection of photons (2st occurence).
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
  ///< `NTuple::Tuple` container for the 4-constraint fit branch containing only the *best* combination.
  NTupleContainer fNTuple_fit_mc;
  ///< `NTuple::Tuple` container for the 4-constraint fit of MC truth.
  NTupleContainer fNTuple_photon;
  ///< `NTuple::Tuple` container for the photon branch.
  ///@}

  /// @name Counters and cut objects
  ///@{
  CutObject fCutFlow_NChargedOK;
  ///< **Cut flow counter**: total number of events that have exactly the number of charged tracks we want.
  CutObject fCutFlow_NFitOK;
  ///< **Cut flow counter**: total number of events where there is at least one combination where the kinematic fit worked.
  CutObject fCutFlow_NPIDnumberOK;
  ///< **Cut flow counter**: total number of events that that has exactly the identified tracks that we want.
  CutObject fCut_GammaAngle;
  ///< Cut on angle between the photon and the nearest charged track *in degrees*.
  CutObject fCut_GammaPhi;
  ///< Cut on \f$\phi\f$ angle between the photon and the nearest charged track *in radians*.
  CutObject fCut_GammaTheta;
  ///< Cut on \f$\theta\f$ angle between the photon and the nearest charged track *in radians*.
  ///@}

  /// @name Fit objects
  ///@{
  VertexParameter          fVertexParameter;
  VertexFit*               fVertexFit;
  KalmanKinematicFit*      fKalmanKinematicFit;
  KKFitResult_D0omega_K4pi fBestKalmanFit;
  KKFitResult_D0omega_K4pi fCurrentKalmanFit;
  ///@}

  /// @name Photon kinematics
  ///@{
  Hep3Vector fEmcPosition;
  Hep3Vector fExtendedEmcPosition;
  ///@}

private:
  /// @name Helper methods for initialize
  ///@{
  void AddNTupleItems_mult_sel();
  void AddNTupleItems_dedx();
  void AddNTupleItems_fit();
  void AddNTupleItems_photon();
  void AddAdditionalNTuples_topology();
  ///@}

  /// @name Helper methods for execute
  ///@{
  void CutNumberOfChargedParticles();
  void CreateChargedTrackSelections();
  bool InitializePID();
  bool CategorizeTrack(EvtRecTrack* track);

  void CreateNeutralTrackSelections();
  void GetEmcPosition();
  bool GetExtendedEmcPosition();

  AngleDifferences FindSmallestPhotonAngles();
  void             WritePhotonKinematics(const AngleDifferences& angles);
  bool             CutPhotonAngles(const AngleDifferences& angles);

  void WriteMultiplicities();
  void PrintMultiplicities();
  void CutPID();
  void WriteDedxOfSelectedParticles();
  void AddNTupleItems_Fit(NTupleContainer& tuple);
  void SetFitNTuple(KKFitResult* fitresults, NTupleContainer& tuple);
  void FindBestKinematicFit();
  void ResetBestKalmanFit();
  void DoKinematicFitForAllCombinations();
  void BuildVertexParameter();

  void DoVertexFit();
  void InitializeVertexFit();
  void AddTracksToVertexFit();
  void AddVertexToVertexFit();

  void DoKinematicFit();
  void InitializeKinematicFit();
  void AddTracksToKinematicFit();
  void AddConstraintsToKinematicFit();
  void ExtractFitResults();

  void WriteBestFitWithMcTruth();
  void SetAdditionalNtupleItems_topology();
  void CreateMCTruthSelection();
  void PutParticleInCorrectVector(Event::McParticle* mcParticle);
  ///@}
};

/// @}
#endif