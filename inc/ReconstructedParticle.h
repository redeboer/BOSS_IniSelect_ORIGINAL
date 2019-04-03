#ifndef BOSS_Afterburner_ReconstructedParticle_H
#define BOSS_Afterburner_ReconstructedParticle_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "FitPars.h"
	#include "Particle.h"
	#include "RooRealVar.h"
	#include "RooGaussian.h"
	#include "TString.h"
	#include <list>
	#include <vector>
	#include <utility>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Extended data container of a reconstructed particle.
		/// In essence an container of `ROOT`'s `TParticlePDG`, but with extended functionality for particle reconstruction. It contains additional parameters that are relevant when reconstructing the particle, like fit range in the invariant mass plot. The idea is that you can use this object to generalise functions like fitting, where the mass range is particle-specific. It is then convenient to only have to feed the function this object.
		/// @todo Somehow, a distinction has to be made for different decay channels (not just daughters) in the parameters of `ReconstructedParticle`.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     October 25th, 2018
	class ReconstructedParticle : public Particle
	{
	public:
		/// @name Constructors
			///@{
			ReconstructedParticle(int pdgCode);
			ReconstructedParticle(const char* pdgCode);
			ReconstructedParticle(int pdgCode, const char* daughters);
			ReconstructedParticle(const char* pdgCode, const char* daughters);
			///@}

		/// @name Setters
			///@{
			void SetDaughterLabel(const char* daughters);
			void SetFitPars(const TString& input_sig, const TString& input_bkg) { fFitParameters.Set(input_sig, input_bkg); }
			void SetGaussianPars(TString input);
			void ImportFitPars_DG();
			///@}

		/// @name RooFit
			///@{
			std::shared_ptr<RooRealVar> RooRealVarInvMass();
			std::shared_ptr<RooRealVar> RooRealVarMean();
			std::shared_ptr<RooRealVar> RooRealVarMeanZero();
			std::shared_ptr<RooRealVar> RooRealVarSigma(UChar_t num);
			std::shared_ptr<RooRealVar> RooRealVarSigmaFixed(UChar_t num);
			///@}

		/// @name Getters
			///@{
			const char* DaughterLabel() const { return fDaughterLabels.Data(); }
			const double BWConvolutedWidth() const { return fBWConvolutedWidth; }
			const double BWPureWidth() const { return fBWPureWidth; }
			const double FitFrom() const { return fFitRange.first; }
			const double FitUntil() const  { return fFitRange.second; }
			const double GaussianSmallWidth() const { return fGaussianWidths.front(); }
			const double GaussianWideWidth() const { return fGaussianWidths.back(); }
			const double LowerMass() const;
			const double MassOffsetPercentage() const { return fMassOffset; }
			const double PlotFrom() const { return fPlotRange.first; }
			const double PlotUntil() const { return fPlotRange.second; }
			const double UpperMass() const;
			const std::vector<double> GaussianWidths() const { return fGaussianWidths; }
			const UChar_t NPol() const { return fNPol; }
			const FitPars& FitParameters() const { return fFitParameters; }
			///@}

	protected:
		/// @name Data members
			///@{
			FitPars fFitParameters; ///< Vector of fit parameters. @remark Temporary implementations, see `FitPars`.
			TString fDaughterLabels; ///< LaTeX formatted string for decay particles.
			double fBWConvolutedWidth; ///< Estimate for the width of the Breit-Wigner function when convoluted with a double Gaussian.
			double fBWPureWidth; ///< Estimate for the width of the Breit-Wigner function when fitting BW only.
			double fMassOffset; ///< Percentage (divided by 100) that the mean (namely, the mass) may vary.
			UChar_t fNPol; ///< Degree of the background polynomial (whether Chebychev or normal polynomial).
			std::vector<double> fGaussianWidths; ///< Vector of for Gaussian widths that you can use that as estimates of the widths for e.g. the double gaussian that you plan to fit. These sigmas are supposed to characterise the resolution of the detector. For consistency in naming, the first one should be smaller than the second and so forth.
			std::pair<double, double> fFitRange; ///< Invariant mass range over which you fit a function (double Gaussian, Crystal ball, Breit-Wigner, etc.).
			std::pair<double, double> fPlotRange; ///< Invariant mass range that you plot.
			std::list<std::shared_ptr<RooRealVar> > fRooRealVars; ///< This list is to ensure that `RooRealVar` remain existing untill this `ReconstructedParcticle` is destroyed.
			std::list<std::shared_ptr<RooGaussian> > fRooGaussians; ///< This list is to ensure that `RooGaussian` remain existing untill this `ReconstructedParcticle` is destroyed.
			///@}

		/// @name Helper methods
			///@{
			void DetermineReconstructionParameters();
			void Initialize();
			///@}

	};


/// @}
#endif