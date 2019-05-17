#ifndef Physics_Analysis_KKFitResult_rhopi_pipigg_H
#define Physics_Analysis_KKFitResult_rhopi_pipigg_H

#include "IniSelect/Fit/KKFitResult.h"

/// @addtogroup BOSS_objects
/// @{
/// Derived class for a container that contains important fit results of the `KalmanKinematicFit`, including masses.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     December 14th, 2018
struct KKFitResult_rhopi_pipigg : public KKFitResult
{
  void SetValues_impl();

  double fM_pi0;
  double fM_rho0;
  double fM_rhom;
  double fM_rhop;
  double fM_JpsiRho0;
  double fM_JpsiRhom;
  double fM_JpsiRhop;
};
/// @}
#endif