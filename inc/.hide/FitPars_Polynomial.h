#ifndef BOSS_Afterburner_FitPars_BW_H
#define BOSS_Afterburner_FitPars_BW_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "FitPars.h"




// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Derived container that contains the parameters of a double Gaussian fit.
	/// This object only contains the most essential parameters. You should define the actual parameters in the derived classes (such as `FitPars_BW`).
	///
	/// A `FitPars_BW` object is added to 
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     Mrach 28th, 2019
	class FitPars_BW : public FitPars
	{
	public:
	private:
			double fWidth; ///< Estimate for the width of the Breit-Wigner function when fitting BW only.
			double fMassOffset; ///< Percentage (divided by 100) that the mean (namely, the mass) may vary.
			double fSingleGaussianWidth; ///< Estimate for the width of one Gaussian function.
			UChar_t fNPol; ///< Degree of the background polynomial (whether Chebychev or normal polynomial).
			std::pair<double, double> fDoubleGaussianWidths; ///< Pair of two sigma values. You can use that as estimates of the widths for the double gaussian that you plan to fit. These sigmas are supposed to characterise the resolution of the detector. For consistency in naming, the first one should be smaller than the second.
	};


/// @}
#endif