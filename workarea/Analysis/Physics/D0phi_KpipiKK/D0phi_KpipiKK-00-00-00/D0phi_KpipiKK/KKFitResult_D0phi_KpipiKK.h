#ifndef Physics_Analysis_KKFitResult_D0phi_KpipiKK_H
#define Physics_Analysis_KKFitResult_D0phi_KpipiKK_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "TrackSelector/KKFitResult.h"
#include "TrackSelector/TrackSelector.h"
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
class KKFitResult_D0phi_KpipiKK : public KKFitResult
{
public:
  /// @name Constructor
  ///@{
  KKFitResult_D0phi_KpipiKK() {}
  KKFitResult_D0phi_KpipiKK(KalmanKinematicFit* kkmfit);
  KKFitResult_D0phi_KpipiKK(Event::McParticle* kaonNeg1, Event::McParticle* kaonNeg2,
                            Event::McParticle* kaonPos, Event::McParticle* pionPos,
                            Event::McParticle* photon1, Event::McParticle* photon2);
  ///@}

  /// @name Parameters computed from fit
  ///@{
  double fM_pi0;  ///< Current computed mass of the \f$\pi^0\f$ candidate.
  double fM_D0;   ///< Current computed mass of the \f$D^0\f$ candidate.
  double fM_Jpsi; ///< Current computed mass of the \f$J/\psi\f$ candidate.
  double fM_phi;  ///< Current computed mass of the \f$\phi\f$ candidate.
  double fP_D0;   ///< Current computed 3-momentum of \f$D^0\f$ candidate.
  double fP_phi;  ///< Current computed 3-momentum of \f$D^0\f$ candidate.
  ///@}

private:
  /// @name Setters
  ///@{
  void SetValues(const HepLorentzVector& pKaonNeg1, const HepLorentzVector& pKaonNeg2,
                 const HepLorentzVector& pKaonPos, const HepLorentzVector& pPionPos,
                 const HepLorentzVector& pPhoton1, const HepLorentzVector& pPhoton2);
  void SetValues(Event::McParticle* kaonNeg1, Event::McParticle* kaonNeg2,
                 Event::McParticle* kaonPos, Event::McParticle* pionPos, Event::McParticle* photon1,
                 Event::McParticle* photon2);
  ///@}
};

/// @}
#endif