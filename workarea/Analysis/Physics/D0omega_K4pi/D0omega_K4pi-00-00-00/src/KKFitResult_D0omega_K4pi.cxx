// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "D0omega_K4pi/KKFitResult_D0omega_K4pi.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "TrackSelector/TSGlobals.h"
#include "TrackSelector/TrackSelector.h"
#include <cmath>

using CLHEP::HepLorentzVector;
using namespace TSGlobals;

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
  SetValues(kkmfit->pfit(0), /// -# \f$K^-\f$
            kkmfit->pfit(1), /// -# \f$\gamma\f$ (first occurrence)
            kkmfit->pfit(2), /// -# \f$\gamma\f$ (second occurrence)
            kkmfit->pfit(3), /// -# \f$\pi^-\f$
            kkmfit->pfit(4), /// -# \f$\pi^+\f$  (first occurrence)
            kkmfit->pfit(5)  /// -# \f$\pi^+\f$  (second occurrence)
  );
}

/// Construct a `KKFitResult_D0omega_K4pi` object based on a pointer to a `KalmanKinematicFit`
/// object.
KKFitResult_D0omega_K4pi::KKFitResult_D0omega_K4pi(
  Event::McParticle* kaonNeg, Event::McParticle* photon1, Event::McParticle* photon2,
  Event::McParticle* pionNeg, Event::McParticle* pionPos1, Event::McParticle* pionPos2) :
  KKFitResult(nullptr)
{
  SetValues(kaonNeg, photon1, photon2, pionNeg, pionPos1, pionPos2);
}

// * ============================ * //
// * ------- KKFITRESULTS ------- * //
// * ============================ * //

/// Helper function for the constructor (hence `private` method).
void KKFitResult_D0omega_K4pi::SetValues(Event::McParticle* kaonNeg, Event::McParticle* photon1,
                                         Event::McParticle* photon2, Event::McParticle* pionNeg,
                                         Event::McParticle* pionPos1, Event::McParticle* pionPos2)
{
  /// <ol>
  /// <li> Test whether all `Event::McParticle` pointers exist.
  if(!kaonNeg) return;
  if(!photon1) return;
  if(!photon2) return;
  if(!pionNeg) return;
  if(!pionPos1) return;
  if(!pionPos2) return;
  /// <li> Apply `SetValues` to the `initialFourMomentum` of these `Event::McParticle` pointers.
  SetValues(kaonNeg->initialFourMomentum(), photon1->initialFourMomentum(),
            photon2->initialFourMomentum(), pionNeg->initialFourMomentum(),
            pionPos1->initialFourMomentum(), pionPos2->initialFourMomentum());
  /// </ol>
}

/// Helper function for the constructor (hence `private` method).
void KKFitResult_D0omega_K4pi::SetValues(const HepLorentzVector& pKaonNeg,
                                         const HepLorentzVector& pPhoton1,
                                         const HepLorentzVector& pPhoton2,
                                         const HepLorentzVector& pPionNeg,
                                         const HepLorentzVector& pPionPos1,
                                         const HepLorentzVector& pPionPos2)
{
  /// <ol>
  /// <li> Compute Lorentz vectors of the particles to be reconstructed:
  /// <ul>
  HepLorentzVector ppi0   = pPhoton1 + pPhoton2;  /// <li> \f$pi^0 \rightarrow \gamma\gamma\f$
  HepLorentzVector pD0    = pKaonNeg + pPionPos1; /// <li> \f$D^0 \rightarrow K^-\pi^+\pi^0\f$
  HepLorentzVector pomega = pPionNeg + pPionPos2 + ppi0; /// <li> \f$\omega \rightarrow K^-K^+\f$
  HepLorentzVector pJpsi  = pD0 + pomega;                /// <li> \f$J/\psi \rightarrow D^0\omega\f$
  /// </ul>
  /// <li> Compute invariant masses and momentum:
  /// <ul>
  fM_pi0   = ppi0.m();   /// <li> `fM_pi0`  = \f$M_{\gamma\gamma}\f$
  fM_D0    = pD0.m();    /// <li> `fM_D0`   = \f$M_{K^-\pi^+\pi^0}\f$
  fM_omega = pomega.m(); /// <li> `fM_omega`  = \f$M_{K^-K^+}\f$
  fM_Jpsi  = pJpsi.m();  /// <li> `fM_Jpsi` = \f$M_{D^0\omega}\f$
  /// </ul>
  /// <li> Compute absolute 3-momenta of reconstructed and original particles:
  fP_D0    = ThreeMomentum(pD0);
  fP_omega = ThreeMomentum(pomega);
  fP_pi0   = ThreeMomentum(ppi0);
  fP_Km    = ThreeMomentum(pKaonNeg);
  fP_pim   = ThreeMomentum(pPionNeg);
  fP_pip1  = ThreeMomentum(pPionPos1);
  fP_pip2  = ThreeMomentum(pPionPos2);
  /// <li> Compute Dalitz variables for omega.
  fDalitzOmega_pimpip = (pPionNeg + pPionPos2).m();
  fDalitzOmega_pi0pim = (pPionNeg + ppi0).m();
  fDalitzOmega_pi0pip = (pPionPos2 + ppi0).m();
  std::cout << "fM_pi0   = " << fM_pi0 << std::endl;
  std::cout << "fM_D0    = " << fM_D0 << std::endl;
  std::cout << "fM_omega = " << fM_omega << std::endl;
  std::cout << "fM_Jpsi  = " << fM_Jpsi << std::endl;
  std::cout << "fP_D0    = " << fP_D0 << std::endl;
  std::cout << "fP_omega = " << fP_omega << std::endl;
  std::cout << "fP_pi0   = " << fP_pi0 << std::endl;
  std::cout << "fP_Km    = " << fP_Km << std::endl;
  std::cout << "fP_pim   = " << fP_pim << std::endl;
  std::cout << "fP_pip1  = " << fP_pip1 << std::endl;
  std::cout << "fP_pip2  = " << fP_pip2 << std::endl;
  /// <li> Info about the photons
  fRelativePhotonAngle = pPhoton1.angle(pPhoton2);
  /// <li> Compute measure for best fit: `fFitMeasure` := \f$M_{\pi^-\pi^+\pi^0} - m_{\omega}\f$
  fFitMeasure = std::abs(fM_omega - Mass::phi);
  /// <li> Set `fHasResults` to `true`.
  fHasResults = true;
  /// </ol>
}