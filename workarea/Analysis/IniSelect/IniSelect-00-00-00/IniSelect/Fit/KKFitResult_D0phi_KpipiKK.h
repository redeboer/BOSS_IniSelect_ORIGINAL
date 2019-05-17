#ifndef Physics_Analysis_KKFitResult_D0phi_KpipiKK_H
#define Physics_Analysis_KKFitResult_D0phi_KpipiKK_H

#include "IniSelect/Fit/KKFitResult.h"
#include "IniSelect/Globals.h"

/// @addtogroup BOSS_objects
/// @{
/// Derived class for a container that contains important fit results of the `KalmanKinematicFit`, including masses.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     March 29th, 2019
struct KKFitResult_D0phi_KpipiKK : public KKFitResult
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

void KKFitResult_D0phi_KpipiKK::SetValues_impl()
{
  CLHEP::HepLorentzVector Km1 = KinematicFitter::GetTrack("K-", 0);
  CLHEP::HepLorentzVector Km2 = KinematicFitter::GetTrack("K-", 1);
  CLHEP::HepLorentzVector Kp  = KinematicFitter::GetTrack("K+");
  CLHEP::HepLorentzVector pip = KinematicFitter::GetTrack("pi+");
  CLHEP::HepLorentzVector g1  = KinematicFitter::GetTrack("g", 0);
  CLHEP::HepLorentzVector g2  = KinematicFitter::GetTrack("g", 1);

  CLHEP::HepLorentzVector pi0  = g1 + g2;
  CLHEP::HepLorentzVector D0   = Km1 + pip + pi0;
  CLHEP::HepLorentzVector phi  = Km2 + Kp;
  CLHEP::HepLorentzVector Jpsi = D0 + phi;

  fM_pi0  = pi0.m();
  fM_D0   = D0.m();
  fM_phi  = phi.m();
  fM_Jpsi = Jpsi.m();
  fP_D0   = std::sqrt(D0.px() * D0.px() + D0.py() * D0.py() + D0.pz() * D0.pz());
  fP_phi  = std::sqrt(phi.px() * phi.px() + phi.py() * phi.py() + phi.pz() * phi.pz());

  fFitMeasure = std::abs(fM_phi - IniSelect:Mass::phi);
}

#endif