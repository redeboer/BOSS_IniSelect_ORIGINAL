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

void KKFitResult_rhopi_pipigg::SetValues_impl()
{
  CLHEP::HepLorentzVector g1  = KinematicFitter::GetTrack("g", 0);
  CLHEP::HepLorentzVector g2  = KinematicFitter::GetTrack("g", 1);
  CLHEP::HepLorentzVector pim = KinematicFitter::GetTrack("pi-");
  CLHEP::HepLorentzVector pip = KinematicFitter::GetTrack("pi+");

  CLHEP::HepLorentzVector pi0      = g1 + g2;
  CLHEP::HepLorentzVector rho0     = pim + pip;
  CLHEP::HepLorentzVector rhom     = pi0 + pim;
  CLHEP::HepLorentzVector rhop     = pi0 + pip;
  CLHEP::HepLorentzVector JpsiRho0 = rho0 + pi0;
  CLHEP::HepLorentzVector JpsiRhom = rhom + pip;
  CLHEP::HepLorentzVector JpsiRhop = rhop + pim;

  fM_pi0      = pi0.m();
  fM_rho0     = rho0.m();
  fM_rhom     = rhom.m();
  fM_rhop     = rhop.m();
  fM_JpsiRho0 = JpsiRho0.m();
  fM_JpsiRhom = JpsiRhom.m();
  fM_JpsiRhop = JpsiRhop.m();

  fFitMeasure = fChiSquared;
}

#endif