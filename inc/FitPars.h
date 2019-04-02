#ifndef BOSS_Afterburner_FitPars_H
#define BOSS_Afterburner_FitPars_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "TString.h"
	#include <vector>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// @remark **DEVELOPMENTAL**
	/// This object is used as a test whether the loading of parameters from a config file can be streamlined through an object other than `ReconstructedParticle`.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     April 2nd, 2019
	class FitPars
	{
	public:
		FitPars() {}
		FitPars(const TString &input_sig, const TString &input_bck);
		void Print() const;
		void Set(const TString &input_sig, const TString &input_bck);
		void SetBackground(const TString &input) { Set(fBkgPars, input); }
		void SetSignal(const TString &input) { Set(fSigPars, input); }
		const std::vector<double>& BackgroundParameters() const { return fBkgPars; }
		const std::vector<double>& SignalParameters() const { return fSigPars; }
	private:
		void Set(std::vector<double>& vec, TString input);
		std::vector<double> fBkgPars; ///< Vector of parameters for the background.
		std::vector<double> fSigPars; ///< Vector of parameters for the signal.
	};


/// @}
#endif