// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ArgPair_base.h"
	#include "CommonFunctions.h"
	#include "FrameworkSettings.h"
	using namespace CommonFunctions;



// * ========================================= * //
// * ------- CONSTRUCTOR AND DESTRUCTOR ------ * //
// * ========================================= * //


	/// Constructor for the `ArgPair` classes that is called when defining the different parameter names in the constructor of `ChainLoader`.
	/// A new entry is made in the `fParameterMapping` with key `identifier`. @b Aborts if `fParameterMapping` already contains a key with string `identifier`.
	ArgPair_base::ArgPair_base(const std::string &identifier) :
		fIdentifier(identifier)
	{
		if(fParameterMapping.find(fIdentifier) == fParameterMapping.end())
			Error::PrintFatalError();
		fParameterMapping[fIdentifier];
	}


	/// The destructor removes key `fIdentifier` with its value (a pointer to `this` object) from the `fParameterMappings`.
	ArgPair_base::~ArgPair_base()
	{
		fParameterMapping.erase(fIdentifier);
	}


	std::unordered_map<ArgPair_base*> ArgPair_base::fParameterMapping;



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