#ifndef Physics_Analysis_KKFitResult_D0omega_K4pi_H
#define Physics_Analysis_KKFitResult_D0omega_K4pi_H

#include "IniSelect/Fit/KKFitResult.h"
#include "IniSelect/Globals.h"

/// @addtogroup BOSS_objects
/// @{
/// Derived class for a container that contains important fit results of the `KalmanKinematicFit`, including masses.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     March 29th, 2019
struct KKFitResult_D0omega_K4pi : public KKFitResult
{
  void SetValues_impl();

  double fM_D0;
  double fM_Jpsi;
  double fM_omega;
  double fM_pi0;

  double fP_D0;
  double fP_omega;
  double fP_pi0;
  double fP_Km;
  double fP_pim;
  double fP_pip1;
  double fP_pip2;

  double fDalitzOmega_pimpip;
  double fDalitzOmega_pi0pim;
  double fDalitzOmega_pi0pip;
  double fRelativePhotonAngle;
  double fRelativeGammaAngle;
};
/// @}
#endif