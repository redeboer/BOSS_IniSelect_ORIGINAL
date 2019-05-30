#ifndef Physics_JpsiToDPV_Globals_H
#define Physics_JpsiToDPV_Globals_H

#include "CLHEP/Vector/LorentzVector.h"
#include "TMath.h"

namespace IniSelect
{
  static const double mD0    = 1.8648400;  // mass of D0
  static const double mpi0   = 0.13497700; // mass of pi0
  static const double mphi   = 1.0194550;  // mass of phi
  static const double momega = 0.78265000; // mass of omega

  static const double me  = 0.000511; // electron
  static const double mmu = 0.105658; // muon
  static const double mpi = 0.139570; // charged pion
  static const double mK  = 0.493677; // charged kaon
  static const double mp  = 0.938272; // proton

  static const double xmass[5] = {
    me,  // electron
    mmu, // muon
    mpi, // charged pion
    mK,  // charged kaon
    mp   // proton
  };
  static const double velc_mm = 299.792458; // tof path unit in mm
  static const double Ecms    = 3.097;      // center-of-mass energy

  static const double DegToRad = 180. / TMath::Pi();
  static const double fivepi   = 5 * TMath::Pi();

  static const CLHEP::HepLorentzVector ecms(0.011 * Ecms, 0, 0, Ecms);

  static const int incPid1 = 91;
  static const int incPid2 = 92;
  static const int incPid  = 443;
}
#endif