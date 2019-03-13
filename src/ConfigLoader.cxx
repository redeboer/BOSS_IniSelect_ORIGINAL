// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ConfigLoader.h"
	#include "CommonFunctions.h"
	#include "FrameworkSettings.h"
	using namespace CommonFunctions;



// * ====================== * //
// * ------- SETTERS ------ * //
// * ====================== * //

	/// Attempt to get a parameter @b name from a line.
	std::string ConfigLoader::GetParameterName(std::string line)
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



// * ====================== * //
// * ------- SETTERS ------ * //
// * ====================== * //

	/// Remove all leading characters if they are 'c'.
	/// @param line String from which you want to remove the characters.
	/// @param c Character that you want to trim.
	void ConfigLoader::RemoveLeading(std::string &line, const char c)
	{
		if(line.front() == c) line = line.substr(line.find_first_not_of(c));
	}

	/// Remove all trailing characters if they are 'c'.
	/// @param line String from which you want to remove the characters.
	/// @param c Character that you want to trim.
	void ConfigLoader::RemoveTrailing(std::string &line, const char c)
	{
		if(line.back() == c) line.resize(line.find_last_not_of(c)+1);
	}

	/// Remove all leading and trailing characters if they are 'c'.
	/// @param line String from which you want to remove the characters.
	/// @param c Character that you want to trim.
	void ConfigLoader::Trim(std::string &line, const char c)
	{
		RemoveLeading (line, c);
		RemoveTrailing(line, c);
	}