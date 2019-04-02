#ifndef BOSS_Afterburner_ReconstructedParticle_H
#define BOSS_Afterburner_ReconstructedParticle_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "Particle.h"
	#include "TString.h"
	#include <utility>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Extended data container of a reconstructed particle. In essence an container of `ROOT`'s `TParticlePDG`, but with extended functionality for particle reconstruction. It contains additional parameters that are relevant when reconstructing the particle, like fit range in the invariant mass plot. The idea is that you can use this object to generalise functions like fitting, where the mass range is particle-specific. It is then convenient to only have to feed the function this object.
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
			///@}

		/// @name Getters
			///@{
			const char* DaughterLabel() const { return fDaughterLabels.Data(); }
			const double BWConvolutedWidth() const { return fBWConvolutedWidth; }
			const double BWPureWidth() const { return fBWPureWidth; }
			const double FitFrom() const { return fFitRange.first; }
			const double FitUntil() const  { return fFitRange.second; }
			const double GaussianSmallWidth() const { return fDoubleGaussianWidths.first; }
			const double GaussianWideWidth() const { return fDoubleGaussianWidths.second; }
			const double LowerMass() const;
			const double MassOffsetPercentage() const { return fMassOffset; }
			const double PlotFrom() const { return fPlotRange.first; }
			const double PlotUntil() const { return fPlotRange.second; }
			const double SingleGaussianWidth() const { return fSingleGaussianWidth; }
			const double UpperMass() const;
			const UChar_t NPol() const { return fNPol; }
			///@}

	protected:
		/// @name Data members
			///@{
			TString fDaughterLabels; ///< LaTeX formatted string for decay particles.
			double fBWConvolutedWidth; ///< Estimate for the width of the Breit-Wigner function when convoluted with a double Gaussian.
			double fBWPureWidth; ///< Estimate for the width of the Breit-Wigner function when fitting BW only.
			double fMassOffset; ///< Percentage (divided by 100) that the mean (namely, the mass) may vary.
			double fSingleGaussianWidth; ///< Estimate for the width of one Gaussian function.
			UChar_t fNPol; ///< Degree of the background polynomial (whether Chebychev or normal polynomial).
			std::pair<double, double> fDoubleGaussianWidths; ///< Pair of two sigma values. You can use that as estimates of the widths for the double gaussian that you plan to fit. These sigmas are supposed to characterise the resolution of the detector. For consistency in naming, the first one should be smaller than the second.
			std::pair<double, double> fFitRange; ///< Invariant mass range over which you fit a function (double Gaussian, Crystal ball, Breit-Wigner, etc.).
			std::pair<double, double> fPlotRange; ///< Invariant mass range that you plot.
			///@}

		/// @name Helper methods
			///@{
			void DetermineReconstructionParameters();
			void Initialize();
			///@}

	};


/// @}
#endif