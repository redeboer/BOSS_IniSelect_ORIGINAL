// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ReconstructedParticle_new.h"
	#include <iostream>



// * =========================================== * //
// * ------- CONSTRUCTORS AND DESTRUCTORS ------ * //
// * =========================================== * //


	/// Construct particle based on its code in the PDG.
	ReconstructedParticle_new::ReconstructedParticle_new(const int code, const TString &decay_chain) : Particle(code)
	{
		Initialize(decay_chain);
	}


	/// Construct particle based on its name in the PDG.
	ReconstructedParticle_new::ReconstructedParticle_new(const int code, const TString &decay_chain) : Particle(code)
	{
		Initialize();
	}



// * ======================= * //
// * ------- SETTERS ------- * //
// * ======================= * //


	/// Set a LaTeX label for the daughters. This method is public as to allow you to modify it later. Construction cannot be automised, as it is up to you to decide which decay channel you want to analyse.
	void ReconstructedParticle_new::SetDaughterLabel(const char* daughters)
	{
		fDaughterLabels = daughters;
	}



// * ======================= * //
// * ------- GETTERS ------- * //
// * ======================= * //


	/// Get (compute) the lower mass boundary. Useful for fitting parameters.
	const double ReconstructedParticle_new::LowerMass() const
	{
		if(fParticlePDG) return (1. - fMassOffset) * Mass();
		else return 0.;
	}


	/// Get (compute) the upper mass boundary. Useful for fitting parameters.
	const double ReconstructedParticle_new::UpperMass() const
	{
		if(fParticlePDG) return (1. + fMassOffset) * Mass();
		else return 0.;
	}



// * =============================== * //
// * ------- PRIVATE METHODS ------- * //
// * =============================== * //
// ! Tweak your analysis parameters here ! //


	/// Determine the wide and small sigma estimates for the double Gaussian fit. These are supposed to characterise the resolution of the deterctor.
	void ReconstructedParticle_new::DetermineReconstructionParameters()
	{
		if(fParticlePDG) {
			switch(fParticlePDG->PdgCode()) {
				case 111: // neutral pion
					fMassOffset           = .02;
					fBWPureWidth          = .013;
					fBWConvolutedWidth    = .00002;
					fSingleGaussianWidth  = .002;
					fDoubleGaussianWidths = {.00499, .0135};
					fFitRange             = {.10, .17};
					fPlotRange            = {.10, .17};
					fNPol                 = 2;
					break;
				case 113: // neutral rho
					fMassOffset           = .05;
					fBWPureWidth          = .8;
					fBWConvolutedWidth    = .001;
					fSingleGaussianWidth  = .002;
					fDoubleGaussianWidths = {.0469, .1312};
					fFitRange             = {.40, 1.1};
					fPlotRange            = {.30, 1.7};
					fNPol                 = 2;
					break;
				case 213:
				case -213: // rho meson
					fMassOffset           = .05;
					fBWPureWidth          = .8;
					fBWConvolutedWidth    = .0004;
					fSingleGaussianWidth  = .002;
					fDoubleGaussianWidths = {.0542, .209};
					fFitRange             = {.50, 1.42};
					fPlotRange            = {.30, 1.7};
					fNPol                 = 2;
					break;
				case 421: // D0 meson
					fMassOffset           = .05;
					fBWPureWidth          = .8;    //! not yet optimised
					fBWConvolutedWidth    = .0004; //! not yet optimised
					fSingleGaussianWidth  = .002;
					fDoubleGaussianWidths = {.0542, .209};
					fFitRange             = {1.84, 1.91};
					fPlotRange            = {1.83, 1.94};
					fNPol                 = 0;
					break;
				case 333: // phi meson
					fMassOffset           = .05;
					fBWPureWidth          = .8;    //! not yet optimised
					fBWConvolutedWidth    = .0004; //! not yet optimised
					fSingleGaussianWidth  = .002;
					fDoubleGaussianWidths = {.002, .006};
					fFitRange             = {.99, 1.053};
					fPlotRange            = {.99, 1.18};
					fNPol                 = 2;
					break;
				case 443: // J/psi meson
					fMassOffset           = .05;
					fBWPureWidth          = .8;
					fBWConvolutedWidth    = .0004;
					fSingleGaussianWidth  = .002;
					fDoubleGaussianWidths = {1e-8, 1e-8};
					fFitRange             = {3.096813, 3.096815};
					fPlotRange            = {3.096813, 3.096815};
					fNPol                 = 2;
					break;
				default:
					std::cout << "ERROR: No particle reconstruction defined for PDG code " << fParticlePDG->PdgCode() << " (" << fParticlePDG->GetName() << ")" << std::endl;
					break;
			}
		}
	}