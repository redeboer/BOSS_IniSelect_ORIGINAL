// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "CommonFunctions.h"
	#include "FrameworkSettings.h"
	#include "ReconstructedParticle.h"
	#include "RooAddPdf.h"
	#include "RooBreitWigner.h"
	#include "RooChebychev.h"
	#include "RooFFTConvPdf.h"
	#include "RooPlot.h"
	#include <iostream>
	using namespace CommonFunctions;



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


	/// Create a `RooRealVar` variable for resonstructing a certain particle
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


	std::shared_ptr<RooRealVar> ReconstructedParticle::RooRealVarMeanZero() {
		auto ptr = std::make_shared<RooRealVar>("GaussianMeanZero", "GaussianMeanZero", 0.);
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


	std::shared_ptr<RooRealVar> ReconstructedParticle::RooRealVarSigmaFixed(UChar_t num) {
		auto ptr = std::make_shared<RooRealVar>(
			Form("#sigma_{%u}", num+1),
			Form("%s width %u", NameLaTeX(), num+1),
			fGaussianWidths[num]);
		fRooRealVars.push_back(ptr);
		return ptr;
	}


	/// Fit the sum of two Gaussian functions on a invariant mass distrubution. The mean of the two Gaussian is in both cases taken to be the mass of the particle to be reconstructed.
		/// For a pure particle signal, that is, without backround @b and without a physical particle width, the width of the two Gaussians characterises the resolution of the detector.
		/// See https://root.cern.ch/roofit-20-minutes for an instructive tutorial.
		/// @param hist Invariant mass histogram that you would like to fit
		/// @param particle Hypothesis particle: which particle are you reconstructing? All analysis parameters, such as estimates for Gaussian widths, are contained within this object.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	std::vector<std::shared_ptr<RooRealVar> > ReconstructedParticle::FitConvolutionBWGaussian(TH1F *hist, TString logScale)
	{
		// * DATA MEMBERS * //
			// * FitObject
				std::shared_ptr<RooDataHist> fRooDataHist;
				std::shared_ptr<RooRealVar>  fRooRealVar;
			// * FitObjectDoubleGaussian
				RooArgList fBckParameters;
				RooArgList fComponents;
				RooArgList fAddedGaussians;
				RooArgList fNContributions;
				std::shared_ptr<RooAddPdf>    fFullShape;
				std::shared_ptr<RooChebychev> fPolBackground;
				std::shared_ptr<RooRealVar>   fSigToBckRatio;
				std::vector<std::shared_ptr<RooRealVar> >  fFitPars;
				std::vector<std::shared_ptr<RooGaussian> > fGaussians;
				std::vector<std::shared_ptr<RooRealVar> >  fNGauss;

		if(TerminalIO::IsEmptyPtr(hist)) return fFitPars;
		// * METHODS * //
		if(true) { // local namespace for testing the beginnings of a class design
			// * Create RooFit variable and data distribution * //
				fRooRealVar = RooRealVarInvMass();
				fRooDataHist = std::make_shared<RooDataHist>(CreateRooFitInvMassDistr(hist, *fRooRealVar));

			// * Create Gaussian function * //
				fFitPars.push_back(RooRealVarMeanZero());
				for(UChar_t i = 0; i < GaussianWidths().size()-1; ++i) {
					fFitPars.push_back(RooRealVarSigma(i));
					fGaussians.push_back(std::make_shared<RooGaussian>(
						Form("gauss%u", i+1),
						Form("Gaussian PDF %u for #it{M}_{%s} distribution", i+1, DaughterLabel()),
						*fRooRealVar, *fFitPars[0], *fFitPars[1+i]));
					fNGauss.push_back(std::make_shared<RooRealVar>(
						Form("N_{gaus%u}", i+1),
						Form("N_{gaus%u}", i+1),
						.8, 0., 1.));
					fAddedGaussians.add(*fGaussians[i]);
					if(i!=0) fNContributions.add(*fNGauss[i]);
				}

			// * Add the Gaussian components * //
				RooRealVar ratio("ratio", "Ratio between the two Gaussian pdfs", .8, 0., 1.);
				RooAddPdf doublegauss("double_gaussian", "Double gaussian",
					fAddedGaussians, fNContributions);

			// * Create Breit-Wigner/Cauchy distribution * //
				fFitPars.push_back(RooRealVarMean());
				RooRealVar width("#sigma_{BW}", Form("%s BW width", NameLaTeX()),
					GaussianWidths().back(), 0., 10.*GaussianWidths().back());
				RooBreitWigner bw("breitwigner",
					Form("Breit-Wigner PDF for #it{M}_{%s} distribution", DaughterLabel()),
					*fRooRealVar, *fFitPars.back(), width);

			// * Convolute * //
				RooFFTConvPdf signal("convolution", "convolution", *fRooRealVar, bw, doublegauss);
				RooRealVar n("N_{gaus1}", "N_{gaus1}", 1e2, 0., 1e6);
				RooArgList fComponents(signal);
				RooArgList fNContributions(n);

			// * Add polynomial background if required * //
				RooArgList fBckParameters;
				for(UChar_t i = 0; i <= NPol(); ++i) {
					auto p = new RooRealVar(Form("p%u", i), Form("p%u", i), 0., -1e6, 1e6);
					fBckParameters.add(*p);
				}
				RooChebychev fPolBackground("polBkg",
					Form("Polynomial-%u background", NPol()),
					*fRooRealVar, fBckParameters);
				RooRealVar fSigToBckRatio(
					Form("N_{pol%u}", NPol()),
					Form("N_{pol%u}", NPol()),
					0., 0., 1e5);
				if(NPol()) {
					fComponents.add(fPolBackground);
					fNContributions.add(fSigToBckRatio);
				}

			// * Add the components and fit * //
				RooAddPdf fFullShape("full_shape", "Double gaussian + background", fComponents, fNContributions);
				auto result = fFullShape.fitTo(
					*fRooDataHist,
					RooFit::Range(FitFrom(), FitUntil()));

			// * Plot results and save * //
				RooPlot *frame = fRooRealVar->frame(); // create a frame to draw
				frame->SetAxisRange(PlotFrom(), PlotUntil());
				fRooDataHist->plotOn(frame, // draw distribution
					RooFit::LineWidth(2), RooFit::LineColor(kBlue+2), RooFit::LineWidth(1),
					RooFit::MarkerColor(kBlue+2), RooFit::MarkerSize(.5));
				fFullShape.plotOn(frame, RooFit::LineWidth(2), RooFit::LineColor(kBlack));
				if(NPol()) {
					fFullShape.plotOn(frame, RooFit::Components(signal), // draw signal
						RooFit::LineWidth(1), RooFit::LineColor(kRed-4));
					fFullShape.plotOn(frame, RooFit::Components(fPolBackground), // draw background
						RooFit::LineStyle(kDashed), RooFit::LineWidth(1), RooFit::LineColor(kGray));
				}
				fFullShape.paramOn(frame, RooFit::Layout(.56, .98, .92));

			// * Write fitted histograms * //
				TString pname = Name();
				pname.ReplaceAll("/", ""); // in case of for instance "J/psi"
				Draw::DrawAndSave(Form("ConvBWDoubleGauss_%s", pname.Data()), "", logScale, frame);
				delete frame;

		}
		return fFitPars;
	}


	/// Fit the sum of two Gaussian functions on a invariant mass distrubution. The mean of the two Gaussian is in both cases taken to be the mass of the particle to be reconstructed.
		/// For a pure particle signal, that is, without backround @b and without a physical particle width, the width of the two Gaussians characterises the resolution of the detector.
		/// See https://root.cern.ch/roofit-20-minutes for an instructive tutorial.
		/// @param hist Invariant mass histogram that you would like to fit
		/// @param particle Hypothesis particle: which particle are you reconstructing? All analysis parameters, such as estimates for Gaussian widths, are contained within this object.
	std::vector<std::shared_ptr<RooRealVar> > ReconstructedParticle::FitBreitWigner(TH1F *hist, TString logScale)
	{
		// * DATA MEMBERS * //
			// * FitObject
				std::shared_ptr<RooDataHist> fRooDataHist;
				std::shared_ptr<RooRealVar>  fRooRealVar;
			// * FitObjectDoubleGaussian
				RooArgList fBckParameters;
				RooArgList fComponents;
				RooArgList fNContributions;
				std::shared_ptr<RooAddPdf>    fFullShape;
				std::shared_ptr<RooChebychev> fPolBackground;
				std::shared_ptr<RooRealVar>   fSigToBckRatio;
				std::vector<std::shared_ptr<RooRealVar> >  fFitPars;
				std::vector<std::shared_ptr<RooGaussian> > fGaussians;
				std::vector<std::shared_ptr<RooRealVar> >  fNGauss;

		if(TerminalIO::IsEmptyPtr(hist)) return fFitPars;
		// * METHODS * //
		if(true) { // local namespace for testing the beginnings of a class design
			// * Create RooFit variable and data distribution * //
				auto fRooRealVar = RooRealVarInvMass();
				auto fRooDataHist = CreateRooFitInvMassDistr(hist, *fRooRealVar);

			// * Create Breit-Wigner function and fit * //
				RooRealVar mean(
					Form("m_{%s}", NameLaTeX()),
					Form("%s mass", NameLaTeX()),
					Mass(), LowerMass(), UpperMass());
				RooRealVar width("width", Form("%s width", NameLaTeX()),
					BWPureWidth(), 0., 100.*BWPureWidth());
				RooGaussian signal("breitwigner",
					Form("Breit-Wigner PDF for #it{M}_{%s} distribution", DaughterLabel()),
					*fRooRealVar, mean, width);
				RooRealVar n("N_{BW}", "N_{BW}", 1e2, 0., 1e6);
				RooArgList fComponents(signal);
				RooArgList fNContributions(n);

			// * Add polynomial background if required * //
				RooArgList fBckParameters;
				for(UChar_t i = 0; i <= NPol(); ++i) {
					auto p = new RooRealVar(Form("p%u", i), Form("p%u", i), 0., -1e6, 1e6);
					fBckParameters.add(*p);
				}
				RooChebychev fPolBackground("polBkg",
					Form("Polynomial-%u background", NPol()),
					*fRooRealVar, fBckParameters);
				RooRealVar fSigToBckRatio(
					Form("N_{pol%u}", NPol()),
					Form("N_{pol%u}", NPol()),
					0., 0., 1e5);
				if(NPol()) {
					fComponents.add(fPolBackground);
					fNContributions.add(fSigToBckRatio);
				}

			// * Add the components and fit * //
				RooAddPdf fFullShape("full_shape", "Breit-Wigner + background", fComponents, fNContributions);
				auto result = fFullShape.fitTo(
					fRooDataHist,
					RooFit::Range(FitFrom(), FitUntil()));

			// * Plot results and save * //
				RooPlot *frame = fRooRealVar->frame(); // create a frame to draw
				frame->SetAxisRange(PlotFrom(), PlotUntil());
				fRooDataHist.plotOn(frame, // draw distribution
					RooFit::LineWidth(2), RooFit::LineColor(kBlue+2), RooFit::LineWidth(1),
					RooFit::MarkerColor(kBlue+2), RooFit::MarkerSize(.5));
				fFullShape.plotOn(frame, RooFit::LineWidth(2), RooFit::LineColor(kBlack));
				if(NPol()) {
					fFullShape.plotOn(frame, RooFit::Components(signal), // draw Breit-Wigner
						RooFit::LineWidth(1), RooFit::LineColor(kRed-4));
					fFullShape.plotOn(frame, RooFit::Components(fPolBackground), // draw background
						RooFit::LineStyle(kDashed), RooFit::LineWidth(1), RooFit::LineColor(kGray));
				}
				fFullShape.paramOn(frame, RooFit::Layout(.56, .98, .92));

			// * Write fitted histograms * //
				TCanvas c;
				c.SetBatch();
				frame->Draw();
				Draw::SaveCanvas(Form("BreitWigner_%s", Name()), &c);
		}
		return fFitPars;
	}


	/// Fit the sum of two Gaussian functions on a invariant mass distrubution. The mean of the two Gaussian is in both cases taken to be the mass of the particle to be reconstructed.
		/// For a pure particle signal, that is, without backround @b and without a physical particle width, the width of the two Gaussians characterises the resolution of the detector.
		/// See https://root.cern.ch/roofit-20-minutes for an instructive tutorial.
		/// @param hist Invariant mass histogram that you would like to fit
		/// @param particle Hypothesis particle: which particle are you reconstructing? All analysis parameters, such as estimates for Gaussian widths, are contained within this object.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	std::vector<std::shared_ptr<RooRealVar> > ReconstructedParticle::FitPureGaussians(TH1F *hist, TString logScale, TString outputName, const bool fixpars)
	{
		// * DATA MEMBERS * //
			// * FitObject
				std::shared_ptr<RooDataHist> fRooDataHist;
				std::shared_ptr<RooRealVar>  fRooRealVar;
			// * FitObjectDoubleGaussian
				RooArgList fBckParameters;
				RooArgList fComponents;
				RooArgList fNContributions;
				std::shared_ptr<RooAddPdf>    fFullShape;
				std::shared_ptr<RooChebychev> fPolBackground;
				std::shared_ptr<RooRealVar>   fSigToBckRatio;
				std::vector<std::shared_ptr<RooRealVar> >  fFitPars;
				std::vector<std::shared_ptr<RooGaussian> > fGaussians;
				std::vector<std::shared_ptr<RooRealVar> >  fNGauss;

		if(TerminalIO::IsEmptyPtr(hist)) return fFitPars;
		// * METHODS * //
		if(true) { // local namespace for testing the beginnings of a class design
			// * Create RooFit variable and data distribution * //
				fRooRealVar  = RooRealVarInvMass();
				fRooDataHist = std::make_shared<RooDataHist>(CreateRooFitInvMassDistr(hist, *fRooRealVar));

			// * Create Gaussian functions * //
				fFitPars.push_back(RooRealVarMean());
				for(UChar_t i = 0; i < GaussianWidths().size(); ++i) {
					if(fixpars) fFitPars.push_back(RooRealVarSigmaFixed(i));
					else        fFitPars.push_back(RooRealVarSigma(i));
					fGaussians.push_back(std::make_shared<RooGaussian>(
						Form("gauss%u", i+1),
						Form("Gaussian PDF %u for #it{M}_{%s} distribution", i+1, DaughterLabel()),
						*fRooRealVar, *fFitPars[0], *fFitPars[1+i]));
					fNGauss.push_back(std::make_shared<RooRealVar>(
						Form("N_{gaus%u}", i+1),
						Form("N_{gaus%u}", i+1),
						1e2, 0., 1e6));
					fComponents.add(*fGaussians[i]);
					fNContributions.add(*fNGauss[i]);
				}

			// * Add polynomial background if required * //
				for(UChar_t i = 0; i <= NPol(); ++i) {
					fBckParameters.addClone(RooRealVar(Form("p%u", i), Form("p%u", i), 0., -1e6, 1e6));
				}
				fPolBackground = std::make_shared<RooChebychev>("polBkg",
					Form("Polynomial-%u background", NPol()),
					*fRooRealVar, fBckParameters);
				fSigToBckRatio = std::make_shared<RooRealVar>(
					Form("N_{pol%u}", NPol()),
					Form("N_{pol%u}", NPol()),
					0., 0., 1e5);
				if(NPol()) {
					fComponents.add(*fPolBackground);
					fNContributions.add(*fSigToBckRatio);
				}

			// * Add the components and fit * //
				fFullShape = std::make_shared<RooAddPdf>("full_shape", "Double gaussian + background", fComponents, fNContributions);
				fFullShape->fitTo(
					*fRooDataHist,
					RooFit::Range(FitFrom(), FitUntil()));


			// * Plot results and save * //
				auto frame = fRooRealVar->frame(); // create a frame to draw
				frame->SetAxisRange(PlotFrom(), PlotUntil());
				fRooDataHist->plotOn(frame, // draw distribution
					RooFit::LineWidth(2), RooFit::LineColor(kBlue+2), RooFit::LineWidth(1),
					RooFit::MarkerColor(kBlue+2), RooFit::MarkerSize(.5));
				fFullShape->plotOn(frame, RooFit::LineWidth(2), RooFit::LineColor(kBlack)); // draw sig+bck fit
				for(UChar_t i = 0; i < GaussianWidths().size(); ++i) {
					fFullShape->plotOn(frame, RooFit::Components(*fComponents.at(i)),
						RooFit::LineWidth(1), RooFit::LineColor(Settings::Draw::gColorPalette[i]));
				}
				if(NPol()) {
					fFullShape->plotOn(frame, RooFit::Components(*fPolBackground), // draw background
						RooFit::LineStyle(kDashed), RooFit::LineWidth(1), RooFit::LineColor(kGray));
				}
				fFullShape->paramOn(frame, RooFit::Layout(.56, .98, .92));

			// * Write fitted histograms * //
				if(outputName.EqualTo("")) outputName = Name();
				outputName.ReplaceAll("/", ""); // in case of for instance "J/psi"
				Draw::DrawAndSave(Form("DoubleGauss_%s", outputName.Data()), "", logScale, frame);
				delete frame;
		}
		return fFitPars;
	}


	/// Create a `RooDataHist` specifically for resonstructing a certain particle (`ReconstructedParticle`).
	RooDataHist ReconstructedParticle::CreateRooFitInvMassDistr(TH1F *hist, const RooRealVar &var) {
		RooDataHist distr(
			Form("%scandidate_RooDataHist", Name()),
			hist->GetTitle(), var, RooFit::Import(*hist));
		return distr;
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
					fGaussianWidths      = {.00499, .0135};
					fFitRange            = {.10, .17};
					fPlotRange           = {.10, .17};
					fNPol                = 2;
					break;
				case 113: // neutral rho
					fMassOffset          = .05;
					fBWPureWidth         = .8;
					fBWConvolutedWidth   = .001;
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
					fGaussianWidths      = {.0542, .209};
					fFitRange            = {.50, 1.42};
					fPlotRange           = {.30, 1.7};
					fNPol                = 2;
					break;
				case 421: // D0 meson
					fMassOffset          = .05;
					fBWPureWidth         = .8;    //! not yet optimised
					fBWConvolutedWidth   = .0004; //! not yet optimised
					fGaussianWidths      = {.0542, .15, .5};
					fFitRange            = {1.84, 1.91};
					fPlotRange           = {1.83, 1.94};
					fNPol                = 0;
					break;
				case 333: // phi meson
					fMassOffset          = .05;
					fBWPureWidth         = .8;    //! not yet optimised
					fBWConvolutedWidth   = .0004; //! not yet optimised
					fGaussianWidths      = {.002, .006};
					fFitRange            = {.99, 1.053};
					fPlotRange           = {.99, 1.18};
					fNPol                = 2;
					break;
				case 443: // J/psi meson
					fMassOffset          = .05;
					fBWPureWidth         = .8;
					fBWConvolutedWidth   = .0004;
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