#ifndef BOSS_Afterburner_ConfigLoader_H
#define BOSS_Afterburner_ConfigLoader_H


// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "ArgPair.h"
	#include "ArgPair_base.h"
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
		ConfigLoader(const std::string &path);

	private:
		const std::string fConfigPath;

		ArgPair<std::string> fInputFilename;

		ArgPair<bool> fPrintBranches; ///< Whether or not to print all branch names.
		ArgPair<bool> fPrintAverages; ///< Whether or not to print the averages for all branches. This could be useful when testing whether the branches have been filled correctly. Note that this could increase run time significantly in case of a large data set!
		ArgPair<bool> fFitplots;      ///< Whether or not to produce invariant mass fits.
		ArgPair<bool> fPlotstats;     ///< Whether or not to draw the legend in the upper right corner with histogram statistics.

		ArgPair<bool> fDraw_mctruth; ///< Whether or not to draw the MC truth parameters.
		ArgPair<bool> fDraw_mult;    ///< Whether or not to draw the multiplicity branches.
		ArgPair<bool> fDraw_tof;     ///< Whether or not to draw the `"tof*"` branches.
		ArgPair<bool> fDraw_vertex;  ///< Whether or not to draw the `"vertex"` branch.
		ArgPair<bool> fPureplot;     ///< Whether or not to plot histograms of branches <i>without fit</i>.
		ArgPair<bool> fSetranges;    ///< Whether or not to precisely set histogram ranges.
		ArgPair<std::string> fLogY;  ///< Whether to draw the \f$y\f$ axis of the `TH1F` in log scale.
		ArgPair<std::string> fLogZ;  ///< Whether to draw the \f$z\f$ axis of the `TH2F` in log scale.

		ArgPair<bool> fDraw_fit;  ///< Whether or not to draw the `"fit"` branches.
		ArgPair<bool> fDo_conv_d; ///< Whether or not to produce perform a Breit-Wigner convoluted with a <i>double</i> Gaussian.
		ArgPair<bool> fDo_conv_s; ///< Whether or not to produce perform a Breit-Wigner convoluted with a <i>single</i> Gaussian.
		ArgPair<bool> fDo_gauss;  ///< Whether or not to produce perform a double Gaussian fit.

		size_t LoadConfiguration(const std::string &path);

		std::string GetParameterIdentifier(std::string line);
		std::string GetParameterValue(std::string line);

		bool ImportValue(std::string line);
		bool ImportFirstValue(std::string line);
		bool ImportNextValue(std::string line);
		void ImportSingleValue(std::string line);

		void DrawDifference(TH1 *histToDraw, TH1 *histToSubtract, Option_t* opt="E1", const char* setLog="");

	};



/// @}
#endif