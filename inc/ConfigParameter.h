#ifndef BOSS_Afterburner_ConfigParameter_H
#define BOSS_Afterburner_ConfigParameter_H


// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //


	#include "AxisBinning.h"
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
		void operator=(const TYPE &val) { fValue = val; ConvertValueToStrings(); }
		bool operator==(const std::string &data) const { return !GetIdentifier().compare(data); }
		bool operator!=(const std::string &data) const { return  GetIdentifier().compare(data); }
		explicit operator TYPE() { return fValue; }
		void PrintValue() const;
	private:
		bool ConvertStringsToValue_impl_str();
		virtual bool ConvertStringsToValue_impl();
		virtual bool ConvertValueToStrings_impl();
		bool HasSingleString() const;
		TYPE fValue; /// The object that contains the data read from the `fReadStrings` `list`.
	};



/// @}
// * =============================== * //
// * ------- SPECIALISATIONS ------- * //
// * =============================== * //
// ! This list needs to be expended if you are implementing new template specialisations


	template class ConfigParameter<bool>;
	template class ConfigParameter<std::string>;
	template class ConfigParameter<AxisBinning>;



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
		if(ConvertValueToStrings()) fValueIsSet = true;
	}



// * ===================================== * //
// * ------- ConvertValueToStrings ------- * //
// * ===================================== * //


	/// Default `ConvertValueToStrings` template function: works only for `type`s like `double`s, because this function relies on `std::stringstream`.
	template<typename TYPE> inline
	bool ConfigParameter<TYPE>::ConvertValueToStrings_impl()
	{
		std::stringstream ss;
		ss << fValue;
		AddValue(ss.str());
		return true;
	}


	/// `ConvertValueToStrings` implementation handler for `bool`.
	template<> inline
	bool ConfigParameter<bool>::ConvertValueToStrings_impl()
	{
		if(fValue) AddValue("true");
		else       AddValue("false");
		return true;
	}


	/// `ConvertValueToStrings` implementation handler for `AxisBinning`.
	template<> inline
	bool ConfigParameter<AxisBinning>::ConvertValueToStrings_impl()
	{
		std::stringstream ss;
		if(!fValue.IsOK()) {
			std::cout << "WARNING: " << fValue.NBins() << ", " << fValue.From() << ", " << fValue.To() << std::endl;
			return false;
		}
		ss << fValue.NBins() << ", " << fValue.From() << ", " << fValue.To();
		AddValue(ss.str());
		return true;
	}



// * ===================================== * //
// * ------- ConvertStringsToValue ------- * //
// * ===================================== * //


	/// Default implementation handler for `ConvertStringsToValue` (only works for internal C++ types).
	template<typename TYPE> inline
	bool ConfigParameter<TYPE>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		std::istringstream ss(fReadStrings.front());
		ss >> fValue;
		return true;
	}


	/// `ConvertStringsToValue` handler for an object that can be contructed from a `std::string`.
	template<class TYPE> inline
	bool ConfigParameter<TYPE>::ConvertStringsToValue_impl_str()
	{
		if(!HasSingleString()) return false;
		fValue = fReadStrings.front();
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
		return ConvertStringsToValue_impl_str();
	}


	/// `ConvertStringsToValue` handler for `AxisBinning`s (has a `string` constructor).
	template<> inline
	bool ConfigParameter<AxisBinning>::ConvertStringsToValue_impl()
	{
		return ConvertStringsToValue_impl_str();
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


	/// In case of a `AxisBinning` `fValue`, encapsulate its value in double quotations marks (`"`).
	template<> inline
	void ConfigParameter<AxisBinning>::PrintValue() const
	{
		std::cout << fValue.NBins() << " bins, range " << fValue.From() << " to " << fValue.To();
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