#ifndef Physics_Analysis_KKFitResult_D0phi_KpiKK_H
#define Physics_Analysis_KKFitResult_D0phi_KpiKK_H

#include "IniSelect/Fit/KKFitResult.h"
#include "IniSelect/Globals.h"

/// @addtogroup BOSS_objects
/// @{
/// Derived class for a container that contains important fit results of the `KalmanKinematicFit`, including masses.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     December 14th, 2018
struct KKFitResult_D0phi_KpiKK : public KKFitResult
{
  void SetValues_impl();

  double fM_Jpsi;
  double fM_D0;
  double fM_phi;

  double fP_Jpsi;
  double fP_D0;
  double fP_phi;
  double fP_pi0;

  double fP_Km1;
  double fP_Km2;
  double fP_Kp;
  double fP_pip;
};
/// @}

void KKFitResult_D0phi_KpiKK::SetValues_impl()
{
  CLHEP::HepLorentzVector& Km1 = KinematicFitter::GetTrack("K-", 0);
  CLHEP::HepLorentzVector& Km2 = KinematicFitter::GetTrack("K-", 1);
  CLHEP::HepLorentzVector& Kp  = KinematicFitter::GetTrack("K+");
  CLHEP::HepLorentzVector& pip = KinematicFitter::GetTrack("pi+");

  CLHEP::HepLorentzVector pD0   = Km1 + pip;
  CLHEP::HepLorentzVector pphi  = Km2 + Kp;
  CLHEP::HepLorentzVector pJpsi = pD0 + pphi;

  fM_D0   = pD0.m();
  fM_phi  = pphi.m();
  fM_Jpsi = pJpsi.m();

  fP_D0  = ThreeMomentum(pD0);
  fP_phi = ThreeMomentum(pphi);
  fP_Km1 = ThreeMomentum(Km1);
  fP_Km2 = ThreeMomentum(Km2);
  fP_Kp  = ThreeMomentum(Kp);
  fP_pip = ThreeMomentum(pip);

  fFitMeasure = std::abs(fM_phi - IniSelect:Mass::phi);
}

#endif