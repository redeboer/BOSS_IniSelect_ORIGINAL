// * ======================================== * //
// * ------- LIBRARIES AND NAMESPACES ------- * //
// * ======================================== * //

	#include "BOSSOutputLoader.h"
	#include "CommonFunctions.h"
	#include "ConfigLoader.h"
	#include <iostream>

	using namespace CommonFunctions;



// * ================================= * //
// * ------- CLASS DEFINITIONS ------- * //
// * ================================= * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Derived class of `ConfigLoader` that contains parameters used for the `InvestigateContent` algorithm.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     March 26th, 2019
	class ConfigLoader_InvestigateContent : public ConfigLoader
	{
	public:
		ConfigLoader_InvestigateContent(const std::string &path, const bool print=true) : ConfigLoader(path, print) { LoadConfiguration(path); }
		/// @name I/O strings
			///@{
			ConfigParameter<std::string> fInputFilename{"Input file or directory", "/mnt/d/IHEP/root/D0phi_KpiKK/Jpsi2009+2012_dst"};
			///@}


		/// @name Do switches
			///@{
			ConfigParameter<bool> fPrintBranches{"Print branches", true}; ///< Whether or not to print all branch names.
			ConfigParameter<bool> fPrintCutFlow{"Print cut flow", false}; ///< Whether or not to print the cut flow branch (if available).
			///@}


	};



/// @}
// * ============================= * //
// * ------- MAIN FUNCTION ------- * //
// * ============================= * //
/// @addtogroup BOSS_Afterburner_scripts
/// @{


	/// Simple script that allows to quickly investigate the `TTree`s and branches of a ROOT file that has been produced with BOSS.
		/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
		/// @date     March 26th, 2019
	void InvestigateContent(const char* configuration_file="configs/debug.config")
	{
		/// -# Attempt to load `configuration_file`.
		ConfigLoader_InvestigateContent config(configuration_file, true);

		/// -# Attempt to load input ROOT file as a `BOSSOutputLoader` object).
		BOSSOutputLoader file(config.fInputFilename, false, false);
		if(file.IsZombie()) {
			TerminalIO::PrintFatalError(Form("Failed to load ROOT file\n  \"%s\"", config.fInputFilename->c_str()));
			return;
		}

		/// -# If loaded successfully, print the branch names (if requested by `fPrintBranches`).
			if(config.fPrintBranches) file.Print();

		/// -# If loaded successfully, print the cut flow (if requested by `fPrintCutFlow`).
			if(config.fPrintCutFlow) file.PrintCutFlow();

	}


	/// Main function that is called when executing the executable compiled using e.g. `g++`.
	int main(int argc, char *argv[])
	{
		if(argc > 2) {
			std::cout << "FATAL ERROR: Cannot run this macro with more than one argument" << std::endl;
			return 1;
		}
		if(argc==1) InvestigateContent();
		else        InvestigateContent(argv[1]);
		return 0;
	}



/// @}