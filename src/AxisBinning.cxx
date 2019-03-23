// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "AxisBinning.h"
	#include "CommonFunctions.h"
	#include "TString.h"
	#include "TList.h"
	#include <sstream>
	using namespace CommonFunctions;



// * =========================== * //
// * ------- CONSTRUCTORS ------ * //
// * =========================== * //

	/// Construct the binnings from **number of bins**.
	AxisBinning::AxisBinning(const int &nbins, const double &from, const double &to) :
		fIsOK(false), fNBins(0), fBinWidth(0.), fRange(0., 0.)
	{
		Set(nbins, from, to);
	}


	/// Construct the binnings from **bin width**.
	AxisBinning::AxisBinning(const double &width, const double &from, const double &to) :
		fIsOK(false), fNBins(0), fBinWidth(0.), fRange(0., 0.)
	{
		Set(width, from, to);
	}


	/// Construct the binnings from **bin width**.
	AxisBinning::AxisBinning(const std::string &binning) :
		fIsOK(false), fNBins(0), fBinWidth(0.), fRange(0., 0.)
	{
		Set(binning);
	}



// * ====================== * //
// * ------- SETTERS ------ * //
// * ====================== * //


	/// Private helper function to set all values at once.
	/// @warning Since `fBinWidth` and `fNBins` should be related
	void AxisBinning::Set(const int &nbins, const double &width, const double &from, const double &to)
	{
		if(!CheckBinWidth(width, from, to)) {
			Error::PrintWarning(Form("Problem with bin width %g: %d bins (from %g to %g)", width, nbins, from, to));
			return;
		}
		if(!CheckNBins(nbins)) {
			Error::PrintWarning(Form("Problem with %d bins: bin width %g (from %g to %g)", nbins, width, from, to));
			return;
		}
		fBinWidth = width;
		fNBins = nbins;
		fRange = { from, to };
		fIsOK = true;
	}


	/// Set the binnings from **number of bins**.
	void AxisBinning::Set(const int &nbins, const double &from, const double &to)
	{
		Set(nbins, ComputeBinWidth(nbins, from, to), from, to);
	}


	/// Set the binnings from **bin width**.
	void AxisBinning::Set(const double &width, const double &from, const double &to)
	{
		Set(ComputeNBins(width, from, to), width, from, to);
	}


	/// Intelligently set binnins from a string.
	void AxisBinning::Set(const std::string &binning)
	{
		/// -# **Abort** if the string does not contain two commas.
			if(!binning.size()) return;
			int ncommas{0};
			for(auto& c : binning) if(c==',') ++ncommas;
			if(ncommas!=2) {
				Error::PrintWarning(Form("Binning string \"%s\" has to contain 2 commas", binning.c_str()));
				return;
			}
		/// -# Set local variables to load from the string.
			int    nbins = 0;
			double width = 0.;
			double from  = 0.;
			double to    = 0.;
		/// -# Import the three arguments from the string.
			TString line(binning.c_str());
			TString tok;
			UChar_t sw = 0;
			Ssiz_t start{0};
			while(line.Tokenize(tok, start, ",")) {
				switch(sw) {
					case 0 :
						if(tok.Contains('.') || tok.Contains('e') || tok.Contains('E')) width = tok.Atof();
						else nbins = tok.Atoi();
						break;
					case 1 : from = tok.Atof(); break;
					case 2 : to   = tok.Atof(); break;
					default :
						Error::PrintFatalError("AxisBinning::Set(const std::string&)");
						std::terminate();
				}
				++sw;
			}
		/// -# Import the three arguments from the string
			if(nbins) Set(nbins, from, to); // if first argument is int
			else      Set(width, from, to); // if first argument is double
	}


	void AxisBinning::SetBinWidth(const double &width)
	{
		Set(ComputeNBins(width, fRange.first, fRange.second), width, fRange.first, fRange.second);
	}


	void AxisBinning::SetNBins(const int &nbins)
	{
		Set(nbins, ComputeBinWidth(nbins, fRange.first, fRange.second), fRange.first, fRange.second);
	}


	void AxisBinning::SetRange(const double &from, const double &to)
	{
		Set(fNBins, fBinWidth, from, to);
	}


	void AxisBinning::SetFrom(const double &from)
	{
		SetRange(from, fRange.second);
	}


	void AxisBinning::SetTo(const double &to)
	{
		SetRange(fRange.first, to);
	}