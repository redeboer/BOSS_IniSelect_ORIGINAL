#ifndef BOSS_Afterburner_ConfigLoader_H
#define BOSS_Afterburner_ConfigLoader_H


// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "ArgPair.h"
	#include "BranchPlotOptions.h"
	#include "CommonFunctions.h"
	#include <fstream>
	#include <list>
	#include <string>



// * ================================= * //
// * ------- CLASS DEFINITIONS ------- * //
// * ================================= * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Container object for a <i>BOSS Afterburner</i> analysis configuration file.
	/// Give this object a path to the configuration text file for the analysis you want to perform, and all settings will be automatically loaded from this file. The syntax for this file is of course determined by this object.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     January 24th, 2018
	class ConfigLoader
	{
	public:
		/// @name Constructors
			///@{
			ConfigLoader(const char* path);
			///@}

		/// @name Getters for configurations
			/// <b>Interface is defined here!</b>
			///@{
			const char* InputFilename() { return fInputFilename.value.data(); };
			std::list<BranchPlotOptions>& BranchesToPlot();
			///@}

		/// @name Setters
			///@{
			int LoadConfiguration(const char* filename);
			///@}

	// private:
		/// @name IO parameters
			///@{
			std::string fConfigPath;
			ArgPair<std::string> fInputFilename;
			///@}

		/// @name Do-or-don't switches
			///@{
			ArgPair<bool> fPrintBranches;     ///< Whether or not to print all branch names.
			ArgPair<bool> fPrintAverages;     ///< Whether or not to print the averages for all branches. This could be useful when testing whether the branches have been filled correctly. Note that this could increase run time significantly in case of a large data set!
			ArgPair<bool> fFitplots;  ///< Whether or not to produce invariant mass fits.
			ArgPair<bool> fPlotstats; ///< Whether or not to draw the legend in the upper right corner with histogram statistics.
			///@}

		/// @name Draw options
			///@{
			ArgPair<bool> fDraw_mctruth; ///< Whether or not to draw the MC truth parameters.
			ArgPair<bool> fDraw_mult; ///< Whether or not to draw the multiplicity branches.
			ArgPair<bool> fDraw_tof; ///< Whether or not to draw the `"tof*"` branches.
			ArgPair<bool> fDraw_vertex; ///< Whether or not to draw the `"vertex"` branch.
			ArgPair<bool> fPureplot; ///< Whether or not to plot histograms of branches <i>without fit</i>.
			ArgPair<bool> fSetranges; ///< Whether or not to precisely set histogram ranges.
			ArgPair<std::string> fLogY; ///< Whether to draw the \f$y\f$ axis of the `TH1F` in log scale.
			ArgPair<std::string> fLogZ; ///< Whether to draw the \f$z\f$ axis of the `TH2F` in log scale.
			///@}

		/// @name Fit options
			///@{
			ArgPair<bool> fDraw_fit;  ///< Whether or not to draw the `"fit"` branches.
			ArgPair<bool> fDo_conv_d; ///< Whether or not to produce perform a Breit-Wigner convoluted with a <i>double</i> Gaussian.
			ArgPair<bool> fDo_conv_s; ///< Whether or not to produce perform a Breit-Wigner convoluted with a <i>single</i> Gaussian.
			ArgPair<bool> fDo_gauss;  ///< Whether or not to produce perform a double Gaussian fit.
			///@}

		/// @name Helpers for configuration loading
			///@{
			std::string GetParameterName(std::string line);
			std::string GetParameterValue(std::string line);
			///@}

		/// @name String modulation
			///@{
			void RemoveLeading(std::string &line, const char c=' ');
			void RemoveTrailing(std::string &line, const char c=' ');
			void Trim(std::string &line, const char c=' ');
			///@}

		/// @name @b TEMPORARY
			///@{
			void DrawDifference(TH1 *histToDraw, TH1 *histToSubtract, Option_t* opt="E1", const char* setLog="");
			///@}

	};


/// @}



// * =========================== * //
// * ------- CONSTRUCTORS ------ * //
// * =========================== * //

	/// Constructor that opens a `TFile` and unordered_maps its contents.
	/// @remark <b>You have to set the <i>names</i> and <i>default values</i> of the `ArgPair`s here!</b>
	ConfigLoader::ConfigLoader(const char* path) :
		fConfigPath(path),
		// ! Set your argument names and default values here ! //
		fInputFilename("Input file or directory", "NOFILE"),
		fPrintBranches("Print branches",          false),
		fPrintAverages("Print averages",          false),
		fLogY         ("Use y log scale",         "y"),
		fLogZ         ("Use z log scale",         "z"),
		fSetranges    ("Set plot ranges",         true),
		fPlotstats    ("Plot statistics",         false),
		fPureplot     ("Plot raw data",           true),
		fDraw_mctruth ("Draw mctruth",            false),
		fDraw_mult    ("Draw multiplicites",      false),
		fDraw_vertex  ("Draw vertex",             false),
		fDraw_tof     ("Draw ToF",                false),
		fDraw_fit     ("Draw fit branches",       true),
		fFitplots     ("Perform fits",            false),
		fDo_gauss     ("Do Gaussian",             true),
		fDo_conv_s    ("Do single convolution",   false),
		fDo_conv_d    ("Do double convolution",   false)
	{
		ArgPair_base::LoadConfiguration(path);
	}