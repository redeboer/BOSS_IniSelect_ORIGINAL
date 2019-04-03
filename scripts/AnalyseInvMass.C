// * ======================================== * //
// * ------- LIBRARIES AND NAMESPACES ------- * //
// * ======================================== * //

	#include "BOSSOutputLoader.h"
	#include "CommonFunctions.h"
	#include "ConfigLoader.h"
	#include "TStyle.h"
	#include "TError.h"
	#include <iostream>
	#include <list>
	#include <vector>

	using namespace CommonFunctions;
	using namespace CommonFunctions::Draw;
	using namespace CommonFunctions::Fit;
	using namespace RooFit;
	using namespace Settings;
	using namespace std;



// * ================================= * //
// * ------- CLASS DEFINITIONS ------- * //
// * ================================= * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Derived class of `ConfigLoader` that contains parameters used for a simple plotting algorithm.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     March 26th, 2019
	class ConfigLoader_AnalyseInvMass : public ConfigLoader
	{
	public:
		ConfigLoader_AnalyseInvMass(const std::string &path, const bool print=true) : ConfigLoader(path, print) { ConfigParBase::PrintAll();
		LoadConfiguration(path);
		}
		/// @name I/O strings
			///@{
			ConfigParameter<std::string> Filename_excl{"Exclusive MC file"};
			ConfigParameter<std::string> Filename_incl{"Inclusive MC file"};
			ConfigParameter<std::string> Filename_data{"Measurements file"};
			ConfigParameter<std::list<BranchPlotOptions> > ListOfbranches_excl{"Plot these inclusive MC branches"};
			ConfigParameter<std::list<BranchPlotOptions> > ListOfbranches_incl{"Plot these exclusive MC branches"};
			ConfigParameter<std::list<BranchPlotOptions> > ListOfbranches_data{"Plot these measurement branches"};
			///@}


		/// @name Do switches
			///@{
			ConfigParameter<bool> Do_PrintCutFlow{"Print cut flow", true}; ///< Whether or not to print an indication of the cut flow for the three files.
			ConfigParameter<bool> Do_PureHists{"Draw and save histograms without fits", true};
			ConfigParameter<bool> Do_Fit{"Draw and save fits", true};
			///@}


		/// @name Fit parameters
			///@{
			ConfigParameter<std::list<std::pair<ReconstructedParticle, BranchPlotOptions> > > ExclFits {"Signals to fit"};
			///@}


		/// @name Global plot style options
			///@{
			ConfigParameter<bool> Do_PlotStatsLegend{"Plot legend with statistics", false}; ///< Whether or not to draw the legend in the upper right corner with histogram statistics.
			///@}


	};



/// @}
// * ============================= * //
// * ------- MAIN FUNCTION ------- * //
// * ============================= * //
/// @addtogroup BOSS_Afterburner_scripts
/// @{


	/// This script originates from `AnalyseInvMass` and is here for temporary developmental purposes.
		/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
		/// @date     April 2nd, 2019
	void D0phi_KpiKK(const char* configuration_file="configs/debug.config")
	{
		/// -# Attempt to load `configuration_file`.
			ConfigLoader_AnalyseInvMass config(configuration_file, false);

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
				std::vector<std::vector<std::shared_ptr<RooRealVar> > > results;
				for(auto &fit : *config.ExclFits) {
					results.push_back(FitPureGaussians(
						excl[fit.second.TreeName()].GetInvariantMassHistogram(fit.second, fit.first),
						*particles[i], fit.second.LogXYZ().c_str()));
					++i;
				}
				for(auto &res : results) for(auto &r : res) std::cout << r->GetName() << std::endl;

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