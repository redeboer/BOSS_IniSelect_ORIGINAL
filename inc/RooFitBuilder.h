#ifndef BOSS_Afterburner_RooFitBuilder_H
#define BOSS_Afterburner_RooFitBuilder_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "RooDataHist.h"
	#include "RooGaussian.h"
	#include "RooRealVar.h"
	#include "RooAddPdf.h"
	#include "TString.h"
	#include <list>
	#include <utility>
	#include <vector>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// **DEVELOPMENTAL** Object that can be used to construct and fit `RooFit` objects.
		/// One of the challenges with `RooFit` is whether obhects in memory are owned or not. Usually, you write one script and build fits from `RooFit` objects within one scope, but if you want to modulate this process (whether through functions or through some `RooFit` wrapper class), you'll run into trouble.
		///
		/// This class is an attempt streamlining that process of building `RooFit` objects within a modulated class. The class makes use of shared pointers (`shared_ptr`) to reduce the risk of memory leaks while at the same time being able to pass the objects as pointer arguments to `RooFit`.
		///
		/// At present, the class is not fully developed and is therefore not use anywhere else, but has been included in the release to show how to 'own' and maintain of `RooFit` object through shared pointers.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     April 3rd, 2019
	class RooFitBuilder
	{
	public:
		/// @name Constructors
			///@{
			///@}


		/// @name Setters
			///@{
			void ImportDistribution(TH1F *hist, const char *id, const char *name, const char *title, const double from, const double to);
			void ImportDistribution(TH1F *hist, const ReconstructedParticle &particle);
			///@}


		/// @name Builders
			///@{
			std::shared_ptr<RooRealVar> BuildParameter(const char* name, const char* title, Double_t value, const char* unit="");
			std::shared_ptr<RooRealVar> BuildParameter(const char* name, const char* title, Double_t value, Double_t minValue, Double_t maxValue, const char* unit="");
			std::shared_ptr<RooRealVar> BuildMean(const ReconstructedParticle& particle);
			std::shared_ptr<RooRealVar> BuildSigma(const ReconstructedParticle& particle, UChar_t num, const bool fix=false);
			std::vector<std::shared_ptr<RooRealVar> >& BuildSigmas(const ReconstructedParticle& particle, const bool fix=false);
			std::vector<std::shared_ptr<RooRealVar> > FitConvolutionBWGaussian(TH1F *hist, const ReconstructedParticle &particle, TString logScale);
			std::vector<std::shared_ptr<RooRealVar> > FitBreitWigner(TH1F *hist, const ReconstructedParticle &particle, TString logScale);
			std::vector<std::shared_ptr<RooRealVar> > FitPureGaussians(TH1F *hist, const ReconstructedParticle &particle, TString logScale, TString outputName = "", const bool fixpars=false);
			RooDataHist CreateRooFitInvMassDistr(TH1F *hist, const RooRealVar &var);
			///@}


		/// @name Getters
			///@{
			///@}


	protected:
		/// @name Data members
			///@{
			std::shared_ptr<RooDataHist> fDistribution;
			std::shared_ptr<RooRealVar>  fVariable;
			std::shared_ptr<RooAddPdf>   fFullShape;
			std::vector<std::shared_ptr<RooGaussian> > fGaussians;
			std::vector<std::shared_ptr<RooRealVar> > fSigmas;
			std::vector<std::shared_ptr<RooRealVar> > fParameters;
			static std::list<std::shared_ptr<RooRealVar> > fAllParameters;
			///@}


		/// @name Helper methods
			///@{
			void DetermineReconstructionParameters();
			void Initialize();
			///@}


	};


/// @}
#endif