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

void KKFitResult_D0phi_3K3pi::SetValues_impl()
{
  CLHEP::HepLorentzVector pKaonNeg1 = KinematicFitter::GetTrack("K-", 0);
  CLHEP::HepLorentzVector pKaonNeg2 = KinematicFitter::GetTrack("K+", 1);
  CLHEP::HepLorentzVector pKaonPos  = KinematicFitter::GetTrack("K+");
  CLHEP::HepLorentzVector pPionNeg  = KinematicFitter::GetTrack("pi-");
  CLHEP::HepLorentzVector pPionPos1 = KinematicFitter::GetTrack("pi+", 0);
  CLHEP::HepLorentzVector pPionPos2 = KinematicFitter::GetTrack("pi+", 1);

  CLHEP::HepLorentzVector pD0   = pKaonNeg1 + pPionNeg + pPionPos1 + pPionPos2;
  CLHEP::HepLorentzVector pphi  = pKaonNeg2 + pKaonPos;
  CLHEP::HepLorentzVector pJpsi = pD0 + pphi;

  fM_D0   = pD0.m();
  fM_phi  = pphi.m();
  fM_Jpsi = pJpsi.m();
  fP_D0   = std::sqrt(pD0.px() * pD0.px() + pD0.py() * pD0.py() + pD0.pz() * pD0.pz());
  fP_phi  = std::sqrt(pphi.px() * pphi.px() + pphi.py() * pphi.py() + pphi.pz() * pphi.pz());

  fFitMeasure = std::abs(fM_phi - IniSelect:Mass::phi);
}

#endif