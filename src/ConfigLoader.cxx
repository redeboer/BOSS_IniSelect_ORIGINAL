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
fInputFilename("Input file or directory"),
fLogY         ("Use y log scale"),
fLogZ         ("Use z log scale"),
fPrintBranches("Print branches"),
fPrintAverages("Print averages"),
fSetranges    ("Set plot ranges"),
fPlotstats    ("Plot statistics"),
fPureplot     ("Plot raw data"),
fDraw_mctruth ("Draw mctruth"),
fDraw_mult    ("Draw multiplicites"),
fDraw_vertex  ("Draw vertex"),
fDraw_tof     ("Draw ToF"),
fDraw_fit     ("Draw fit branches"),
fFitplots     ("Perform fits"),
fDo_gauss     ("Do Gaussian"),
fDo_conv_s    ("Do single convolution"),
fDo_conv_d    ("Do double convolution")
		// /// * Construct `ArgPair` objects of type `string`
		// 	fInputFilename("Input file or directory", "NOFILE"),
		// 	fLogY         ("Use y log scale",       "y"),
		// 	fLogZ         ("Use z log scale",       "z"),
		// /// * Construct `ArgPair` objects that serve as a switch (`bool`)
		// 	fPrintBranches("Print branches",        false),
		// 	fPrintAverages("Print averages",        false),
		// 	fSetranges    ("Set plot ranges",       true),
		// 	fPlotstats    ("Plot statistics",       false),
		// 	fPureplot     ("Plot raw data",         true),
		// 	fDraw_mctruth ("Draw mctruth",          false),
		// 	fDraw_mult    ("Draw multiplicites",    false),
		// 	fDraw_vertex  ("Draw vertex",           false),
		// 	fDraw_tof     ("Draw ToF",              false),
		// 	fDraw_fit     ("Draw fit branches",     true),
		// 	fFitplots     ("Perform fits",          false),
		// 	fDo_gauss     ("Do Gaussian",           true),
		// 	fDo_conv_s    ("Do single convolution", false),
		// 	fDo_conv_d    ("Do double convolution", false)
	{
		LoadConfiguration(path);
	}



// * ====================== * //
// * ------- SETTERS ------ * //
// * ====================== * //


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


	/// Attempt to get a corresponding parameter @b identifier from a line.
	ConfigParBase* ConfigLoader::GetParameter(std::string input)
	{
		/// -# If `input` contains an equal sign, remove everything after it. (You are also allowed to place the equal sign on the next input.)
		if(input.find('=') != std::string::npos) input.resize(input.find_first_of('='));
		/// -# Remove leading and trailing spaces and tabs and double quotation marks (`"`).
		String::Trim(input);
		String::Trim(input, '\"');
		/// @return Parameter name as deduced from input `line`
		return ConfigParBase::GetParameter(input);
	}


	/// Import parameter values from a line.
	/// The `input` line has to be formatted, that is, it may not contain the initial opening or final closing bracket.
	void ConfigLoader::ImportValues(ConfigParBase *par, std::string input)
	{
		String::Trim(input);
		std::string remainder(input);
		do {
			std::string value(remainder);
			if(remainder.front() == '\"') {
				value.resize(value.find_first_of('\"'));
				remainder = remainder.substr(remainder.find_first_not_of('\"')+1);
				String::Trim(value, '\"');
				String::Trim(remainder, ',');
			} else{
				value.resize(value.find_first_of(','));
				remainder = remainder.substr(remainder.find_first_not_of(',')+1);
			}
			String::Trim(value);
			String::Trim(remainder);
			par->AddValue(value);
		} while(remainder.size());
	}


	/// Load a configuration for analysis from a <i>BOSS Afterburner</i> configuration file.
	size_t ConfigLoader::LoadConfiguration(const std::string &filename)
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
				String::Trim(line);
			/// <li> Skip line if it is a comment.
				if(String::IsComment(line)) continue;
				ConfigParBase* par = nullptr;
			/// <li> Now, there are four scenarios:
				/// <ol>
				/// <li> If `line` at this stage does not contain an equal sign, it means that this line only contains a parameter identifier and that its values are defined on the next line.
					if(line.find('=') == std::string::npos) par = ConfigParBase::GetParameter(line);
				/// <li> If the end of `line` is an equal sign or an opening bracket (`{`), it also means that this line only contains a parameter identifier and that its values are defined on the next line.
					else if(line.back() == '=' || line.back() == '{') {
						line.pop_back();
						String::Trim(line);
						par = GetParameter(line);
					}
				/// <li> Otherwise, if in addition there is no opening bracket (`{`), this means this parameter only has one value. We can add it and continue to the next line to process the next parameter.
					if(line.find('{') == std::string::npos) {
						par = GetParameter(line);
						line = line.substr(line.find_first_of('=')+1);
						String::Trim(line);
						par->AddValue(line);
						continue;
					}
				/// <li> In the remaining scenario, this line contains the first value for this parameter and we should continue reading.
					else {
						par = GetParameter(line);
						line = line.substr(line.find_first_of('{')+1);
						String::Trim(line);
						ImportValues(par, line);
					}
				/// </ol>
			/// <li> Import the remaining values by continuing over the next lines until the first closing bracket (`}`) is encountered.
				while(getline(file, line)) {
					/// <ol>
					/// <li> `Trim` line.
						String::Trim(line);
					/// <li> If this line does not contain a closing bracket, simply import all arguments on this line and go to the next.
						if(line.find('}') == std::string::npos) ImportValues(par, line);
					/// <li> If not, we should import the last arguments and then `break` the `while` loop
						else {
							line.resize(line.find_last_of('}'));
							ImportValues(par, line);
							break;
						}
					/// </ol>
				}
			/// </ul>
		}
		/// -# Print loaded values in table form.
		ConfigParBase::PrintAll();
		/// @return Number of valid loaded arguments
		return ConfigParBase::GetNParameters();
	}