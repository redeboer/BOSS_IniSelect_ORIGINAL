#ifndef BOSS_Afterburner_ConfigParameter_H
#define BOSS_Afterburner_ConfigParameter_H


// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //


	#include "CommonFunctions.h"
	#include "ConfigParBase.h"
	#include "TString.h"
	#include <iomanip>
	#include <iostream>
	#include <list>
	#include <sstream>
	#include <string>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// This `template` class is used to specify how to handle the values loaded to the `ConfigParBase` base class.
		/// You should define in this derived class how to go from the loaded string values to data. This has to defined for each `TYPE`. In principle, the `TYPE` is just a `typename`, that is, for instance a `double` or `int`. But you can also overload the `ConvertStringsToValue` function and specify in that overload how to go from the strings in `fReadStrings` to the data of your object.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     March 15th, 2019
	template<class TYPE>
	class ConfigParameter : public ConfigParBase {
	public:
		ConfigParameter(const std::string &identifier);
		ConfigParameter(const std::string &identifier, const TYPE &default_value);
		void operator=(const TYPE &val) { fValue = val; }
		bool operator==(const std::string &data) const { return !GetIdentifier().compare(data); }
		bool operator!=(const std::string &data) const { return  GetIdentifier().compare(data); }
		explicit operator TYPE() { return fValue; }
		void PrintValue() const;
		virtual bool ConvertStringsToValue();
		virtual bool ConvertValueToStrings();
		bool ConvertStringsToValue_impl();
	private:
		bool HasSingleString() const;
		TYPE fValue; /// The object that contains the data read from the `fReadStrings` `list`.
	};



/// @}
// * =============================== * //
// * ------- SPECIALISATIONS ------- * //
// * =============================== * //
// ! This list needs to be expended if you are implementing new template specialisations


	template class ConfigParameter<std::string>;
	template class ConfigParameter<bool>;



// * ============================ * //
// * ------- CONSTRUCTORS ------- * //
// * ============================ * //


	template<class TYPE> inline
	ConfigParameter<TYPE>::ConfigParameter(const std::string &identifier) :
		ConfigParBase(identifier) {}


	template<class TYPE> inline
	ConfigParameter<TYPE>::ConfigParameter(const std::string &identifier, const TYPE &default_value) :
		ConfigParBase(identifier), fValue(default_value)
	{
		if(!ConvertValueToStrings()) return;
		fValueIsSet = true;
	}



// * ===================================== * //
// * ------- ConvertValueToStrings ------- * //
// * ===================================== * //


	/// Convert the string values in `fReadStrings` to the `fValue` data member.
	/// Default `ConvertValueToStrings` template function: works only for `type`s like `double`s, because this function relies on `std::stringstream`.
	template<typename TYPE> inline
	bool ConfigParameter<TYPE>::ConvertValueToStrings()
	{
		ClearValues();
		std::stringstream ss;
		ss << fValue;
		AddValue(ss.str());
		return true;
	}


	/// `ConvertValueToStrings` template specialisation for `bool`.
	template<> inline
	bool ConfigParameter<bool>::ConvertValueToStrings()
	{
		ClearValues();
		if(fValue) AddValue("true");
		else       AddValue("false");
		return true;
	}



// * ===================================== * //
// * ------- ConvertStringsToValue ------- * //
// * ===================================== * //


	/// Convert the string values read to `fReadStrings` to the `fValue` member.
	template<typename TYPE> inline
	bool ConfigParameter<TYPE>::ConvertStringsToValue()
	{
		if(!fReadStrings.size()) {
			CommonFunctions::Error::PrintWarning(Form("No values to convert for parameter \"%s\"", GetIdentifier().c_str()));
			return false;
		}
		if(!ConvertStringsToValue_impl()) return false;
		fValueIsSet = true;
		return true;
	}


	/// Default `ConvertStringsToValue` handler (only works for internal C++ types).
	/// @remark The implementation might differ per template and would have to be defined here in template specialisations of `ConverStringsToValue_impl`
	template<typename TYPE> inline
	bool ConfigParameter<TYPE>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		std::istringstream ss(fReadStrings.front());
		ss >> fValue;
		return true;
	}


	/// `ConvertStringsToValue` handler for `bool`eans.
	template<> inline
	bool ConfigParameter<bool>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		if(
			!fReadStrings.front().compare("false") ||
			!fReadStrings.front()[0] == '0') fValue = false;
		else fValue = true;
		return true;
	}


	/// `ConvertStringsToValue` handler for `string`s.
	template<> inline
	bool ConfigParameter<std::string>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		fValue = fReadStrings.front();
		return true;
	}



// * ========================== * //
// * ------- PrintValue ------- * //
// * ========================== * //


	/// By default `fReadStrings` are printed in a vector style as a value.
	template<class TYPE> inline
	void ConfigParameter<TYPE>::PrintValue() const
	{
		if(!fReadStrings.size()) return;
		if(fReadStrings.size() == 1)
			std::cout << fReadStrings.front();
		else {
			std::cout << "{ ";
			auto val = fReadStrings.begin();
			while(val != fReadStrings.end()) {
				std::cout << *val;
				++val;
				if(val != fReadStrings.end()) std::cout << ", ";
			}
			std::cout << " }";
		}
	}


	/// In case of a `bool` `fValue`, either `true` or `false` should be printed.
	template<> inline
	void ConfigParameter<bool>::PrintValue() const
	{
		if(fValue) std::cout << "true";
		else       std::cout << "false";
	}


	/// In case of a `string` `fValue`, encapsulate its value in double quotations marks (`"`).
	template<> inline
	void ConfigParameter<std::string>::PrintValue() const
	{
		std::cout << "\"" << fValue << "\"";
	}



// * ================================ * //
// * ------- HELPER FUNCTIONS ------- * //
// * ================================ * //


	/// Check if `fReadStrings` contains more than one value and print warning if so.
	/// This should not be the case in case `fValue` is of type `double`, `bool`, etc.
	template<typename TYPE> inline
	bool ConfigParameter<TYPE>::HasSingleString() const
	{
		if(fReadStrings.size() > 1) {
			CommonFunctions::Error::PrintWarning(Form("Parameter \"%s\" is single value, but has multiple read values\n  --> Maybe define another template specialisation of ConvertStringsToValue?", GetIdentifier().c_str()));
			return false;
		}
		return true;
	}



#endif