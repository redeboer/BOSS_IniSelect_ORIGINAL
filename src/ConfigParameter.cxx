// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ConfigParameter.h"
	#include "CommonFunctions.h"
	#include "FrameworkSettings.h"
	using namespace CommonFunctions;



// * ========================================= * //
// * ------- CONSTRUCTOR AND DESTRUCTOR ------ * //
// * ========================================= * //


	/// Constructor for the `ArgPair` classes that is called when defining the different parameter names in the constructor of `ChainLoader`.
	/// A new entry is made in the `fInstances` with key `identifier`. @b Aborts if `fInstances` already contains a key with string `identifier`.
	ConfigParameter::ConfigParameter(const std::string &identifier) :
		ConfigParBase(identifier) {}


	/// The destructor removes key `fIdentifier` with its value (a pointer to `this` object) from the `fInstancess`.
	ConfigParameter::~ConfigParameter()
	{
		fInstances.erase(fIdentifier);
	}


	std::unordered_map<std::string, ConfigParameter*> ConfigParameter::fInstances;



// * ====================== * //
// * ------- GETTERS ------ * //
// * ====================== * //


	/// Get access to one of the declared `ConfigParameter` objects.
	/// @return Returns a `nullptr` if there is no key with name `identifier`.
	ConfigParameter* ConfigParameter::GetParameter(const std::string &identifier)
	{
		auto key = fInstances.find(identifier);
		if(key == fInstances.end()) return nullptr;
		return key->second;
	}
