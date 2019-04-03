// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "CommonFunctions.h"
	#include "TSystem.h"
	#include <stdio.h>
	#include <utility> // for shared pointers
	#include <vector>
	#include <cmath>



// * ================================== * //
// * ------- SUB-NAMESPACE DRAW ------- * //
// * ================================== * //


	/// Auxiliary function that is used by the more specific `DrawAndSave` functions for `TH1D` and `TH2D`.
		/// @param hist Histogram that you want to draw and save.
		/// @param saveas Filename that the output file name should have. See `CommonFunctions::CommonFunctions::File::SetOutputFilename` for more information.
		/// @param opt Draw options.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	void CommonFunctions::Draw::DrawAndSave(TH1 &hist, const char* saveas, Option_t* opt, TString logScale)
	{
		hist.Draw(opt);
		SaveCanvas(saveas, gPad, logScale);
	}


	/// Draw and save a 1D distribution (output folder is determined from `FrameworkSettings.h`).
		/// @param hist One-dimenational histogram that you would like to draw and save.
		/// @param saveas Filename that the output file name should have. See `CommonFunctions::CommonFunctions::File::SetOutputFilename` for more information.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	void CommonFunctions::Draw::DrawAndSave(TH1D &hist, const char* saveas, TString logScale)
	{
		DrawAndSave(hist, saveas, "ep", logScale);
	}


	/// Draw and save a 2D distribution (output folder is determined from `FrameworkSettings.h`).
		/// @param hist Two-dimenational histogram that you would like to draw and save.
		/// @param saveas Filename that the output file name should have. See `CommonFunctions::CommonFunctions::File::SetOutputFilename` for more information.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	void CommonFunctions::Draw::DrawAndSave(TH2D &hist, const char* saveas, TString logScale)
	{
		DrawAndSave(hist, saveas, "colz", logScale);
	}


	/// Function that allows you to create and save a quick sketch of a `TTree` branch.
		/// @param tree The `TTree` of which you want to draw and save a branch.
		/// @param varexp The branches that you want to plot should be in this parameter. See https://root.cern.ch/doc/master/classTTree.html#a8a2b55624f48451d7ab0fc3c70bfe8d7 for the syntax.
		/// @param selection Cuts on the branch. See https://root.cern.ch/doc/master/classTTree.html#a8a2b55624f48451d7ab0fc3c70bfe8d7 for the syntax.
		/// @param opt Draw options.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	void CommonFunctions::Draw::DrawAndSave(TTree* tree, const char* varexp, const char* selection, Option_t* opt, TString logScale)
	{
		tree->Draw(varexp, selection, opt);
		SaveCanvas(Form("%s_%s", tree->GetName(), varexp), gPad, logScale);
	}


	/// Helper function that allows you to save a `TPad` or `TCanvas`.
		/// @param pad Pointer to the `TPad` that you want to save. **Can also be a `TCanvas`.**
		/// @param saveas Filename that the output file name should have. See `CommonFunctions::CommonFunctions::File::SetOutputFilename` for more information.
		/// @param logScale If this argument contains an `'x'`, the \f$x\f$-scale will be set to log scale (same for `'y'` and `'z'`).
	void CommonFunctions::Draw::SaveCanvas(const char *saveas, TVirtualPad *pad, TString logScale)
	{
		if(CommonFunctions::TerminalIO::IsEmptyPtr(pad)) return;
		SetLogScale(logScale, pad);
		pad->SaveAs(CommonFunctions::File::SetOutputFilename(saveas));
	}


	/// *TEMPORARY* function that serves as a fix for the bug that causes the wrong best pair to be stored.
		/// @todo Consider removing `CommonFunctions::Draw::DrawDifference`.
	void CommonFunctions::Draw::DrawDifference(TH1 *histToDraw, TH1 *histToSubtract, Option_t* opt, const char* setLog)
	{
		if(!gPad) return;
		gPad->Clear();
		histToDraw->Scale(-1.);
		histToDraw->Add(histToSubtract);
		histToSubtract->SetLineColor(kWhite);
		// histToSubtract->SetMarkerColor(kWhite);
		TString option(opt);
		if(dynamic_cast<TH1F*>(histToDraw)) {
			histToSubtract->Draw(option.Data());
			option += "";
		}
		histToDraw->Draw(option.Data());
		CommonFunctions::Draw::SaveCanvas(Form("%s", histToDraw->GetName()), gPad, setLog);
	}


	/// Helper function that allows you to set the log scale of a `TPad` or `TCanvas`.
		/// @param logScale Which axes do you want to set? Just mention e.g. `"xy"` if you want to have the x and y axes in logaritmic scale. This opt is not capital sensitive.
		/// @param pad Pointer to the `TPad` of which you want to set the log scale. **Can also be a `TCanvas`.**
	void CommonFunctions::Draw::SetLogScale(TString logScale, TVirtualPad *pad)
	{
		// * Check arguments * //
		if(logScale.EqualTo("")) return;
		if(CommonFunctions::TerminalIO::IsEmptyPtr(pad)) return;
		logScale.ToLower();
		// * Set log scale * //
		pad->SetLogx(0); pad->SetLogy(0); pad->SetLogz(0);
		if(logScale.Contains("x")) pad->SetLogx();
		if(logScale.Contains("y")) pad->SetLogy();
		if(logScale.Contains("z")) pad->SetLogz();
	}



// * ================================== * //
// * ------- SUB-NAMESPACE FILE ------- * //
// * ================================== * //


	/// This function creates a default output file name according to the `FrameworkSettings`. The file name consists of the plots output director, with the extension as subfolder and then the filename you want.
		/// @param filenameWithoutExt Output file name to which `gPlotOutputDir` in `FrameworkSettings.h` will be prepended and **`gExtension` will be appended**.
	/// @return Full path of the resulting output file name.
	const char* CommonFunctions::File::GetOutputFilename(const char* filenameWithoutExt)
	{
		return Form("%s/%s/%s.%s", Settings::File::gPlotOutputDir, Settings::File::gExtension, filenameWithoutExt, Settings::File::gExtension);
	}


	/// This function creates a file name from the settings in `FrameworkSettings.h` and from the file name (without extension) that you feed it.
	/// You don't have to worry about creating the output directory, as it is created automatically from the slashes `/` in the resulting file name
		/// @param filenameWithoutExt Output file name to which `gPlotOutputDir` in `FrameworkSettings.h` will be prepended and `gExtension` will be appended.
	/// @return Full path of the resulting output file name.
	const char* CommonFunctions::File::SetOutputFilename(const char* filenameWithoutExt)
	{
		// * Create full path * //
		const char* fullpath = GetOutputFilename(filenameWithoutExt);
		// * Create dirname * //
		TString dirname(fullpath);
		Int_t pos = dirname.Last('/');
		dirname.Resize(pos);
		gSystem->mkdir(dirname.Data(), true); // true: recursively create directory
		// * Get basename * //
		TString basename(fullpath);
		basename.Remove(0, pos+1);
		// * Create file name and return * //
		return Form("%s/%s", dirname.Data(), basename.Data());
	}



// * ================================== * //
// * ------- SUB-NAMESPACE HIST ------- * //
// * ================================== * //

	/// Create a histogram object especially for invariant mass analysis.
		/// @param particle Axis titles, histogram title, and histogram ranges are determined from this object.
		/// @param nBins Number of bins on the \f$x\f$-axis.
	/// @return A new, <i>empty</i> `TH1D` histogram, ready to be filled from a branch from a `TTree`.
	TH1D CommonFunctions::Hist::CreateInvariantMassHistogram(const ReconstructedParticle& particle, const int nBins)
	{
		return TH1D(
			Form("hist_%s", particle.Name()),
			Form("Invariant mass for %s candidate;#it{M}_{%s} (GeV/#it{c}^{2});counts",
				particle.NameLaTeX(), particle.DaughterLabel()),
			nBins, particle.PlotFrom(), particle.PlotUntil());
	}

	/// Set the axis titles of a histogram
		/// @param hist Pointer to the histogram of which you want to change the axis titles.
		/// @param xAxis Title of the \f$x\f$-axis.
		/// @param yAxis Title of the \f$y\f$-axis. Can be left empty.
		/// @param zAxis Title of the \f$z\f$-axis. Can be left empty.
		/// @param update Whether or not to update the `gPad`. Not that this only makes sense if this particular histogram has been recently drawn.
	void CommonFunctions::Hist::SetAxisTitles(TH1* hist, const char* xAxis, const char* yAxis, const char* zAxis, bool update)
	{
		if(CommonFunctions::TerminalIO::IsEmptyPtr(hist)) return;
		if(xAxis) hist->GetXaxis()->SetTitle(xAxis);
		if(yAxis) hist->GetYaxis()->SetTitle(yAxis);
		if(zAxis) hist->GetZaxis()->SetTitle(zAxis);
		if(update && gPad) gPad->Update();
	}



// * ==================================== * //
// * ------- SUB-NAMESPACE STRING ------- * //
// * ==================================== * //


	/// Remove all leading and trailing whitespaces from a string.
		/// @todo This function already exists as such in `TString`. Migrate from `std::string` to `TString`.
	std::string &CommonFunctions::String::Trim(std::string &input)
	{
		while(input.front()==' ' || input.front()=='\t') input.erase(0, 1);
		while(input.back() ==' ' || input.back() =='\t') input.pop_back();
		return input;
	}


	/// Remove all leading and trailing characters of type `c` from a string.
	std::string &CommonFunctions::String::Trim(std::string &input, char c)
	{
		while(input.front()==c) input.erase(0, 1);
		while(input.back() ==c) input.pop_back();
		return input;
	}


	/// Remove all leading and trailing whitespaces from a string.
		/// @todo This function already exists as such in `TString`. Migrate from `std::string` to `TString`.
	TString &CommonFunctions::String::Trim(TString &input)
	{
		while(input.BeginsWith(" ") || input.BeginsWith("\t")) input.Remove(0, 1);
		while(input.EndsWith  (" ") || input.EndsWith  ("\t")) input.Resize(input.Length()-1);
		return input;
	}


	/// Remove all leading and trailing characters of type `c` from a string.
	TString &CommonFunctions::String::Trim(TString &input, char c)
	{
		while(input.BeginsWith(c)) input.Remove(0, 1);
		while(input.EndsWith(Form("%c", c))) input.Resize(input.Length()-1);
		return input;
	}


	/// Check if a string starts with a comment identifier (such as `//` in the case of `C++`).
	const bool CommonFunctions::String::IsComment(std::string input)
	{
		Trim(input);
		input = input.substr(0,2);
		if(!input.compare("//")) return true; // C++ line comment
		if(!input.compare("/*")) return true; // C++ begin block comment
		if(input.front() == '*') return true; // C++ continue/end block comment
		if(input.front() == '#') return true; // bash comment
		if(input.front() == '%') return true; // LaTeX comment
		return false;
	}


	/// Check if a string starts with a comment identifier (such as `//` in the case of `C++`).
	const bool CommonFunctions::String::IsComment(const TString &input)
	{
		return IsComment((std::string)(input.Data()));
	}



// * ========================================== * //
// * ------- SUB-NAMESPACE TERMINAL I/O ------- * //
// * ========================================== * //


	/// Print a message in a certain colour.
		/// @param code An [ANSI escape colour code](https://en.wikipedia.org/wiki/ANSI_escape_code). For instance, `31` would be red.
		/// @param message The message you want to print.
		/// @param qualifier If added, this will print `"<qualifier>: " in bold in front of the message.
	void CommonFunctions::TerminalIO::ColourPrint(const char code, const std::string &message, const std::string &qualifier)
	{
		if(qualifier.size()) printf("\033[1;%dm%s: \033[0m", code, qualifier.c_str());
		printf("\033[%dm%s\033[0m\n", code, message.c_str());
	}


	/// Print a **fatal error** message (that is, one that terminates) in red.
	/// @warning Calling this function will terminate the entire programme.
	void CommonFunctions::TerminalIO::PrintFatalError(const std::string &message)
	{
		ColourPrint(31, message, "FATAL ERROR");
		std::terminate();
	}


	/// Print a **success** message in green.
	void CommonFunctions::TerminalIO::PrintSuccess(const std::string &message)
	{
		ColourPrint(32, message);
	}


	/// Print a **warning** message in yellow.
	void CommonFunctions::TerminalIO::PrintWarning(const std::string &message)
	{
		ColourPrint(33, message, "WARNING");
	}



	/// Auxiliary function that you can used in other functions to check if a pointer that you fed it is a nullpointer.
	/// It is useful to use this function if you want to print output when you are incidentally using an empty pointer.
	bool CommonFunctions::TerminalIO::IsEmptyPtr(void* ptr)
	{
		if(!ptr) {
			std::cout << "ERROR: fed a nullpointer" << std::endl;
			return true;
		}
		return false;
	}