// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ConfigParBase.h"
	#include "CommonFunctions.h"
	#include "TString.h"
	using namespace CommonFunctions;



// * ========================================= * //
// * ------- CONSTRUCTOR AND DESTRUCTOR ------ * //
// * ========================================= * //


	/// Constructor for the `ArgPair` classes that is called when defining the different parameter names in the constructor of `ChainLoader`.
	/// A new entry is made in the `fInstances` with key `identifier`. @b Aborts if `fInstances` already contains a key with string `identifier`.
	ConfigParBase::ConfigParBase(const std::string &identifier) :
		fIdentifier(identifier)
	{
		if(fInstances.find(fIdentifier) == fInstances.end())
			Error::PrintFatalError(Form("Parameter \"%s\" is already defined", fIdentifier));
		fInstances[fIdentifier];
	}


	/// The destructor removes key `fIdentifier` with its value (a pointer to `this` object) from the `fInstancess`.
	ConfigParBase::~ConfigParBase()
	{
		fInstances.erase(fIdentifier);
	}


	std::unordered_map<std::string, ConfigParBase*> ConfigParBase::fInstances;



// * ====================== * //
// * ------- GETTERS ------ * //
// * ====================== * //


	/// Get access to one of the declared `ConfigParBase` objects.
	/// @return Returns a `nullptr` if there is no key with name `identifier`.
	ConfigParBase* ConfigParBase::GetParameter(const std::string &identifier)
	{
		auto key = fInstances.find(identifier);
		if(key == fInstances.end()) return nullptr;
		return key->second;
	}



// * ========================== * //
// * ------- INFORMATION ------ * //
// * ========================== * //


	void ConfigParBase::PrintAll()
	{
		std::cout << std::endl << "Loaded " << fInstances.size() << " paramters" << std::endl;
		for(auto &it : fInstances) {
			std::cout << std::endl << "  " << it.first << std::endl;
			for(auto &val : *it.second->GetListOfValues()) {
				std::cout << std::endl << "    " << val << std::endl;
			}
		}
	}