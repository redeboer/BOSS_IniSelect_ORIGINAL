#ifndef Physics_Analysis_D0omega_K4pi_H
#define Physics_Analysis_D0omega_K4pi_H

#include "IniSelect/Algorithms/TrackSelector.h"
#include "IniSelect/Fit/KKFitResult_D0omega_K4pi.h"

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
  NTupleContainer fNTuple_fit4c_all;
  NTupleContainer fNTuple_fit4c_best;
  NTupleContainer fNTuple_fit_mc;
  ///@}

  /// @name Fit objects
  ///@{
  KKFitResult_D0omega_K4pi fBestKalmanFit;
  KKFitResult_D0omega_K4pi fCurrentKalmanFit;
  ///@}

private:
  /// @name Helper methods for initialize
  ///@{
  void ConfigureCandidateSelection();
  void AddAdditionalNTupleItems();
  void AddNTupleItems_dedx();
  void AddNTupleItems_fit();
  void AddNTupleItems_fit(NTupleContainer& tuple);
  void AddAdditionalNTuples_topology();
  ///@}

  /// @name Helper methods for execute
  ///@{
  void ConfigurePID();
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