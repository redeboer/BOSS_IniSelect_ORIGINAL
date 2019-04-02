// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "FitPars.h"
	#include "CommonFunctions.h"
	#include <iostream>
	#include <iomanip>
	using namespace CommonFunctions;



// * =========================== * //
// * ------- CONSTRUCTORS ------ * //
// * =========================== * //

	/// Construct the binnings from **number of bins**.
	FitPars::FitPars(const TString &input_sig, const TString &input_bkg)
	{
		Set(input_sig, input_bkg);
	}



// * ====================== * //
// * ------- SETTERS ------ * //
// * ====================== * //


	/// Private helper function to set all values at once.
	/// Values are comma-separaated
	void FitPars::Set(const TString &input_sig, const TString &input_bkg)
	{
		Set(fBkgPars, input_bkg);
		Set(fSigPars, input_sig);
	}



// * ========================== * //
// * ------- INFORMATION ------ * //
// * ========================== * //


	/// Print all loaded values.
	void FitPars::Print() const
	{
		/// -# Print empty output and **abort** if the vector of parameters (`fPars`) is empty.
			if(!fSigPars.size() && !fBkgPars.size()) {
				TerminalIO::PrintWarning("No values loaded to this FitPars object");
				return;
			}
		/// -# Print numbered list of **signal** parameters.
			std::cout << "Signal parameters" << std::endl;
			for(auto i = 0; i < fSigPars.size(); ++i)
				std::cout << std::setw(3) << i << ": " << fSigPars[i] << std::endl;
		/// -# Print numbered list of **background** parameters.
			std::cout << "Background parameters" << std::endl;
			for(auto i = 0; i < fBkgPars.size(); ++i)
				std::cout << std::setw(3) << i << ": " << fBkgPars[i] << std::endl;
		std::cout << std::endl;
	}



// * ============================== * //
// * ------- PRIVATE HELPERS ------ * //
// * ============================== * //


	/// Private helper function for public function `Set`.
	void FitPars::Set(std::vector<double> &vec, TString input)
	{
		if(input.EqualTo("")) return;
		vec.clear();
		vec.resize(input.CountChar(',')+1);
		input.ReplaceAll(",", ", ");
		TString tok;
		Ssiz_t start{0};
		size_t i{0};
		while(input.Tokenize(tok, start, ",")) {
			vec[i] = tok.Atof();
			++i;
		}
	}