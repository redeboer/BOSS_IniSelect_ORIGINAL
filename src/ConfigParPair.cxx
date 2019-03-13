// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ConfigParBase.h"
	#include "CommonFunctions.h"
	#include "FrameworkSettings.h"
	using namespace CommonFunctions;



// * ========================================= * //
// * ------- CONSTRUCTOR AND DESTRUCTOR ------ * //
// * ========================================= * //


	/// Constructor for the `ArgPair` classes that is called when defining the different parameter names in the constructor of `ChainLoader`.
	/// A new entry is made in the `fParameterMapping` with key `identifier`. @b Aborts if `fParameterMapping` already contains a key with string `identifier`.
	ConfigParBase::ConfigParBase(const std::string &identifier) :
		fIdentifier(identifier)
	{
		if(fParameterMapping.find(fIdentifier) == fParameterMapping.end())
			Error::PrintFatalError();
		fParameterMapping[fIdentifier];
	}


	/// The destructor removes key `fIdentifier` with its value (a pointer to `this` object) from the `fParameterMappings`.
	ConfigParBase::~ConfigParBase()
	{
		fParameterMapping.erase(fIdentifier);
	}


	std::unordered_map<std::string, ConfigParBase*> ConfigParBase::fParameterMapping;



// * ====================== * //
// * ------- GETTERS ------ * //
// * ====================== * //


	/// Get access to one of the declared `ConfigParBase` objects.
	/// @return Returns a `nullptr` if there is no key with name `identifier`.
	ConfigParBase* ConfigParBase::GetParameter(const std::string &identifier)
	{
		auto key = fParameterMapping.find(identifier);
		if(key == fParameterMapping.end()) return nullptr;
		return key->second;
	}
