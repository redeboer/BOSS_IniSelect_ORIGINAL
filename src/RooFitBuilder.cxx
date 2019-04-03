// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "CommonFunctions.h"
	#include "FrameworkSettings.h"
	#include "RooFitBuilder.h"
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


	std::list<std::shared_ptr<RooRealVar> > RooFitBuilder::fAllParameters;



// * ======================= * //
// * ------- SETTERS ------- * //
// * ======================= * //


	void RooFitBuilder::ImportDistribution(TH1F *hist, const char *id, const char *name, const char *title, const double from, const double to)
	{
		if(TerminalIO::IsEmptyPtr(hist)) return;
		fVariable = std::make_shared<RooRealVar>(name, title, from, to);
		fDistribution = std::make_shared<RooDataHist>(id, hist->GetTitle(), *fVariable, RooFit::Import(*hist));
	}


	void RooFitBuilder::ImportDistribution(TH1F *hist, const ReconstructedParticle &particle)
	{
		ImportDistribution(hist,
			Form("%scandidate_RooDataHist", particle.Name()),
			Form("#it{M}_{%s}", particle.DaughterLabel()),
			Form("#it{M}_{%s} (GeV/#it{c}^{2})", particle.DaughterLabel()),
			particle.PlotFrom(),
			particle.PlotUntil());
	}



// * ======================== * //
// * ------- BUILDERS ------- * //
// * ======================== * //


	std::shared_ptr<RooRealVar> RooFitBuilder::BuildParameter(const char* name, const char* title, Double_t value, const char* unit) {
		auto ptr = std::make_shared<RooRealVar>(name, title, value, unit);
		fAllParameters.push_back(ptr);
		fParameters   .push_back(ptr);
		return ptr;
	}


	std::shared_ptr<RooRealVar> RooFitBuilder::BuildParameter(const char* name, const char* title, Double_t value, Double_t minValue, Double_t maxValue, const char* unit) {
		auto ptr = BuildParameter(name, title, value, unit);
		ptr->setRange(minValue, maxValue);
		return ptr;
	}


	std::shared_ptr<RooRealVar> RooFitBuilder::BuildMean(const ReconstructedParticle& particle) {
		auto ptr = BuildParameter(
			Form("m_{%s}",  particle.NameLaTeX()),
			Form("%s mass", particle.NameLaTeX()),
			particle.Mass(), particle.LowerMass(), particle.UpperMass(),
			Settings::Detector::gUnitGeV
		);
		return ptr;
	}


	std::shared_ptr<RooRealVar> RooFitBuilder::BuildSigma(const ReconstructedParticle& particle, UChar_t num, const bool fix) {
		if(num >= particle.GaussianWidths().size()) return nullptr;
		auto ptr = std::make_shared<RooRealVar>(
			Form("#sigma_{%u}", num+1),
			Form("%s width %u", particle.NameLaTeX(), num+1),
			particle.GaussianWidths()[num],
			Settings::Detector::gUnitGeV);
		if(!fix) ptr->setRange(
			Settings::Fit::gSigmaScaleFactorLow * particle.GaussianWidths()[num],
			Settings::Fit::gSigmaScaleFactorUp  * particle.GaussianWidths()[num]);
		fSigmas.push_back(ptr);
		return ptr;
	}


	std::vector<std::shared_ptr<RooRealVar> >& RooFitBuilder::BuildSigmas(const ReconstructedParticle& particle, const bool fix) {
		fSigmas.clear();
		for(UChar_t num = 0; num < particle.GaussianWidths().size(); ++num)
			BuildSigma(particle, num, fix);
		return fSigmas;
	}


	/// Fit the sum of two Gaussian functions on a invariant mass distrubution. The mean of the two Gaussian is in both cases taken to be the mass of the particle to be reconstructed.
		/// For a pure particle signal, that is, without backround @b and without a physical particle width, the width of the two Gaussians characterises the resolution of the detector.
		/// See https://root.cern.ch/roofit-20-minutes for an instructive tutorial.
		/// @param hist Invariant mass histogram that you would like to fit
		/// @param particle Hypothesis particle: which particle are you reconstructing? All analysis parameters, such as estimates for Gaussian widths, are contained within this object.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	std::vector<std::shared_ptr<RooRealVar> > RooFitBuilder::FitConvolutionBWGaussian(TH1F *hist, const ReconstructedParticle &particle, TString logScale)
	{
		if(TerminalIO::IsEmptyPtr(hist)) return fParameters;
		// * DATA MEMBERS * //
			RooArgList fBckParameters;
			RooArgList fComponents;
			RooArgList fAddedGaussians;
			RooArgList fNContributions;
			std::shared_ptr<RooChebychev> fPolBackground;
			std::shared_ptr<RooRealVar>   fSigToBckRatio;
			std::vector<std::shared_ptr<RooGaussian> > fGaussians;
			std::vector<std::shared_ptr<RooRealVar> >  fNGauss;

		// * METHODS * //
		if(true) {
			// * Create RooFit variable and data distribution * //
				ImportDistribution(hist, particle);

			// * Create Gaussian function * //
				BuildParameter("GaussianMean_0", "GaussianMean_0", 0.);
				BuildSigmas(particle, true);
				for(UChar_t i = 0; i < particle.GaussianWidths().size()-1; ++i) {
					fGaussians.push_back(std::make_shared<RooGaussian>(
						Form("gauss%u", i+1),
						Form("Gaussian PDF %u for #it{M}_{%s} distribution", i+1, particle.DaughterLabel()),
						*fVariable, *fParameters[0], *fParameters[1+i]));
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
				BuildMean(particle);
				RooRealVar width("#sigma_{BW}", Form("%s BW width", particle.NameLaTeX()),
					particle.GaussianWidths().back(), 0., 10.*particle.GaussianWidths().back());
				RooBreitWigner bw("breitwigner",
					Form("Breit-Wigner PDF for #it{M}_{%s} distribution", particle.DaughterLabel()),
					*fVariable, *fParameters.back(), width);

			// * Convolute * //
				RooFFTConvPdf signal("convolution", "convolution", *fVariable, bw, doublegauss);
				RooRealVar n("N_{gaus1}", "N_{gaus1}", 1e2, 0., 1e6);
				RooArgList fComponents(signal);
				RooArgList fNContributions(n);

			// * Add polynomial background if required * //
				RooArgList fBckParameters;
				for(UChar_t i = 0; i <= particle.NPol(); ++i) {
					auto p = new RooRealVar(Form("p%u", i), Form("p%u", i), 0., -1e6, 1e6);
					fBckParameters.add(*p);
				}
				RooChebychev fPolBackground("polBkg",
					Form("Polynomial-%u background", particle.NPol()),
					*fVariable, fBckParameters);
				RooRealVar fSigToBckRatio(
					Form("N_{pol%u}", particle.NPol()),
					Form("N_{pol%u}", particle.NPol()),
					0., 0., 1e5);
				if(particle.NPol()) {
					fComponents.add(fPolBackground);
					fNContributions.add(fSigToBckRatio);
				}

			// * Add the components and fit * //
				fFullShape = std::make_shared<RooAddPdf>("full_shape", "Double gaussian + background", fComponents, fNContributions);
				auto result = fFullShape->fitTo(
					*fDistribution,
					RooFit::Range(particle.FitFrom(), particle.FitUntil()));

			// * Plot results and save * //
				RooPlot *frame = fVariable->frame(); // create a frame to draw
				frame->SetAxisRange(particle.PlotFrom(), particle.PlotUntil());
				fDistribution->plotOn(frame, // draw distribution
					RooFit::LineWidth(2), RooFit::LineColor(kBlue+2), RooFit::LineWidth(1),
					RooFit::MarkerColor(kBlue+2), RooFit::MarkerSize(.5));
				fFullShape->plotOn(frame, RooFit::LineWidth(2), RooFit::LineColor(kBlack));
				if(particle.NPol()) {
					fFullShape->plotOn(frame, RooFit::Components(signal), // draw signal
						RooFit::LineWidth(1), RooFit::LineColor(kRed-4));
					fFullShape->plotOn(frame, RooFit::Components(fPolBackground), // draw background
						RooFit::LineStyle(kDashed), RooFit::LineWidth(1), RooFit::LineColor(kGray));
				}
				fFullShape->paramOn(frame, RooFit::Layout(.56, .98, .92));

			// * Write fitted histograms * //
				TString pname = particle.Name();
				pname.ReplaceAll("/", ""); // in case of for instance "J/psi"
				Draw::DrawAndSave(Form("ConvBWDoubleGauss_%s", pname.Data()), "", logScale, frame);
				delete frame;

		}
		return fParameters;
	}


	/// Fit the sum of two Gaussian functions on a invariant mass distrubution. The mean of the two Gaussian is in both cases taken to be the mass of the particle to be reconstructed.
		/// For a pure particle signal, that is, without backround @b and without a physical particle width, the width of the two Gaussians characterises the resolution of the detector.
		/// See https://root.cern.ch/roofit-20-minutes for an instructive tutorial.
		/// @param hist Invariant mass histogram that you would like to fit
		/// @param particle Hypothesis particle: which particle are you reconstructing? All analysis parameters, such as estimates for Gaussian widths, are contained within this object.
	std::vector<std::shared_ptr<RooRealVar> > RooFitBuilder::FitBreitWigner(TH1F *hist, const ReconstructedParticle &particle, TString logScale)
	{
		// * DATA MEMBERS * //
			RooArgList fBckParameters;
			RooArgList fComponents;
			RooArgList fNContributions;
			std::shared_ptr<RooChebychev> fPolBackground;
			std::shared_ptr<RooRealVar>   fSigToBckRatio;
			std::vector<std::shared_ptr<RooRealVar> >  fNGauss;

		if(TerminalIO::IsEmptyPtr(hist)) return fParameters;
		// * METHODS * //
		if(true) { // local namespace for testing the beginnings of a class design
			// * Create RooFit variable and data distribution * //
				ImportDistribution(hist, particle);

			// * Create Breit-Wigner function and fit * //
				RooRealVar mean(
					Form("m_{%s}", particle.NameLaTeX()),
					Form("%s mass", particle.NameLaTeX()),
					particle.Mass(), particle.LowerMass(), particle.UpperMass());
				RooRealVar width("width", Form("%s width", particle.NameLaTeX()),
					particle.BWPureWidth(), 0., 100*particle.BWPureWidth());
				RooGaussian signal("breitwigner",
					Form("Breit-Wigner PDF for #it{M}_{%s} distribution", particle.DaughterLabel()),
					*fVariable, mean, width);
				RooRealVar n("N_{BW}", "N_{BW}", 1e2, 0., 1e6);
				RooArgList fComponents(signal);
				RooArgList fNContributions(n);

			// * Add polynomial background if required * //
				RooArgList fBckParameters;
				for(UChar_t i = 0; i <= particle.NPol(); ++i) {
					auto p = new RooRealVar(Form("p%u", i), Form("p%u", i), 0., -1e6, 1e6);
					fBckParameters.add(*p);
				}
				RooChebychev fPolBackground("polBkg",
					Form("Polynomial-%u background", particle.NPol()),
					*fVariable, fBckParameters);
				RooRealVar fSigToBckRatio(
					Form("N_{pol%u}", particle.NPol()),
					Form("N_{pol%u}", particle.NPol()),
					0., 0., 1e5);
				if(particle.NPol()) {
					fComponents.add(fPolBackground);
					fNContributions.add(fSigToBckRatio);
				}

			// * Add the components and fit * //
				fFullShape = std::make_shared<RooAddPdf>("full_shape", "Breit-Wigner + background", fComponents, fNContributions);
				auto result = fFullShape->fitTo(
					*fDistribution,
					RooFit::Range(particle.FitFrom(), particle.FitUntil()));

			// * Plot results and save * //
				RooPlot *frame = fVariable->frame(); // create a frame to draw
				frame->SetAxisRange(particle.PlotFrom(), particle.PlotUntil());
				fDistribution->plotOn(frame, // draw distribution
					RooFit::LineWidth(2), RooFit::LineColor(kBlue+2), RooFit::LineWidth(1),
					RooFit::MarkerColor(kBlue+2), RooFit::MarkerSize(.5));
				fFullShape->plotOn(frame, RooFit::LineWidth(2), RooFit::LineColor(kBlack));
				if(particle.NPol()) {
					fFullShape->plotOn(frame, RooFit::Components(signal), // draw Breit-Wigner
						RooFit::LineWidth(1), RooFit::LineColor(kRed-4));
					fFullShape->plotOn(frame, RooFit::Components(fPolBackground), // draw background
						RooFit::LineStyle(kDashed), RooFit::LineWidth(1), RooFit::LineColor(kGray));
				}
				fFullShape->paramOn(frame, RooFit::Layout(.56, .98, .92));

			// * Write fitted histograms * //
				TCanvas c;
				c.SetBatch();
				frame->Draw();
				Draw::SaveCanvas(Form("BreitWigner_%s", particle.Name()), &c);
		}
		return fParameters;
	}


	/// Fit the sum of two Gaussian functions on a invariant mass distrubution. The mean of the two Gaussian is in both cases taken to be the mass of the particle to be reconstructed.
		/// For a pure particle signal, that is, without backround @b and without a physical particle width, the width of the two Gaussians characterises the resolution of the detector.
		/// See https://root.cern.ch/roofit-20-minutes for an instructive tutorial.
		/// @param hist Invariant mass histogram that you would like to fit
		/// @param particle Hypothesis particle: which particle are you reconstructing? All analysis parameters, such as estimates for Gaussian widths, are contained within this object.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	std::vector<std::shared_ptr<RooRealVar> > RooFitBuilder::FitPureGaussians(TH1F *hist, const ReconstructedParticle &particle, TString logScale, TString outputName, const bool fixpars)
	{
		// * DATA MEMBERS * //
			RooArgList fBckParameters;
			RooArgList fComponents;
			RooArgList fNContributions;
			std::shared_ptr<RooAddPdf>    fFullShape;
			std::shared_ptr<RooChebychev> fPolBackground;
			std::shared_ptr<RooRealVar>   fSigToBckRatio;
			std::vector<std::shared_ptr<RooRealVar> >  fNGauss;

		if(TerminalIO::IsEmptyPtr(hist)) return fParameters;
		// * METHODS * //
		if(true) { // local namespace for testing the beginnings of a class design
			// * Create RooFit variable and data distribution * //
				ImportDistribution(hist, particle);

			// * Create Gaussian functions * //
				BuildMean(particle);
				BuildSigmas(fixpars);
				for(UChar_t i = 0; i < fSigmas.size(); ++i) {
					fGaussians.push_back(std::make_shared<RooGaussian>(
						Form("gauss%u", i+1),
						Form("Gaussian PDF %u for #it{M}_{%s} distribution", i+1, particle.DaughterLabel()),
						*fVariable, *fParameters[0], *fParameters[1+i]));
					fNGauss.push_back(std::make_shared<RooRealVar>(
						Form("N_{gaus%u}", i+1),
						Form("N_{gaus%u}", i+1),
						1e2, 0., 1e6));
					fComponents.add(*fGaussians[i]);
					fNContributions.add(*fNGauss[i]);
				}

			// * Add polynomial background if required * //
				for(UChar_t i = 0; i <= particle.NPol(); ++i) {
					fBckParameters.addClone(RooRealVar(Form("p%u", i), Form("p%u", i), 0., -1e6, 1e6));
				}
				fPolBackground = std::make_shared<RooChebychev>("polBkg",
					Form("Polynomial-%u background", particle.NPol()),
					*fVariable, fBckParameters);
				fSigToBckRatio = std::make_shared<RooRealVar>(
					Form("N_{pol%u}", particle.NPol()),
					Form("N_{pol%u}", particle.NPol()),
					0., 0., 1e5);
				if(particle.NPol()) {
					fComponents.add(*fPolBackground);
					fNContributions.add(*fSigToBckRatio);
				}

			// * Add the components and fit * //
				fFullShape = std::make_shared<RooAddPdf>("full_shape", "Double gaussian + background", fComponents, fNContributions);
				fFullShape->fitTo(
					*fDistribution,
					RooFit::Range(particle.FitFrom(), particle.FitUntil()));


			// * Plot results and save * //
				auto frame = fVariable->frame(); // create a frame to draw
				frame->SetAxisRange(particle.PlotFrom(), particle.PlotUntil());
				fDistribution->plotOn(frame, // draw distribution
					RooFit::LineWidth(2), RooFit::LineColor(kBlue+2), RooFit::LineWidth(1),
					RooFit::MarkerColor(kBlue+2), RooFit::MarkerSize(.5));
				fFullShape->plotOn(frame, RooFit::LineWidth(2), RooFit::LineColor(kBlack)); // draw sig+bck fit
				for(UChar_t i = 0; i < particle.GaussianWidths().size(); ++i) {
					fFullShape->plotOn(frame, RooFit::Components(*fComponents.at(i)),
						RooFit::LineWidth(1), RooFit::LineColor(Settings::Draw::gColorPalette[i]));
				}
				if(particle.NPol()) {
					fFullShape->plotOn(frame, RooFit::Components(*fPolBackground), // draw background
						RooFit::LineStyle(kDashed), RooFit::LineWidth(1), RooFit::LineColor(kGray));
				}
				fFullShape->paramOn(frame, RooFit::Layout(.56, .98, .92));

			// * Write fitted histograms * //
				if(outputName.EqualTo("")) outputName = particle.Name();
				outputName.ReplaceAll("/", ""); // in case of for instance "J/psi"
				Draw::DrawAndSave(Form("DoubleGauss_%s", outputName.Data()), "", logScale, frame);
				delete frame;
		}
		return fParameters;
	}



// * ======================= * //
// * ------- GETTERS ------- * //
// * ======================= * //






// * =============================== * //
// * ------- PRIVATE METHODS ------- * //
// * =============================== * //


