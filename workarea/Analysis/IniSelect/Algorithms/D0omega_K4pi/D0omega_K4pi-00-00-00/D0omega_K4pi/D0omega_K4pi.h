#ifndef Physics_Analysis_D0omega_K4pi_H
#define Physics_Analysis_D0omega_K4pi_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "D0omega_K4pi/KKFitResult_D0omega_K4pi.h"
#include "TrackSelector/TrackSelector.h"

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
  D0omega_K4pi(const std::string& name, ISvcLocator* pSvcLocator);

protected:
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
  ///@}

  /// @name Fit objects
  ///@{
  KKFitResult_D0omega_K4pi fBestKalmanFit;
  KKFitResult_D0omega_K4pi fCurrentKalmanFit;
  ///@}

private:
  /// @name Helper methods for initialize
  ///@{
  void ConfigureParticleSelection();
  void AddAdditionalNTupleItems();
  void AddNTupleItems_dedx();
  void AddNTupleItems_fit();
  void AddNTupleItems_fit(NTupleContainer& tuple);
  void AddAdditionalNTuples_topology();
  ///@}

  /// @name Helper methods for execute
  ///@{
  void ConfigurePID();
  void WriteDedxOfSelectedParticles();
  void SetFitNTuple(KKFitResult* fitresults, NTupleContainer& tuple);

  void FindBestKinematicFit();
  void ResetBestKalmanFit();
  void DoKinematicFitForAllCombinations();
  void DoVertexFit();
  void DoKinematicFit();
  void AddTracksToKinematicFitter();
  void ExtractFitResults();

  void WriteBestFitWithMcTruth();
  void SetAdditionalNtupleItems_topology();
  void CreateMCTruthSelection();
  ///@}
};

/// @}
#endif