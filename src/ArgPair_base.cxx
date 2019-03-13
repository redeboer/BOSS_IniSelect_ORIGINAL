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


	std::unordered_map<std::string, ArgPair_base*> ArgPair_base::fParameterMapping;
