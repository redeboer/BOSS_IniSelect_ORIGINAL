// * ======================================== * //
// * ------- LIBRARIES AND NAMESPACES ------- * //
// * ======================================== * //

	#include "BOSSOutputLoader.h"
	#include "CommonFunctions.h"
	#include "ConfigLoader.h"
	#include "FrameworkSettings.h"
	#include "TError.h"
	#include "TLine.h"
	#include "TStyle.h"
	#include <iomanip>
	#include <iostream>
	#include <list>
	#include <stdio.h>
	#include <utility>
	#include <vector>

	using namespace CommonFunctions;
	using namespace CommonFunctions::Draw;
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
			ConfigParameter<std::string> BranchName{"Branch name containing invariant mass results", "fit4c_best"};
			ConfigParameter<std::list<BranchPlotOptions> > ListOfbranches_excl{"Plot these inclusive MC branches"};
			ConfigParameter<std::list<BranchPlotOptions> > ListOfbranches_incl{"Plot these exclusive MC branches"};
			ConfigParameter<std::list<BranchPlotOptions> > ListOfbranches_data{"Plot these measurement branches"};
			///@}


		/// @name Do switches
			///@{
			ConfigParameter<bool> Do_PrintCutFlow{"Print cut flow", true}; ///< Whether or not to print an indication of the cut flow for the three files.
			ConfigParameter<bool> Do_PureHists{"Draw and save histograms without fits", true};
			ConfigParameter<bool> Do_Fit_excl{"Draw and save fits exclusive", true};
			ConfigParameter<bool> Do_Signal{"Plot inclusive and data signal", true};
			ConfigParameter<bool> Do_Fit_incl{"Draw and save fits data and inclusive", true};
			///@}


		/// @name Fit parameters
			///@{};
			ConfigParameter<std::list<std::pair<ReconstructedParticle, BranchPlotOptions> > > ParticleBranchPairs {"Signals to fit"};
			ConfigParameter<int> nSigma_cut  {"nSigma cut",        3};
			ConfigParameter<int> nSigma_plot {"nSigma plot range", 5};
			ConfigParameter<std::vector<double> > BinWidths {"Bin widths", {.004, .002}};
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
		/// <ol>
		/// <li> Attempt to load `configuration_file`.
			ConfigLoader_AnalyseInvMass config(configuration_file, false);

		/// <li> Attempt to load three directories of ROOT files as a `BOSSOutputLoader` objects: a set of exlusive MC files, a set of inclusive MC files, and a set of data files (the BESIII measurements).
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

		/// <li> Print three cut flows.
			if(config.Do_PrintCutFlow) {
				std::cout << std::endl << "Cut flow for EXCLUSIVE Monte Carlo data set" << std::endl;
				excl.PrintCutFlow();
				std::cout << std::endl << "Cut flow for INCLUSIVE Monte Carlo data set" << std::endl;
				incl.PrintCutFlow();
				std::cout << std::endl << "Cut flow for MEASUREMENT data set" << std::endl;
				data.PrintCutFlow();
			}

		/// <li> Set global plotting style based on loaded configuration settings.
			gStyle->SetOptStat(config.Do_PlotStatsLegend);
			
		/// <li> Plot branches without fits
			if(config.Do_PureHists) {
				for(auto &options : *config.ListOfbranches_excl) excl.Draw(options);
				for(auto &options : *config.ListOfbranches_incl) incl.Draw(options);
				for(auto &options : *config.ListOfbranches_data) data.Draw(options);
			}

		/// <li> Perform fit over exclusive Monte Carlo data sets.
			/// <ol>
			/// <li> **Abort** if config file says fits should not be performed.
				if(!config.Do_Fit_excl) return;

			/// <li> Create vector of particles.
				std::vector<ReconstructedParticle*> particles;
				for(auto &partBranch : *config.ParticleBranchPairs) particles.push_back(&partBranch.first);

			/// <li> Fit and draw double gaussians.
				std::vector<std::vector<std::shared_ptr<RooRealVar> > > fitParameters;
				for(auto &partParam : *config.ParticleBranchPairs) {
					fitParameters.push_back(
						partParam.first.FitPureGaussians(
							excl[partParam.second.TreeName()].GetInvariantMassHistogram(partParam.second, partParam.first),
							partParam.second.LogXYZ().c_str()
						));
				}

			/// <li> Print fit results.
				auto numparts{config.ParticleBranchPairs->size()};
				std::vector<Double_t> masses(numparts);
				std::vector<Double_t> sigmas(numparts);
				for(size_t i = 0; i < numparts; ++i) {
					/// <ol>
					/// <li> Print opening line particle name for this fit result.
						std::printf("\nFinal fit parameters for %s\n", particles[i]->Name());
					/// <li> Compute print widths for name and final fit value columns.
						Int_t width_name{0};
						Int_t width_val{0};
						for(auto &rooVar : fitParameters[i]) {
							auto value = rooVar->getVal();
							TString name{rooVar->GetName()};
							TString vstr{Form("%g", value)};
							if(name.Length() > width_name) width_name = name.Length();
							if(vstr.Length() > width_val ) width_val  = vstr.Length();
						}
					/// <li> Compute total sigma.
						Double_t sigma{0.};
						Double_t sigmaErr{0.};
						for(auto &rooVar : fitParameters[i]) {
							auto value = rooVar->getVal();
							TString name{rooVar->GetName()};
							TString vstr{Form("%g", value)};
							if(name.Length() > width_name) width_name = name.Length();
							if(vstr.Length() > width_val ) width_val  = vstr.Length();
							if(name.Contains("sigma")) {
								sigma += value*value;
								sigmaErr += rooVar->getError()*rooVar->getError();
							}
						}
					/// <li> Print a table of fit parameters with their names and values.
						for(auto &rooVar : fitParameters[i]) {
							std::cout << "  "
								<< std::setw(width_name) << std::left << rooVar->GetName() << " = "
								<< std::left << std::fixed << rooVar->getVal()  << " \\pm "
								<< rooVar->getError() << std::endl;
						}
					/// <li> Push back a pair of mass with total signal.
						masses[i] = fitParameters[i][0]->getVal();
						sigmas[i] = std::sqrt(sigma);
					/// </ol>
				}

			/// <li> Print masses as determined from exclusive MC.
				std::cout << std::endl;
				std::cout << "Masses determined from exclusive MC:" << std::endl;
				Int_t width{0};
				for(auto &fitParameter : fitParameters) {
					TString name{fitParameter[0]->GetName()};
					if(name.Length() > width) width = name.Length();
				}
				for(size_t i = 0; i < numparts; ++i) {
					std::cout << "  "
						<< std::setw(width) << fitParameters[i][0]->GetName() << " = "
						<< masses[i] << " \\pm "
						<< sigmas[i] << std::endl;
				}

			/// </ol>
		/// <li> Plot signal histograms.
			/// <ol>
			/// <li> **Abort** if config file says fits should not be performed.
			if(!config.Do_Signal) return;
			std::cout << std::endl;
			size_t i{0};
			std::vector<TH1F*> hists_data(numparts);
			std::vector<TH1F*> hists_incl(numparts);
			for(auto &particleBranch : *config.ParticleBranchPairs) {
			/// <li> Create a new `BranchPlotOptions` from the one used for the exclusive plots.
				BranchPlotOptions options{particleBranch.second};
			/// <li> Set a \f$3\sigma\f$ cut on the invariant masses of the *other* particles.
				TString varexp;
				for(size_t j = 0; j < numparts; ++j) {
					TString particleName{particles[j]->Name()};
					if(particleName.EqualTo(particleBranch.first.Name())) continue;
					varexp += Form("m%s > %f && ", particleName.Data(), masses[j] - config.nSigma_cut*sigmas[j]);
					varexp += Form("m%s < %f && ", particleName.Data(), masses[j] + config.nSigma_cut*sigmas[j]);
				}
				if(varexp.Length()>4) varexp.Resize(varexp.Length()-4);
				options.SetCutSelection(varexp);
			/// <li> Set a \f$5\sigma\f$ as plot range.
				options[0].SetRange(
					masses[i] - config.nSigma_plot*sigmas[i],
					masses[i] + config.nSigma_plot*sigmas[i]);
				options[0].SetBinWidth(config.BinWidths->at(i));
			/// <li> **Temporary**: Print the new `BranchPlotOptions` object.
				options.SetDrawOptions("e1");
				double x1, y1, x2, y2;
				options.SetOutputFileName(Form("signal_%s_data", particles[i]->Name()));
				hists_data[i] = dynamic_cast<TH1F*>(data.Draw(options));
					hists_data[i]->SetName(Form("hist_data_%s", particles[i]->Name()));
					gPad->Update();
					gPad->GetRangeAxis(x1, y1, x2, y2);
				double max = y2;
				options.SetOutputFileName(Form("signal_%s_incl", particles[i]->Name()));
				hists_incl[i] = dynamic_cast<TH1F*>(incl.Draw(options));
					hists_incl[i]->SetName(Form("hist_incl_%s", particles[i]->Name()));
					gPad->Update();
					gPad->GetRangeAxis(x1, y1, x2, y2);
					if(y2 > max) max = y2;
			/// <li> Modify histogram.
				hists_data[i]->SetLineColor(Settings::Draw::gColorPalette[0]);
				hists_incl[i]->SetLineColor(Settings::Draw::gColorPalette[1]);
				hists_data[i]->GetXaxis()->SetTitle(Form("m_{%s #rightarrow %s} (%s)", particles[i]->NameLaTeX(), particles[i]->DaughterLabel(), Detector::gUnitGeV));
				hists_incl[i]->GetXaxis()->SetTitle(Form("m_{%s #rightarrow %s} (%s)", particles[i]->NameLaTeX(), particles[i]->DaughterLabel(), Detector::gUnitGeV));
				hists_data[i]->GetYaxis()->SetTitle(Form("counts / %g %s", options[0].BinWidth(), Detector::gUnitGeV));
				hists_incl[i]->GetYaxis()->SetTitle(Form("counts / %g %s", options[0].BinWidth(), Detector::gUnitGeV));
				hists_data[i]->GetYaxis()->SetRangeUser(0., max);
				hists_incl[i]->GetYaxis()->SetRangeUser(0., max);
			/// <li> Draw mass lines
				auto line  = new TLine(masses[i], 0, masses[i], max);
				auto line1 = new TLine(
					masses[i] + config.nSigma_cut*sigmas[i], 0,
					masses[i] + config.nSigma_cut*sigmas[i], max);
				auto line2 = new TLine(
					masses[i] - config.nSigma_cut*sigmas[i], 0,
					masses[i] - config.nSigma_cut*sigmas[i], max);
				line ->SetLineStyle(9);
				line ->SetLineColor(kGray+2);
				line1->SetLineColor(kGray+2);
				line2->SetLineColor(kGray+2);
				line1->SetLineStyle(3);
				line2->SetLineStyle(3);
			/// <li> Save canvas.
				gPad->Clear();
				hists_data[i]->Draw("e1");
				hists_incl[i]->Draw("e1 same");
				gPad->RangeAxis(x1, 0., x2, max);
				gPad->Update();
				line ->Draw();
				line1->Draw();
				line2->Draw();
				SaveCanvas(Form("signals_%s", particleBranch.first.Name()));
			/// </ol>
			++i;
			}
		/// <li> Perform fit over data and inclusve Monte Carlo data sets.
			/// <ol>
			/// <li> **Abort** if config file says fits on signal of data and incluive MC should not be performed.
				if(!config.Do_Fit_incl) return;

			/// <li> Fit and draw double gaussians.
				i = 0;
				for(auto &partParam : *config.ParticleBranchPairs) {
					partParam.first.FitPureGaussians(hists_data[i], "", hists_data[i]->GetName(), true);
					++i;
				}

			/// </ol>
		/// </ol>
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