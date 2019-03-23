#ifndef BOSS_Afterburner_BranchPlotOptions_H
#define BOSS_Afterburner_BranchPlotOptions_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "CommonFunctions.h"
	#include "AxisBinning.h"
	#include <vector>
	#include <string>



// * ================================= * //
// * ------- CLASS DEFINITIONS ------- * //
// * ================================= * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// 
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     March 23rd, 2018
	class BranchPlotOptions
	{
	public:
		BranchPlotOptions(const char* varexp, const char* selection="", const Option_t *option="") :
			fVarExp(varexp), fCutSelection(selection), fDrawOption(option) {}

		const char* VarExp() const { return fVarExp.c_str(); }
		const char* CutSelection() const { return fCutSelection.c_str(); }
		const char* DrawOption() const { return fDrawOption.c_str(); }

	private:
		std::string fVarExp;
		std::string fCutSelection;
		std::string fDrawOption;

	};


/// @}
#endif