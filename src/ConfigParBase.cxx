// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "ConfigParBase.h"
	#include "CommonFunctions.h"
	#include "TString.h"
	#include <iomanip>
	using namespace CommonFunctions;



// * ========================================= * //
// * ------- CONSTRUCTOR AND DESTRUCTOR ------ * //
// * ========================================= * //


	/// Constructor for the `ArgPair` classes that is called when defining the different parameter names in the constructor of `ChainLoader`.
	/// A new entry is made in the `fInstances` with key `identifier`. @b Aborts if `fInstances` already contains a key with string `identifier`.
	ConfigParBase::ConfigParBase(const std::string &identifier) :
		fIdentifier(identifier),
		fValueIsSet(false)
	{
		if(!(fInstances.find(fIdentifier) == fInstances.end()))
			Error::PrintFatalError(Form("Parameter \"%s\" is already defined", fIdentifier.c_str()));
		fInstances.emplace(make_pair(fIdentifier, this));
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


		void ConfigParBase::AddValue(std::string value)
		{
			fReadStrings.push_back(value);
		}



// * ====================== * //
// * ------- GETTERS ------ * //
// * ====================== * //


	/// Get access to one of the declared `ConfigParBase` objects.
	/// @return Returns a `nullptr` if there is no key with name `identifier`.
	ConfigParBase* ConfigParBase::GetParameter(const std::string &identifier)
	{
		auto key = fInstances.find(identifier);
		if(key == fInstances.end() || !key->second) {
			Error::PrintWarning(Form("Parameter \"%s\" has not been defined", identifier.c_str()));
			return nullptr;
		}
		return key->second;
	}


	/// Get access to one of the declared `ConfigParBase` objects, but also clean.
	/// This method is used in `LoadConfiguration` to ensure that default values are overwritten if available.
	ConfigParBase* ConfigParBase::GetCleanParameter(const std::string &identifier)
	{
		auto par = GetParameter(identifier);
		if(par) par->ResetIfHasValue();
		return par;
	}



// * ========================== * //
// * ------- INFORMATION ------ * //
// * ========================== * //


	void ConfigParBase::PrintAll()
	{
		std::cout << std::endl << "Loaded " << fInstances.size() << " parameters" << std::endl;
		size_t width{0};
		for(auto &it : fInstances) if(it.first.size() > width) width = it.first.size();
		width += 2;
		for(auto &it : fInstances) {
			auto inst = it.second->GetNReadValues();
			if(!it.second->GetNReadValues()) continue;
			std::cout << std::setw(width) << it.first << " = ";
			it.second->PrintValue();
			std::cout << std::endl;
		}
	}