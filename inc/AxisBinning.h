#ifndef BOSS_Afterburner_AxisBinning_H
#define BOSS_Afterburner_AxisBinning_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "CommonFunctions.h"
	#include <string>
	#include <utility>



// * ================================= * //
// * ------- CLASS DEFINITIONS ------- * //
// * ================================= * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Container object for the binning for *one* axis.
	/// Only contains the number of bins, the bin width and the plot range (from to).
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     March 23rd, 2019
	class AxisBinning
	{
	public:
		AxisBinning() : fIsOK(false), fNBins(0), fBinWidth(0.) {}
		AxisBinning(const int &nbins, const double &from, const double &to);
		AxisBinning(const double &width, const double &from, const double &to);
		AxisBinning(const std::string &binning);
		void Set(const int &nbins, const double &from, const double &to);
		void Set(const double &width, const double &from, const double &to);
		void Set(const std::string &binning);
		void SetBinWidth(const double &width);
		void SetNBins(const int &nbins);
		void SetRange(const double &from, const double &to);
		void SetFrom(const double &from);
		void SetTo(const double &to);
		const bool IsOK() const { return fIsOK; }
		const int& NBins() const { if(fIsOK) return fNBins; else return 0; }
		const double& From() const { if(fIsOK) return fRange.first; else return 0.; }
		const double& To() const { if(fIsOK) return fRange.second; else return 0.; }
		const double& BinWidth() const { if(fIsOK) return fBinWidth; else return 0.; }
		const double ComputeRange() const { return ComputeRange(fRange.first, fRange.second); }

	private:
		bool fIsOK; ///< Switch that keeps track of whether arguments have been set correctly.
		int fNBins; ///< The number of bins on the axis.
		double fBinWidth; ///< The number of bins on the axis.CheckBinWidth
		std::pair<double, double> fRange; ///< Plot range from-to.
		static constexpr int gMaxNBins = 1e4;
		void Set(const int &nbins, const double &width, const double &from, const double &to);
		const double ComputeRange(const double &from, const double &to) const { return to - from; }
		const bool CheckNBins(const int &nbins) const { return nbins < gMaxNBins && nbins > 0; };
		const bool CheckBinWidth(const double &width, const double & from, const double &to) const { return width > 0 && ComputeNBins(width, from, to) < gMaxNBins; };
		const bool CheckRange(const double &from, const double &to) const { return from < to; }
		const double ComputeBinWidth(const int &nbins, const double &from, const double &to) const { return ComputeRange(from, to) / nbins; }
		const int ComputeNBins(const double &width, const double &from, const double &to) const { return ComputeRange(from, to) / width; }
	};



/// @}


#endif