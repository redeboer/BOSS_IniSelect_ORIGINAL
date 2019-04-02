/// @addtogroup BOSS_Afterburner_scripts
/// @{
// * ======================================== * //
// * ------- LIBRARIES AND NAMESPACES ------- * //
// * ======================================== * //

	#include "ConfigLoader_InvMass.h"
	#include "BOSSOutputLoader.h"
	#include "CommonFunctions.h"
	#include "TStyle.h"
	#include <iostream>
	#include <list>
	#include <vector>

	using namespace CommonFunctions;
	using namespace CommonFunctions::Draw;
	using namespace CommonFunctions::Fit;
	using namespace RooFit;
	using namespace Settings;
	using namespace std;



// * ============================= * //
// * ------- MAIN FUNCTION ------- * //
// * ============================= * //


	/// This script originates from `AnalyseInvMass` and is here for temporary developmental purposes.
		/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
		/// @date     April 2nd, 2019
	void D0phi_KpiKK(const char* configuration_file="configs/debug.config")
	{
		/// -# Attempt to load `configuration_file`.
			ConfigLoader_InvMass config(configuration_file, false);

		/// -# Attempt to load three directories of ROOT files as a `BOSSOutputLoader` objects: a set of exlusive MC files, a set of inclusive MC files, and a set of data files (the BESIII measurements).
			BOSSOutputLoader excl(config.Filename_excl, false, false);
			if(excl.IsZombie()) {
				TerminalIO::PrintFatalError(Form("Failed to load directory/file\n  \"%s\"", config.Filename_excl->c_str()));
				return;
			}
			BOSSOutputLoader incl(config.Filename_incl, false, false);
			if(incl.IsZombie()) {
				TerminalIO::PrintFatalError(Form("Failed to load directory/file\n  \"%s\"", config.Filename_incl->c_str()));
				return;
			}
			BOSSOutputLoader data(config.Filename_data, false, false);
			if(data.IsZombie()) {
				TerminalIO::PrintFatalError(Form("Failed to load directory/file\n  \"%s\"", config.Filename_data->c_str()));
				return;
			}

		/// -# Print three cut flows.
			if(config.Do_PrintCutFlow) {
				std::cout << std::endl << "Cut flow for EXCLUSIVE Monte Carlo data set" << std::endl;
				excl.PrintCutFlow();
				std::cout << std::endl << "Cut flow for INCLUSIVE Monte Carlo data set" << std::endl;
				incl.PrintCutFlow();
				std::cout << std::endl << "Cut flow for MEASUREMENT data set" << std::endl;
				data.PrintCutFlow();
			}

		/// -# Set global plotting style based on loaded configuration settings.
			gStyle->SetOptStat(config.Do_PlotStatsLegend);
			
		/// -# Plot branches without fits
			if(config.Do_PureHists) {
				for(auto &options : *config.ListOfbranches_excl) excl.Draw(options);
				for(auto &options : *config.ListOfbranches_incl) incl.Draw(options);
				for(auto &options : *config.ListOfbranches_data) data.Draw(options);
			}

		/// -# Perform fit over exclusive Monte Carlo data sets.
			if(!config.Do_Fit) return;
			// * Particles to reconstruct
				std::vector<ReconstructedParticle*> particles;
				for(auto &it : *config.ExclFits) particles.push_back(&it.first);

			// * Fit double gaussians
				int i = 0;
				for(auto &fit : *config.ExclFits) {
					/// @todo Trouble with the number of polynomials... Perhaps somehow implement a fit object (extension of `BranchPlotOptions`)?
					FitDoubleGaussian(
						excl[fit.second.TreeName()].GetInvariantMassHistogram(fit.second, fit.first),
						*particles[i], fit.second.LogXYZ().c_str());
					++i;
				}

	}


	/// Main function that is called when executing the executable compiled using e.g. `g++`.
	int main(int argc, char *argv[])
	{
		if(argc > 2) {
			cout << "FATAL ERROR: Cannot run this macro with more than one argument" << endl;
			return 1;
		}
		if(argc==1) D0phi_KpiKK();
		else        D0phi_KpiKK(argv[1]);
		return 0;
	}


/// @}