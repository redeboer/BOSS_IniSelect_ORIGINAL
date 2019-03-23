#ifndef BOSS_Afterburner_ConfigLoader_H
#define BOSS_Afterburner_ConfigLoader_H


// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "AxisBinning.h"
	#include "CommonFunctions.h"
	#include "ConfigParameter.h"
	#include "BranchPlotOptions.h"
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
		///
		/// The configuration (`.config`) files of the *BOSS Afterburner* final selection framework can be used to load settings for your analysis during runtime from a configuration text file. This allows you to quickly modify plots without having to recompile your analysis script: you just change some of the parameters in your config file. The types of parameters (their identifiers and value types) are defined in the `ConfigLoader` class.
		///
		/// The `ConfigParameter` classes (base `ConfigParBase` and derived classes such as `ConfigParameter`) represent these certain types of parameters. The `ConfigParBase` class defines the most abstract form of a configuration parameter. It just has an @b identifier (or parameter name) and a @b value or set of values. Both are strings, as they are the raw data read from the data file. A line in your configuration file can for instance look like this:
		/// ```
		/// 	Plot raw data = true
		/// ```
		/// Here, the line `"Plot raw data"` is the parameter @b identifier, while `"true"` is the @b value. The line will only be read if there is an `ConfigParBase` object (or derived object with the name `"Plot raw data"`.) The value is in `string` format, but in this case you will want the value to be a boolean. For this you should define the parameter in the `ConfigLoader` in terms of the `ConfigParameter` class.
		///
		/// Another possibility would be to have the lines:
		/// ```
		/// 	Draw branches = {
		/// 		do something; this is OK;
		/// 		mctruth, E, 150, 0.,
		/// 		3.5, "E1"
		/// 		plo, 1, ;
		///			; empty:zero}
		/// 		
		/// ```
		/// Note this is a lazily formatted vector. What `ConfigParBase` does in this case is, it will read these lines as an arreay
		/// In this case, what will be read to the `list` of read values (`fReadStrings`) for the parameter with name `"Draw branches"` is:
		/// ```
		/// 	do something
		/// 	this is OK
		/// 	mctruth, E, 150, 0.,3.5, "E1"
		/// 	plo, 1,
		/// 	empty:zero
		/// ```
		/// The contents of this example are not that useful, but you get the idea:
		/// 	-# a parameter will contain more than one value if its identifier is followed by an opening curly brace (`{`)
		/// 	-# its values are separated by a semicolumn (`;`).
		/// What you want to do with these read values is up to use to define in a derived class.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     January 24th, 2018
	class ConfigLoader
	{
	public:
		ConfigLoader(const std::string &path);

		ConfigParameter<std::string> fInputFilename{"Input file or directory", "NOFILE"};

		ConfigParameter<bool> fDo_conv_d{"Do double convolution", false}; ///< Whether or not to produce perform a Breit-Wigner convoluted with a <i>double</i> Gaussian.
		ConfigParameter<bool> fDo_conv_s{"Do single convolution", false}; ///< Whether or not to produce perform a Breit-Wigner convoluted with a <i>single</i> Gaussian.
		ConfigParameter<bool> fDo_gauss {"Do Gaussian",           true};  ///< Whether or not to produce perform a double Gaussian fit.

		ConfigParameter<bool> fDraw_fit    {"Draw fit branches",  true};  ///< Whether or not to draw the `"fit"` branches.
		ConfigParameter<bool> fDraw_mctruth{"Draw mctruth",       false}; ///< Whether or not to draw the MC truth parameters.
		ConfigParameter<bool> fDraw_mult   {"Draw multiplicites", false}; ///< Whether or not to draw the multiplicity branches.
		ConfigParameter<bool> fDraw_tof    {"Draw ToF",           false}; ///< Whether or not to draw the `"tof*"` branches.
		ConfigParameter<bool> fDraw_vertex {"Draw vertex",        false}; ///< Whether or not to draw the `"vertex"` branch.

		ConfigParameter<bool> fFitplots     {"Perform fits",    false}; ///< Whether or not to produce invariant mass fits.
		ConfigParameter<bool> fLogY         {"Use y log scale", false}; ///< Whether to draw the \f$y\f$ axis of the `TH1F` in log scale.
		ConfigParameter<bool> fLogZ         {"Use z log scale", false}; ///< Whether to draw the \f$z\f$ axis of the `TH2F` in log scale.
		ConfigParameter<bool> fPlotstats    {"Plot statistics", false}; ///< Whether or not to draw the legend in the upper right corner with histogram statistics.
		ConfigParameter<bool> fPrintAverages{"Print averages",  false}; ///< Whether or not to print the averages for all branches. This could be useful when testing whether the branches have been filled correctly. Note that this could increase run time significantly in case of a large data set!
		ConfigParameter<bool> fPrintBranches{"Print branches",  false}; ///< Whether or not to print all branch names.
		ConfigParameter<bool> fPureplot     {"Plot raw data",   true};  ///< Whether or not to plot histograms of branches <i>without fit</i>.
		ConfigParameter<bool> fSetranges    {"Set plot ranges", true};  ///< Whether or not to precisely set histogram ranges.

		ConfigParameter<AxisBinning> fTestVectorArg{"Test vector"};
		ConfigParameter<std::list<BranchPlotOptions> > fListOfbranches {"Print these branches"};

	private:
		const std::string fConfigPath;

		size_t LoadConfiguration(const std::string &path);

		ConfigParBase* ExtractParameter(std::string line);
		void ImportValues(ConfigParBase *par, std::string line);
		void AddValue(ConfigParBase *par, std::string &line);

		void DrawDifference(TH1 *histToDraw, TH1 *histToSubtract, Option_t* opt="E1", const char* setLog="");

	};



/// @}
#endif