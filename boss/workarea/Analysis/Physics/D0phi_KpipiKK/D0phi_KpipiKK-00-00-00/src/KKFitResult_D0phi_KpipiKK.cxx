// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "CLHEP/Vector/LorentzVector.h"
	#include "TrackSelector/TrackSelector.h"
	#include "D0phi_KpipiKK/KKFitResult_D0phi_KpipiKK.h"
	#include <cmath>

	using CLHEP::HepLorentzVector;
	using namespace TSGlobals;



// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //


	/// Construct a `KKFitResult_D0phi_KpipiKK` object based on a pointer to a `KalmanKinematicFit` object.
	KKFitResult_D0phi_KpipiKK::KKFitResult_D0phi_KpipiKK(KalmanKinematicFit* kkmfit) :
		KKFitResult(kkmfit)
	{
		if(!fFit) return;
		/// Get Lorentz vectors of the decay products using `KalmanKinematicFit::pfit`:
		SetValues(
			kkmfit->pfit(0), /// -# \f$K^-\f$ (first occurrence)
			kkmfit->pfit(1), /// -# \f$K^-\f$ (second occurrence)
			kkmfit->pfit(2), /// -# \f$K^+\f$
			kkmfit->pfit(3), /// -# \f$\pi^+\f$
			kkmfit->pfit(4), /// -# \f$\gamma\f$ (first occurrence)
			kkmfit->pfit(5)  /// -# \f$\gamma\f$ (second occurrence)
		);
	}


	/// Construct a `KKFitResult_D0phi_KpipiKK` object based on a pointer to a `KalmanKinematicFit` object.
	KKFitResult_D0phi_KpipiKK::KKFitResult_D0phi_KpipiKK(
		Event::McParticle* kaonNeg1,
		Event::McParticle* kaonNeg2,
		Event::McParticle* kaonPos,
		Event::McParticle* pionPos,
		Event::McParticle* photon1,
		Event::McParticle* photon2) :
		KKFitResult(nullptr)
	{
		SetValues(kaonNeg1, kaonNeg2, kaonPos, pionPos, photon1, photon2);
	}



// * ============================ * //
// * ------- KKFITRESULTS ------- * //
// * ============================ * //


	/// Helper function for the constructor (hence `private` method).
	void KKFitResult_D0phi_KpipiKK::SetValues(
		Event::McParticle *kaonNeg1,
		Event::McParticle *kaonNeg2,
		Event::McParticle *kaonPos,
		Event::McParticle *pionPos,
		Event::McParticle *photon1,
		Event::McParticle *photon2)
	{
		/// <ol>
		/// <li> Test whether all `Event::McParticle` pointers exist.
			if(!kaonNeg1) return;
			if(!kaonNeg2) return;
			if(!kaonPos)  return;
			if(!pionPos)  return;
		/// <li> Apply `SetValues` to the `initialFourMomentum` of these `Event::McParticle` pointers.
			SetValues(
				kaonNeg1->initialFourMomentum(),
				kaonNeg2->initialFourMomentum(),
				kaonPos ->initialFourMomentum(),
				pionPos ->initialFourMomentum(),
				photon1 ->initialFourMomentum(),
				photon2 ->initialFourMomentum()
			);
		/// </ol>
	}


	/// Helper function for the constructor (hence `private` method).
	void KKFitResult_D0phi_KpipiKK::SetValues(
		const HepLorentzVector &pKaonNeg1, 
		const HepLorentzVector &pKaonNeg2, 
		const HepLorentzVector &pKaonPos, 
		const HepLorentzVector &pPionPos,
		const HepLorentzVector &pPhoton1, 
		const HepLorentzVector &pPhoton2) 
	{
		/// <ol>
		/// <li> Compute Lorentz vectors of the particles to be reconstructed:
			/// <ul>
			HepLorentzVector ppi0  = pPhoton1 + pPhoton2;         /// <li> \f$pi^0 \rightarrow \gamma\gamma\f$
			HepLorentzVector pD0   = pKaonNeg1 + pPionPos + ppi0; /// <li> \f$D^0 \rightarrow K^-\pi^+\pi^0\f$
			HepLorentzVector pphi  = pKaonNeg2 + pKaonPos;        /// <li> \f$\phi \rightarrow K^-K^+\f$
			HepLorentzVector pJpsi = pD0 + pphi;                  /// <li> \f$J/\psi \rightarrow D^0\phi\f$
			/// </ul>
		/// <li> Compute invariant masses and momentum:
			/// <ul>
			fM_pi0  = ppi0 .m(); /// <li> `fM_pi0`  = \f$M_{\gamma\gamma}\f$
			fM_D0   = pD0  .m(); /// <li> `fM_D0`   = \f$M_{K^-\pi^+\pi^0}\f$
			fM_phi  = pphi .m(); /// <li> `fM_phi`  = \f$M_{K^-K^+}\f$
			fM_Jpsi = pJpsi.m(); /// <li> `fM_Jpsi` = \f$M_{D^0\phi}\f$
			fP_D0   = std::sqrt(pD0 .px()*pD0 .px() + pD0 .py()*pD0 .py() + pD0 .pz()*pD0 .pz()); /// <li> `fP_D0`  = \f$|\vec{p}_{K^-\pi^+}|\f$
			fP_phi  = std::sqrt(pphi.px()*pphi.px() + pphi.py()*pphi.py() + pphi.pz()*pphi.pz()); /// <li> `fP_phi` = \f$|\vec{p}_{K^-K^+}|\f$
			/// </ul>
		/// <li> Compute measure for best fit: `fFitMeasure` := \f$M_{K^-K^+} - m_{\phi}\f$
			fFitMeasure = std::abs(fM_phi - gM_phi);
		/// <li> Set `fHasResults` to `true`.
			fHasResults = true;
		/// </ol>
	}