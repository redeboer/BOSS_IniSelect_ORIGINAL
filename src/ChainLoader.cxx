// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ChainLoader.h"
	#include "CommonFunctions.h"
	#include "FrameworkSettings.h"
	#include "TList.h"
	#include "TTimeStamp.h"
	#include <iomanip>
	#include <iostream>
	using namespace CommonFunctions;



// * ============================ * //
// * ------- CONSTRUCTORS ------- * //
// * ============================ * //


	/// Automatically book addresses for all branches of the underlying `TChain`. The branches are accessible by name (use method `Get_<type>`, though you have to the `type` to use this).
		/// @param print_branches Whether or not to print the `TChain` names and its branches+types. Use `true` for debugging purposes in particular.
		/// @param print_averages Whether or not to @b compute and print the averages of each branch. Use `true` for debugging purposes in particular. **Using this setting costs time!**
	void ChainLoader::BookAddresses(bool print_branches, bool print_averages)
	{
		if(!fChain.GetNbranches()) return;
		/// -# Get list of `TBranches`.
			TIter next(fChain.GetListOfBranches());
			TObject *obj  = nullptr;
			if(print_branches) {
				std::cout << "Tree \"" << fChain.GetName() << "\" (" << fChain.GetEntries() << " entries)" << std::endl;
				std::cout << fChain.GetTitle() << std::endl;
				std::cout << "   " << std::setw(18) << std::left  << "BRANCH NAME";
				if(print_averages) std::cout << std::setw(12) << std::right << "MEAN" << std::endl;
				std::cout << std::endl;
			}
		/// -# Loop over list of `TBranches` and determine `typename` of the branch. The data type of a branch can be determined from the last character of its title. See <a href="https://root.cern.ch/doc/master/classTTree.html#a8a2b55624f48451d7ab0fc3c70bfe8d7">`TTree`</a> for the labels of each type.
			while((obj = next())) {
				std::string type(obj->GetTitle());
				if(print_branches) {
					std::cout << "   " << std::setw(18) << std::left << type;
					if(print_averages) std::cout << std::setw(12) << std::right << ComputeMean(&fChain, obj->GetName());
				}
				if(type.find('[') != std::string::npos) {
					if(print_branches) std::cout << " (not loaded)" << std::endl;
					continue; /// If branch title contains a `[`, this means it is an array and it will not be loaded.
				}
				if(print_branches) std::cout << std::endl;
				switch(type.back()) {
					case 'B' : SetAddress(obj, fMap_B); break;
					case 'b' : SetAddress(obj, fMap_b); break;
					case 'S' : SetAddress(obj, fMap_S); break;
					case 's' : SetAddress(obj, fMap_s); break;
					case 'I' : SetAddress(obj, fMap_I); break;
					case 'i' : SetAddress(obj, fMap_i); break;
					case 'F' : SetAddress(obj, fMap_F); break;
					case 'D' : SetAddress(obj, fMap_D); break;
					case 'L' : SetAddress(obj, fMap_L); break;
					case 'l' : SetAddress(obj, fMap_l); break;
					case 'O' : SetAddress(obj, fMap_O); break;
					default :
						std::cout << "ERROR: Unable to book address for branch \"" << type << "\"" << std::endl;
				}
			}
			if(print_branches) std::cout << std::endl;
	}



// * ==================== * //
// * ------- DRAW ------- * //
// * ==================== * //


	/// Draw a distribution of one of the branches in the file.
		/// @param branchX Branch that you want to plot. You may use a formula.
		/// @param nBinx Number of bins to use on the \f$x\f$-axis.
		/// @param x1 Lower limit on the \f$x\f$-axis.
		/// @param x2 Upper limit on the \f$x\f$-axis.
		/// @param save Set to `false` if you do not want to save the histogram that has been drawn.
		/// @param option Draw options.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
		/// @param cut Fill in a cut according to the syntax described <a href="https://root.cern.ch/doc/master/classTTree.html#a73450649dc6e54b5b94516c468523e45">here</a>.
	TH1F* ChainLoader::Draw(const char* branchX, const Int_t nBinx, const double x1, const double x2, Option_t *option, const bool save, const TString &logScale, const char* cut)
	{
		// * Draw histogram * //
		const char* histName = Form("hist_%s", branchX);
		Draw(Form("%s>>%s(%i,%f,%f)", branchX, histName, nBinx, x1, x2), cut, option, save, logScale);
		// * Modify histogram * //
		TH1F *hist = (TH1F*)gDirectory->Get(histName);
		hist->SetTitle(Form("#it{%s} branch of the \"%s\" tree", branchX, fChain.GetName()));
		if(!logScale.Contains("y") && !logScale.Contains("Y")) {// NOTE: cannot set y log scale if min value is 0
			auto maxbin = hist->GetMaximumBin();
			hist->GetYaxis()->SetRangeUser(0., hist->GetBinContent(maxbin)+1.5*hist->GetBinErrorUp(maxbin));
		}
		Hist::SetAxisTitles(hist, branchX, Form("count / %g", hist->GetBinWidth(1)));
		// * Save and return histogram * //
		if(save) Draw::SaveCanvas(Form("%s/%s", fChain.GetName(), branchX), gPad, logScale);
		return hist;
	}


	/// Draw a distribution of one of the branches in the file.
		/// @param varexp Names of the branches that you want to plot. See https://root.cern.ch/doc/master/classTTree.html#a8a2b55624f48451d7ab0fc3c70bfe8d7 for how this works.
		/// @param save Set to `false` if you do not want to save the histogram that has been drawn.
		/// @param option Draw options.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
		/// @param cut Fill in a cut according to the syntax described <a href="https://root.cern.ch/doc/master/classTTree.html#a73450649dc6e54b5b94516c468523e45">here</a>.
	TH1* ChainLoader::Draw(const char* varexp, const char* cut, Option_t *option, const bool save, const TString& logScale)
	{
		/// <ol>
		/// <li> Check if `TChain` contains entries.
		if(!fChain.GetEntries()) return nullptr;
		/// <li> Remove possible output options from `varexp` parameter.
		TString outputName{varexp};
		TString histName  {varexp};
		if(outputName.Contains(">>")) {
			auto firstPipe{outputName.First('>')};
			outputName.Resize(firstPipe);
			histName.Remove(0, firstPipe+2);
			histName.Strip(TString::kBoth, ' ');
		} else {
			TTimeStamp stamp;
			histName = Form("hist_%d", stamp.GetNanoSec());
			outputName += Form(">>%s", histName.Data());
		}
		/// <li> Draw histogram and save
		fChain.Draw(varexp, cut, option);
		auto hist = dynamic_cast<TH1*>(gDirectory->Get(histName.Data()));
		if(save) Draw::SaveCanvas(Form("%s/%s", fChain.GetName(), outputName.Data()), gPad, logScale);
		/// </ol>
		return hist;
	}


	/// Draw a distribution of one of the branches in the file.
		/// @param branchX Branch that you want to plot on the \f$x\f$-axis. You may use a formula.
		/// @param branchY Branch that you want to plot on the \f$y\f$-axis. You may use a formula.
		/// @param nBinx Number of bins to use on the \f$x\f$-axis.
		/// @param x1 Lower limit on the \f$x\f$-axis.
		/// @param x2 Upper limit on the \f$x\f$-axis.
		/// @param nBiny Number of bins to use on the \f$y\f$-axis.
		/// @param y1 Lower limit on the \f$y\f$-axis.
		/// @param y2 Upper limit on the \f$y\f$-axis.
		/// @param save Set to `false` if you do not want to save the histogram that has been drawn.
		/// @param option Draw options.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
		/// @param cut Fill in a cut according to the syntax described <a href="https://root.cern.ch/doc/master/classTTree.html#a73450649dc6e54b5b94516c468523e45">here</a>.
	TH2F* ChainLoader::Draw(const char* branchX, const char* branchY, const Int_t nBinx, const double x1, const double x2, const Int_t nBiny, const double y1, const double y2, Option_t *option, const bool save, const TString &logScale, const char* cut)
	{
		// * Draw histogram * //
		const char* histName = Form("hist_%s_%s", branchX, branchY);
		Draw(Form("%s:%s>>%s(%i,%f,%f,%i,%f,%f)", branchX, branchY, histName, nBinx, x1, x2, nBiny, y1, y2), cut, option, save, logScale);
		// * Modify histogram * //
		TH2F *hist = (TH2F*)gDirectory->Get(histName);
		hist->SetTitle(Form("\"%s\" tree: #it{%s} vs #it{%s}", fChain.GetName(), branchX, branchY));
		Hist::SetAxisTitles(
			hist, branchX, branchY,
			Form("count / (%gx%g)",
				hist->GetXaxis()->GetBinWidth(1),
				hist->GetYaxis()->GetBinWidth(1)
			));
		// * Save and return histogram * //
		if(save) Draw::SaveCanvas(Form("%s/%s:%s", fChain.GetName(), branchY, branchX), gPad, logScale);
		return hist;
	}


	/// Draw a distribution of one of the branches in the file.
	TH1* ChainLoader::Draw(const BranchPlotOptions &options)
	{
		/// -# **Abort** if `TChain` contains entries.
			if(!fChain.GetEntries()) {
				TerminalIO::PrintWarning(Form("Chain \"%s\" has no entries", fChain.GetName()));
				return nullptr;
			}
		/// -# Draw histogram and save
			fChain.Draw(options.VarExp(), options.CutSelection(), options.DrawOption());
			auto hist = dynamic_cast<TH1*>(gDirectory->Get(options.BuildHistName().c_str()));
			if(options.IsWrite()) Draw::SaveCanvas(Form("%s/%s", fChain.GetName(), options.OutputFileName()), gPad, options.LogXYZ());
			return hist;
	}


	/// Draw the distributions of all branches of the underlying `TChain`.
		/// @param option Draw options.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	void ChainLoader::DrawAndSaveAllBranches(Option_t *option, const TString& logScale)
	{
		if(!fChain.GetNtrees()) return;
		TIter next(fChain.GetListOfBranches());
		TObject *obj  = nullptr;
		while((obj = next())) Draw(obj->GetName(), "", option, true, logScale);
	}


	/// Draw the distributions of all branches of the underlying `TChain` if its name starts with `"mult"`.
	/// This function additionally ensures that the bin width is set to 1 and that the histograms are drawn in `"E1"` mode (see https://root.cern.ch/doc/master/classTHistPainter.html).
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
		/// @param option Draw options.
	void ChainLoader::DrawAndSaveAllMultiplicityBranches(const TString& logScale, Option_t *option)
	{
		if(!fChain.GetNtrees()) return;
		TString name(fChain.GetName());
		if(!name.BeginsWith("mult")) return;
		TIter next(fChain.GetListOfBranches());
		TObject *obj  = nullptr;
		while((obj = next())) {
			const char* name = obj->GetName();
			int max = fChain.GetMaximum(name);
			if(!max) continue;
			Draw(name, max, 0, max, option, true, logScale);
		}
	}



// * =========================== * //
// * ------- INFORMATION ------- * //
// * =========================== * //


	/// Create a histogram object especially for invariant mass analysis.
	TH1F* ChainLoader::GetInvariantMassHistogram(const char* varexp, const ReconstructedParticle& particle, const int nBins, Option_t *option, const TString &logScale)
	{
		// * Check input arguments * //
		if(*varexp != 'm') {
			std::cout << "ERROR: varexp \"" << varexp <<  "\" does not start with 'm'" << std::endl;
			return nullptr;
		}
		// * Get histogram and modify
		auto hist = Draw(varexp, nBins, particle.PlotFrom(), particle.PlotUntil(), option, false, logScale);
		hist->SetTitle(Form("Invariant mass for %s candidate", particle.NameLaTeX()));
		Hist::SetAxisTitles(hist,
			Form("#it{M}_{%s} (GeV/#it{c}^{2})", particle.DaughterLabel()),
			Form("count / %g", hist->GetYaxis()->GetBinWidth(1)));
		return hist;
	}


	/// Create a histogram object based on a `BranchPlotOptions` and `ReconstructedParticle` object.
	TH1F* ChainLoader::GetInvariantMassHistogram(const BranchPlotOptions &branch, const ReconstructedParticle& particle)
	{
		if(!branch.IsOK()) return nullptr;
		if(branch.GetNBranches() != 1) {
			TerminalIO::PrintWarning(Form("Cannot get an invariant mass plot for BranchPlotOptions object\n  \"%s\"\nbecause it has %u branches", branch.VarExp(), branch.GetNBranches()));
			return nullptr;
		}
		// * Check input arguments * //
		if(*branch.VarExp() != 'm') {
			std::cout << "ERROR: varexp \"" << branch.VarExp() <<  "\" does not start with 'm'" << std::endl;
			return nullptr;
		}
		// * Get histogram and modify
		auto hist = dynamic_cast<TH1F*>(Draw(branch));
		gPad->SetLogx(branch.LogX());
		gPad->SetLogy(branch.LogY());
		gPad->SetLogz(branch.LogZ());
		hist->SetTitle(Form("Invariant mass for %s candidate", particle.NameLaTeX()));
		Hist::SetAxisTitles(hist,
			Form("#it{M}_{%s} (GeV/#it{c}^{2})", particle.DaughterLabel()),
			Form("count / %g", hist->GetYaxis()->GetBinWidth(1)));
		return hist;
	}


	/// Compute mean value of a branch in the `TChain`.
		/// @param chain `TChain` that you want to check.
		/// @param branchName Name of the branch that you want to check.
	double ChainLoader::ComputeMean(TChain* chain, const char* branchName)
	{
		if(!chain) return -1e6;
		chain->Draw(branchName);
		TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");
		if(!htemp) return -1e6;
		double mean = htemp->GetMean();
		delete htemp;
		return mean;
	}