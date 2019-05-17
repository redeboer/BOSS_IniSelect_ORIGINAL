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

void KKFitResult_D0omega_K4pi::SetValues_impl()
{
  CLHEP::HepLorentzVector p_g1  = KinematicFitter::GetTrack("g", 0);
  CLHEP::HepLorentzVector p_g2  = KinematicFitter::GetTrack("g", 1);
  CLHEP::HepLorentzVector p_Km  = KinematicFitter::GetTrack("K-");
  CLHEP::HepLorentzVector p_pm  = KinematicFitter::GetTrack("pi-");
  CLHEP::HepLorentzVector p_pi1 = KinematicFitter::GetTrack("pi+", 0);
  CLHEP::HepLorentzVector p_pi2 = KinematicFitter::GetTrack("pi+", 1);

  CLHEP::HepLorentzVector p_pi0   = p_g1 + p_g2;
  CLHEP::HepLorentzVector p_D0    = p_Km + p_pi1;
  CLHEP::HepLorentzVector p_omega = p_pm + p_pi2 + p_pi0;
  CLHEP::HepLorentzVector p_Jpsi  = p_D0 + p_omega;

  fM_D0    = p_D0.m();
  fM_Jpsi  = p_Jpsi.m();
  fM_omega = p_omega.m();
  fM_pi0   = p_pi0.m();

  fP_D0    = ThreeMomentum(p_D0);
  fP_omega = ThreeMomentum(p_omega);
  fP_pi0   = ThreeMomentum(p_pi0);
  fP_Km    = ThreeMomentum(p_Km);
  fP_pim   = ThreeMomentum(p_pm);
  fP_pip1  = ThreeMomentum(p_pi1);
  fP_pip2  = ThreeMomentum(p_pi2);

  fDalitzOmega_pimpip = (p_pm + p_pi2).m();
  fDalitzOmega_pi0pim = (p_pm + p_pi0).m();
  fDalitzOmega_pi0pip = (p_pi2 + p_pi0).m();

  fRelativeGammaAngle = p_g1.angle(p_g2);
  fFitMeasure         = std::abs(fM_omega - IniSelect::Mass::phi);
}
#endif