#ifndef Physics_Analysis_KKFitResult_D0omega_K4pi_H
#define Physics_Analysis_KKFitResult_D0omega_K4pi_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "IniSelect/Algorithms/TrackSelector.h"
#include "IniSelect/Fit/KKFitResult.h"
#include "McTruth/McParticle.h"

// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_objects
/// @{

/// Derived class for a container that contains important fit results of the `KalmanKinematicFit`,
/// including masses.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     March 29th, 2019
class KKFitResult_D0omega_K4pi : public KKFitResult
{
public:
  /// @name Constructor
  ///@{
  KKFitResult_D0omega_K4pi() {}
  KKFitResult_D0omega_K4pi(KalmanKinematicFit* kkmfit);
  KKFitResult_D0omega_K4pi(Event::McParticle* kaonNeg, Event::McParticle* photon1,
                           Event::McParticle* photon2, Event::McParticle* pionNeg,
                           Event::McParticle* pionPos1, Event::McParticle* pionPos2);
  ///@}

  /// @name Parameters computed from fit
  ///@{
  double fM_pi0;   ///< Current computed mass of the \f$\pi^0\f$ candidate.
  double fM_D0;    ///< Current computed mass of the \f$D^0\f$ candidate.
  double fM_Jpsi;  ///< Current computed mass of the \f$J/\psi\f$ candidate.
  double fM_omega; ///< Current computed mass of the \f$\omega\f$ candidate.
  double fP_D0;    ///< Current computed 3-momentum of \f$D^0\f$ candidate.
  double fP_omega; ///< Current computed 3-momentum of \f$\omega\f$ candidate.
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
  ///@}

private:
  /// @name Setters
  ///@{
  void SetValues(const HepLorentzVector& pKaonNeg, const HepLorentzVector& pPhoton1,
                 const HepLorentzVector& pPhoton2, const HepLorentzVector& pPionNeg,
                 const HepLorentzVector& pPionPos1, const HepLorentzVector& pPionPos2);
  void SetValues(Event::McParticle* kaonNeg, Event::McParticle* photon1, Event::McParticle* photon2,
                 Event::McParticle* pionNeg, Event::McParticle* pionPos1,
                 Event::McParticle* pionPos2);
  ///@}
};

/// @}
#endif