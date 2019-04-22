// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "IniSelect/Fit/KKFitResult_D0omega_K4pi.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "IniSelect/Algorithms/TrackSelector.h"
#include "IniSelect/Globals.h"
#include <cmath>

using CLHEP::HepLorentzVector;
using namespace IniSelect;

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //

/// Construct a `KKFitResult_D0omega_K4pi` object based on a pointer to a `KalmanKinematicFit`
/// object.
KKFitResult_D0omega_K4pi::KKFitResult_D0omega_K4pi(KalmanKinematicFit* kkmfit) : KKFitResult(kkmfit)
{
  if(!fFit)
  {
    std::cout << std::endl << "empty fit!" << std::endl << std::endl;
    return;
  }
  /// Get Lorentz vectors of the decay products using `KalmanKinematicFit::pfit`:
  SetValues(kkmfit->pfit(0), /// -# \f$\gamma\f$ (first occurrence)
            kkmfit->pfit(1), /// -# \f$\gamma\f$ (second occurrence)
            kkmfit->pfit(2), /// -# \f$K^-\f$
            kkmfit->pfit(3), /// -# \f$\pi^-\f$
            kkmfit->pfit(4), /// -# \f$\pi^+\f$  (first occurrence)
            kkmfit->pfit(5)  /// -# \f$\pi^+\f$  (second occurrence)
  );
}

/// Construct a `KKFitResult_D0omega_K4pi` object based on a pointer to a `KalmanKinematicFit`
/// object.
KKFitResult_D0omega_K4pi::KKFitResult_D0omega_K4pi(
  Event::McParticle* gamma1, Event::McParticle* gamma2, Event::McParticle* kaonNeg,
  Event::McParticle* pionNeg, Event::McParticle* pionPos1, Event::McParticle* pionPos2) :
  KKFitResult(nullptr)
{
  SetValues(gamma1, gamma2, kaonNeg, pionNeg, pionPos1, pionPos2);
}

// * ============================ * //
// * ------- KKFITRESULTS ------- * //
// * ============================ * //

/// Helper function for the constructor (hence `private` method).
void KKFitResult_D0omega_K4pi::SetValues(Event::McParticle* gamma1, Event::McParticle* gamma2,
                                         Event::McParticle* kaonNeg, Event::McParticle* pionNeg,
                                         Event::McParticle* pionPos1, Event::McParticle* pionPos2)
{
  /// <ol>
  /// <li> Test whether all `Event::McParticle` pointers exist.
  if(!gamma1) return;
  if(!gamma2) return;
  if(!kaonNeg) return;
  if(!pionNeg) return;
  if(!pionPos1) return;
  if(!pionPos2) return;
  /// <li> Apply `SetValues` to the `initialFourMomentum` of these `Event::McParticle` pointers.
  SetValues(gamma1->initialFourMomentum(), gamma2->initialFourMomentum(),
            kaonNeg->initialFourMomentum(), pionNeg->initialFourMomentum(),
            pionPos1->initialFourMomentum(), pionPos2->initialFourMomentum());
  /// </ol>
}

/// Helper function for the constructor (hence `private` method).
void KKFitResult_D0omega_K4pi::SetValues(const HepLorentzVector& p_g1, const HepLorentzVector& p_g2,
                                         const HepLorentzVector& p_Km, const HepLorentzVector& p_pm,
                                         const HepLorentzVector& p_pi1,
                                         const HepLorentzVector& p_pi2)
{
  /// <ol>
  /// <li> Compute Lorentz vectors of the particles to be reconstructed:
  /// <ul>
  HepLorentzVector p_pi0   = p_g1 + p_g2;          /// <li> \f$pi^0 \rightarrow \gamma\gamma\f$
  HepLorentzVector p_D0    = p_Km + p_pi1;         /// <li> \f$D^0 \rightarrow K^-\pi^+\pi^0\f$
  HepLorentzVector p_omega = p_pm + p_pi2 + p_pi0; /// <li> \f$\omega \rightarrow K^-K^+\f$
  HepLorentzVector p_Jpsi  = p_D0 + p_omega;       /// <li> \f$J/\psi \rightarrow D^0\omega\f$
  /// </ul>
  /// <li> Compute invariant masses and momentum:
  /// <ul>
  fM_pi0   = p_pi0.m();   /// <li> `fM_pi0`  = \f$M_{\gamma\gamma}\f$
  fM_D0    = p_D0.m();    /// <li> `fM_D0`   = \f$M_{K^-\pi^+\pi^0}\f$
  fM_omega = p_omega.m(); /// <li> `fM_omega`  = \f$M_{K^-K^+}\f$
  fM_Jpsi  = p_Jpsi.m();  /// <li> `fM_Jpsi` = \f$M_{D^0\omega}\f$
  /// </ul>
  /// <li> Compute absolute 3-momenta of reconstructed and original particles:
  fP_D0    = ThreeMomentum(p_D0);
  fP_omega = ThreeMomentum(p_omega);
  fP_pi0   = ThreeMomentum(p_pi0);
  fP_Km    = ThreeMomentum(p_Km);
  fP_pim   = ThreeMomentum(p_pm);
  fP_pip1  = ThreeMomentum(p_pi1);
  fP_pip2  = ThreeMomentum(p_pi2);
  /// <li> Compute Dalitz variables for omega.
  fDalitzOmega_pimpip = (p_pm + p_pi2).m();
  fDalitzOmega_pi0pim = (p_pm + p_pi0).m();
  fDalitzOmega_pi0pip = (p_pi2 + p_pi0).m();
  /// <li> Info about the gammas
  fRelativeGammaAngle = p_g1.angle(p_g2);
  /// <li> Compute measure for best fit: `fFitMeasure` := \f$M_{\pi^-\pi^+\pi^0} - m_{\omega}\f$
  fFitMeasure = std::abs(fM_omega - Mass::phi);
  /// <li> Set `fHasResults` to `true`.
  fHasResults = true;
  /// </ol>
}