// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ConfigLoader.h"
	#include "CommonFunctions.h"
	#include "FrameworkSettings.h"
	using namespace CommonFunctions;



// * =========================== * //
// * ------- CONSTRUCTORS ------ * //
// * =========================== * //

	/// Constructor that opens a `TFile` and unordered_maps its contents.
	/// @remark <b>You have to set the <i>names</i> and <i>default values</i> of the `ArgPair`s here!</b>
	ConfigLoader::ConfigLoader(const std::string &path) :
		/// * Construct normal data members
			fConfigPath(path),
		/// * Construct `ArgPair` objects of type `string`
			fInputFilename("Input file or directory", "NOFILE"),
			fLogY         ("Use y log scale",       "y"),
			fLogZ         ("Use z log scale",       "z"),
		/// * Construct `ArgPair` objects that serve as a switch (`bool`)
			fPrintBranches("Print branches",        false),
			fPrintAverages("Print averages",        false),
			fSetranges    ("Set plot ranges",       true),
			fPlotstats    ("Plot statistics",       false),
			fPureplot     ("Plot raw data",         true),
			fDraw_mctruth ("Draw mctruth",          false),
			fDraw_mult    ("Draw multiplicites",    false),
			fDraw_vertex  ("Draw vertex",           false),
			fDraw_tof     ("Draw ToF",              false),
			fDraw_fit     ("Draw fit branches",     true),
			fFitplots     ("Perform fits",          false),
			fDo_gauss     ("Do Gaussian",           true),
			fDo_conv_s    ("Do single convolution", false),
			fDo_conv_d    ("Do double convolution", false)
	{
		LoadConfiguration(path);
	}



// * ====================== * //
// * ------- SETTERS ------ * //
// * ====================== * //


	/// Attempt to get a parameter @b identifier from a line.
	std::string ConfigLoader::GetParameterIdentifier(std::string line)
	{
		/// -# If `line` contains an equal sign, remove everything after it.
		if(line.find('=') != std::string::npos) line.resize(line.find_first_of('='));
		/// -# If not, remove everything after first space.
		else if(line.find(' ') != std::string::npos) line.resize(line.find_first_of(' '));
		/// -# Remove trailing spaces and tabs.
		RemoveTrailing(line);
		RemoveTrailing(line, '\t');
		/// @return Parameter name as deduced from input `line`
		return line;
	}


	/// Attempt to get a parameter @b value from a line.
	std::string ConfigLoader::GetParameterValue(std::string line)
	{
		/// -# If `line` contains equal sign, get everything before it.
		if(line.find('=') != std::string::npos) line = line.substr(line.find_first_of('=')+1);
		/// -# If not, remove everything before first space.
		else if(line.find(' ') != std::string::npos) line = line.substr(line.find_first_of(' ')+1);
		/// -# Remove leading spaces and tabs.
		RemoveLeading(line);
		RemoveLeading(line, '\t');
		/// -# Remove quotation marks.
		if(!line.compare("\"\"")) line = ""; // if empty std::string
		else Trim(line, 34); // 34 is ASCII for double quoation mark "
		/// @return Parameter value as infered from line
		return line;
	}

	/// *TEMPORARY* function that serves as a fix for the bug that causes the wrong best pair to be stored.
	void ConfigLoader::DrawDifference(TH1 *histToDraw, TH1 *histToSubtract, Option_t* opt, const char* setLog)
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


// * ============================ * //
// * ------- READ HANDLERS ------ * //
// * ============================ * //

	/// Load a configuration for analysis from a <i>BOSS Afterburner</i> configuration file.
	size_t ArgPair_base::LoadConfiguration(const std::string &filename)
	{
		/// -# Create file stream (`std::ifstream`) of config `txt` file.
		std::ifstream file(filename);
		if(!file.is_open()) {
			std::cout << "WARNING: Could not load configuration file \"" << filename << "\"" << std::endl;
			return -1;
		}
		/// -# Print configuration title.
		std::cout << std::endl << "LOADING CONFIGURATION FROM \"" << filename << "\"" << std::endl;
		/// -# Loop over lines.
		std::string line;
		while(getline(file, line)) {
			/// <ul>
			/// <li> Remove weird characters like EOF.
				if(line.back()<' ') line.pop_back();
			/// <li> Skip line if it does <i>not</i> contain spaces or an equal sign.
				if(line.find('=') == std::string::npos && line.find(' ') == std::string::npos) continue;
			/// <li> Remove leading spaces and tabs.
				String::RemoveLeading(line);
				String::RemoveLeading(line, '\t');
			/// <li> Skip line if it is a comment.
				std::string beginning = line.substr(0,2);
				if(!beginning.compare("//")) continue; // C++ line comment
				if(!beginning.compare("/*")) continue; // C++ begin block comment
				if(beginning.front() == '*') continue; // C++ continue/end block comment
				if(beginning.front() == '#') continue; // bash comment
				if(beginning.front() == '%') continue; // LaTeX comment
			/// <li> Get paramter name and value.
				std::string parname  { GetParameterName (line) };
				std::string parvalue { GetParameterValue(line) };
			/// <li> Load value if it compares to one of the parameters.
				ArgPair_base::SetParameters(parname, parvalue);
			/// </ul>
		}
		/// -# Print loaded values in table form.
		ArgPair_base::PrintAll();
		/// @return Number of valid loaded arguments
		return ArgPair_base::instances.size();
	}