#ifndef Physics_Analysis_KKFitResult_D0phi_3K3pi_H
#define Physics_Analysis_KKFitResult_D0phi_3K3pi_H

#include "IniSelect/Fit/KKFitResult.h"
#include "IniSelect/Globals.h"

/// @addtogroup BOSS_objects
/// @{
/// Derived class for a container that contains important fit results of the `KalmanKinematicFit`, including masses.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 6th, 2019
struct KKFitResult_D0phi_3K3pi : public KKFitResult
{
  void SetValues_impl();

  double fM_Jpsi;
  double fM_D0;
  double fM_phi;

  double fP_Jpsi;
  double fP_D0;
  double fP_phi;
  double fP_pi0;
};
/// @}
#endif