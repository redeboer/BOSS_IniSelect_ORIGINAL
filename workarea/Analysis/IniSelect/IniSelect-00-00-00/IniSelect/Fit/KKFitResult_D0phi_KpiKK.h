#ifndef Physics_Analysis_KKFitResult_D0phi_KpiKK_H
#define Physics_Analysis_KKFitResult_D0phi_KpiKK_H

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
/// @date     December 14th, 2018
class KKFitResult_D0phi_KpiKK : public KKFitResult
{
public:
  /// @name Constructor
  ///@{
  KKFitResult_D0phi_KpiKK() {}
  KKFitResult_D0phi_KpiKK(KalmanKinematicFit* kkmfit);
  KKFitResult_D0phi_KpiKK(Event::McParticle* kaonNeg1, Event::McParticle* kaonNeg2,
                          Event::McParticle* kaonPos, Event::McParticle* pionPos);
  ///@}

  /// @name Parameters computed from fit
  ///@{
  double fM_D0;   ///< Current computed mass of the \f$D^0\f$ candidate.
  double fM_Jpsi; ///< Current computed mass of the \f$J/\psi\f$ candidate.
  double fM_phi;  ///< Current computed mass of the \f$\phi\f$ candidate.
  double fP_D0;   ///< Current computed 3-momentum of \f$D^0\f$ candidate.
  double fP_phi;  ///< Current computed 3-momentum of \f$D^0\f$ candidate.

  double fP_Km1;
  double fP_Km2;
  double fP_Kp;
  double fP_pip;
  ///@}

private:
  /// @name Setters
  ///@{
  void SetValues(const HepLorentzVector& pKaonNeg1, const HepLorentzVector& pKaonNeg2,
                 const HepLorentzVector& pKaonPos, const HepLorentzVector& pPionPos);
  void SetValues(Event::McParticle* kaonNeg1, Event::McParticle* kaonNeg2,
                 Event::McParticle* kaonPos, Event::McParticle* pionPos);
  ///@}
};

/// @}
#endif