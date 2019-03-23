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
			fLogY         ("Use y log scale",       false),
			fLogZ         ("Use z log scale",       false),
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
			fDo_conv_d    ("Do double convolution", false),
fTestVectorArg("Test vector")
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
	ConfigParBase* ConfigLoader::ExtractParameter(std::string input)
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
		if(!par) return;
		String::Trim(input);
		if(input.front() == '{') input.erase(0, 1);
		if(input.back () == '}') input.pop_back();
		String::Trim(input);
		if(!input.size()) return;
		do {
			std::string value(input);
			if(input.front() == '\"') {
				input.erase(0, 1);
				value.erase(0, 1);
				if(input.find_first_of('\"') == input.size()) input.clear();
				else input = input.substr(input.find_first_of('\"')+1);
				value.resize(value.find_first_of('\"'));
				String::Trim(input, ',');
				String::Trim(input);
				String::Trim(value, '\"');
			} else {
				if(input.find(',') != std::string::npos) {
					value.resize(value.find_first_of(','));
					input = input.substr(input.find_first_of(',')+1);
				} else {
					input.clear();
				}
			}
			AddValue(par, value);
			String::Trim(input);
		} while(input.size());
	}


	/// Format 
	/// The `input` line has to be formatted, that is, it may not contain the initial opening or final closing bracket.
	void ConfigLoader::AddValue(ConfigParBase *par, std::string &val)
	{
		if(!par) return;
		String::Trim(val);
		String::Trim(val, '\"');
		if(!val.size()) return;
		par->AddValue(val);
	}


	/// Load a configuration for analysis from a <i>BOSS Afterburner</i> configuration file.
	size_t ConfigLoader::LoadConfiguration(const std::string &filename)
	{
		/// -# Create file stream (`std::ifstream`) of config `txt` file.
		std::ifstream file(filename);
		if(!file.is_open()) {
			std::cout << "WARNING: Could not load configuration file \"" << filename << "\"" << std::endl;
			return 0;
		}
		/// -# Print configuration title.
		std::cout << std::endl << "LOADING CONFIGURATION FROM \"" << filename << "\"" << std::endl;
		/// -# Loop over lines.
		std::string line;
		while(getline(file, line)) {
			/// <ul>
			/// <li> Remove weird characters like EOF.
				if(line.back()<' ') line.pop_back();
			/// <li> Remove leading spaces and tabs.
				String::Trim(line);
				String::Trim(line, ';');
			/// <li> Skip line if it is a comment or if it is empty.
				if(!line.size()) continue;
				if(String::IsComment(line)) continue;
			/// <li> Now, attempt to extract the paramater identifier.
				ConfigParBase *par = nullptr;
				std::string parname{line};
				std::string parval {line};
			/// <li> If the `line` does **not** contain brackets opening (`{`) or closing (``}) bracket, ends in an equal sign **and** does contain an equal sign, **there is only one value for this parameter**. This is a simple case and we can move to the next line.
					if(
						line.find('=') != std::string::npos &&
						line.find('{') == std::string::npos &&
						line.find('}') == std::string::npos &&
						line.back() != '='
					) {
						// * Attempt to get parameter
						parname.resize(parname.find_first_of('='));
						String::Trim(parname);
						par = ConfigParBase::GetCleanParameter(parname);
						if(!par) continue;
						// * Add the value to the parameter
						parval = parval.substr(parval.find_first_of('=')+1);
						AddValue(par, parval);
						continue;
					}
			/// <li> If this is not the case, the parameter contains **multiple values**:
				/// <ol>
				/// <li> If the line contains `=`, `{`, and `}`, all parameter values are defined on this line and we can round up without reading the next line.
					if(
						(line.find('=') != std::string::npos) &&
						(line.find('{') != std::string::npos) &&
						(line.find('}') != std::string::npos)) {
						// * Attempt to get parameter
						parname.resize(parname.find_first_of('='));
						String::Trim(parname);
						par = ConfigParBase::GetCleanParameter(parname);
						if(!par) continue;
						// * Add the value to the parameter
						parval = parval.substr(parval.find_first_of('{')+1);
						ImportValues(par, parval);
						continue;
					}
				/// <li> If the line does not contain an equal sign (`=`), it means the line only contains the parameter name and that we should continue reading the next lines.
					if(line.find('=') == std::string::npos) {
						par = ConfigParBase::GetCleanParameter(parname);
						if(!par) continue;
				/// <li> Else, get the parameter, read the first values on the line and continue to the next.
					} else {
						parname.resize(parname.find_first_of('='));
						String::Trim(parname);
						par = ConfigParBase::GetCleanParameter(parname);
						if(!par) continue;
						parval = parval.substr(parval.find_first_of('=')+1);
						ImportValues(par, parval);
					}
				/// </ol>
			/// <li> Import the remaining values by continuing over the next lines until the first closing bracket (`}`) is encountered.
				while(getline(file, line)) {
					/// <ol>
					/// <li> Remove weird characters like EOF.
						if(line.back()<' ') line.pop_back();
					/// <li> `Trim` line and remove opening equal sign (`=`).
						String::Trim(line); if(line.front() == '=') line.erase(0, 1);
						String::Trim(line); if(line.front() == '{') line.erase(0, 1);
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
		/// <li> Set all parameter values from the read strings
			auto mapping = ConfigParBase::GetMapOfParameters();
			for(auto &it : *mapping) it.second->ConvertStringsToValue();
		/// <li> Print loaded values in table form.
		ConfigParBase::PrintAll();
		/// @return Number of valid loaded arguments
		return ConfigParBase::GetNParameters();
	}