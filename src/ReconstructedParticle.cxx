// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "FrameworkSettings.h"
	#include "ReconstructedParticle.h"
	#include <iostream>



// * =========================================== * //
// * ------- CONSTRUCTORS AND DESTRUCTORS ------ * //
// * =========================================== * //


	/// Construct particle based on its code in the PDG.
	ReconstructedParticle::ReconstructedParticle(int pdgCode) : Particle(pdgCode)
	{
		Initialize();
	}


	/// Construct particle based on its code in the PDG.
	ReconstructedParticle::ReconstructedParticle(const char* particleName) : Particle(particleName)
	{
		Initialize();
	}


	/// Construct particle based on its code in the PDG.
	ReconstructedParticle::ReconstructedParticle(int pdgCode, const char* daughters) :
		Particle(pdgCode), fDaughterLabels(daughters)
	{
		Initialize();
	}


	/// Construct particle based on its code in the PDG. This constructor also sets the daughters.
	ReconstructedParticle::ReconstructedParticle(const char* particleName, const char* daughters) :
		Particle(particleName), fDaughterLabels(daughters)
	{
		Initialize();
	}


	/// Encapsulation of what any constructor for this object needs to do.
	void ReconstructedParticle::Initialize()
	{
		DetermineReconstructionParameters();
	}



// * ======================= * //
// * ------- SETTERS ------- * //
// * ======================= * //


	/// Set a LaTeX label for the daughters. This method is public as to allow you to modify it later. Construction cannot be automised, as it is up to you to decide which decay channel you want to analyse.
	void ReconstructedParticle::SetDaughterLabel(const char* daughters)
	{
		fDaughterLabels = daughters;
	}


	/// **Temporary** method to overwrite the members needed for the double Gaussian fit.
		/// @warning Due to the use of indices, this method cannot be made backward-compatible and is therefore not sustainable.
	void ReconstructedParticle::ImportFitPars_DG()
	{
		const std::vector<double> &sig = fFitParameters.SignalParameters();
		if(sig.size() == 3) {
			fMassOffset     = sig[0];
			fFitRange       = { sig[1], sig[2] };
		}
		const std::vector<double> &bck = fFitParameters.BackgroundParameters();
		if(bck.size() == 1) {
			fNPol = bck[0];
		}
	}


	/// **Temporary** method to import new values for the Gaussian widths.
	void ReconstructedParticle::SetGaussianPars(TString input)
	{
		if(input.EqualTo("")) return;
		fGaussianWidths.clear();
		input.ReplaceAll(",", " , ");
		TString tok;
		Ssiz_t start{0};
		while(input.Tokenize(tok, start, ","))
			fGaussianWidths.push_back(tok.Atof());
	}



// * ====================== * //
// * ------- ROOFIT ------- * //
// * ====================== * //


	/// Create a `RooRealVar` variable for resonstructing a certain particle.
	std::shared_ptr<RooRealVar> ReconstructedParticle::RooRealVarInvMass()
	{
		auto ptr = std::make_shared<RooRealVar>(
			Form("#it{M}_{%s}", DaughterLabel()),
			Form("#it{M}_{%s} (GeV/#it{c}^{2})", DaughterLabel()),
			PlotFrom(),
			PlotUntil()
		);
		fRooRealVars.push_back(ptr);
		return ptr;
	}


	std::shared_ptr<RooRealVar> ReconstructedParticle::RooRealVarMean() {
		auto ptr = std::make_shared<RooRealVar>(
			Form("m_{%s}", NameLaTeX()),
			Form("%s mass", NameLaTeX()),
			Mass(), LowerMass(), UpperMass());
		fRooRealVars.push_back(ptr);
		return ptr;
	}


	std::shared_ptr<RooRealVar> ReconstructedParticle::RooRealVarSigma(UChar_t num) {
		auto ptr = std::make_shared<RooRealVar>(
			Form("#sigma_{%u}", num+1),
			Form("%s width %u", NameLaTeX(), num+1),
			fGaussianWidths[num],
			Settings::Fit::gSigmaScaleFactorLow * fGaussianWidths[num],
			Settings::Fit::gSigmaScaleFactorUp  * fGaussianWidths[num]);
		fRooRealVars.push_back(ptr);
		return ptr;
	}



// * ======================= * //
// * ------- GETTERS ------- * //
// * ======================= * //


	/// Get (compute) the lower mass boundary. Useful for fitting parameters.
	const double ReconstructedParticle::LowerMass() const
	{
		if(fParticlePDG) return (1. - fMassOffset) * Mass();
		else return 0.;
	}


	/// Get (compute) the upper mass boundary. Useful for fitting parameters.
	const double ReconstructedParticle::UpperMass() const
	{
		if(fParticlePDG) return (1. + fMassOffset) * Mass();
		else return 0.;
	}



// * =============================== * //
// * ------- PRIVATE METHODS ------- * //
// * =============================== * //
// ! Tweak your analysis parameters here ! //


	/// Determine the wide and small sigma estimates for the double Gaussian fit. These are supposed to characterise the resolution of the deterctor.
	void ReconstructedParticle::DetermineReconstructionParameters()
	{
		if(fParticlePDG) {
			switch(fParticlePDG->PdgCode()) {
				case 111: // neutral pion
					fMassOffset          = .02;
					fBWPureWidth         = .013;
					fBWConvolutedWidth   = .00002;
					fSingleGaussianWidth = .002;
					fGaussianWidths      = {.00499, .0135};
					fFitRange            = {.10, .17};
					fPlotRange           = {.10, .17};
					fNPol                = 2;
					break;
				case 113: // neutral rho
					fMassOffset          = .05;
					fBWPureWidth         = .8;
					fBWConvolutedWidth   = .001;
					fSingleGaussianWidth = .002;
					fGaussianWidths      = {.0469, .1312};
					fFitRange            = {.40, 1.1};
					fPlotRange           = {.30, 1.7};
					fNPol                = 2;
					break;
				case 213:
				case -213: // rho meson
					fMassOffset          = .05;
					fBWPureWidth         = .8;
					fBWConvolutedWidth   = .0004;
					fSingleGaussianWidth = .002;
					fGaussianWidths      = {.0542, .209};
					fFitRange            = {.50, 1.42};
					fPlotRange           = {.30, 1.7};
					fNPol                = 2;
					break;
				case 421: // D0 meson
					fMassOffset          = .05;
					fBWPureWidth         = .8;    //! not yet optimised
					fBWConvolutedWidth   = .0004; //! not yet optimised
					fSingleGaussianWidth = .002;
					fGaussianWidths      = {.0542, .15, .5};
					fFitRange            = {1.84, 1.91};
					fPlotRange           = {1.83, 1.94};
					fNPol                = 0;
					break;
				case 333: // phi meson
					fMassOffset          = .05;
					fBWPureWidth         = .8;    //! not yet optimised
					fBWConvolutedWidth   = .0004; //! not yet optimised
					fSingleGaussianWidth = .002;
					fGaussianWidths      = {.002, .006};
					fFitRange            = {.99, 1.053};
					fPlotRange           = {.99, 1.18};
					fNPol                = 2;
					break;
				case 443: // J/psi meson
					fMassOffset          = .05;
					fBWPureWidth         = .8;
					fBWConvolutedWidth   = .0004;
					fSingleGaussianWidth = .002;
					fGaussianWidths      = {1e-8, 1e-8};
					fFitRange            = {3.096813, 3.096815};
					fPlotRange           = {3.096813, 3.096815};
					fNPol                = 2;
					break;
				default:
					std::cout << "ERROR: No particle reconstruction defined for PDG code " << fParticlePDG->PdgCode() << " (" << fParticlePDG->GetName() << ")" << std::endl;
					break;
			}
		}
	}