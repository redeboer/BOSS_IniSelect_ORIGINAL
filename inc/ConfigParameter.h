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
		/// You should define in this derived class how to go from the loaded string values to data. This has to defined for each `TYPE`. In principle, the `TYPE` is just a `typename`, that is, for instance a `double` or `int`. But you can also overload the `ConvertToValue` function and specify in that overload how to go from the strings in `fReadValues` to the data of your object.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     March 15th, 2019
	template<class TYPE>
	class ConfigParameter : public ConfigParBase {
	public:
		ConfigParameter(const std::string &identifier);
		ConfigParameter(const std::string &identifier, const TYPE &default_value);
		void operator=(const TYPE &val) { value = val; }
		bool operator==(const std::string &data) const { return !name.compare(data); }
		bool operator!=(const std::string &data) const { return  name.compare(data); }
		explicit TYPE operator() { return value; }
		virtual void SetValues();
	private:
		void ConvertToValue();
		void ConvertFromValue();
		TYPE fValue;
	};



/// @}
// * =============================== * //
// * ------- SPECIALISATIONS ------- * //
// * =============================== * //
// ! This list needs to be expended if you start using more typenames


	template class ConfigParameter<std::string>;
	template class ConfigParameter<bool>;



// * ============================ * //
// * ------- CONSTRUCTORS ------- * //
// * ============================ * //


	template<class TYPE> inline
	ConfigParameter<TYPE>::ConfigParameter(const std::string &identifier) :
		ConfigParBase(identifier)
	{
		ConvertToValue();
	}


	template<class TYPE> inline
	ConfigParameter<TYPE>::ConfigParameter(const std::string &identifier, const TYPE &default_value) :
		ConfigParBase(identifier),
		fValue(default_value)
	{
		ConvertFromValue();
	}



// * ======================= * //
// * ------- SETTERS ------- * //
// * ======================= * //


	/// Convert the string values in `fReadValues` to the `fValue` data member.
	/// Default `ConvertFromValue` template function: works only for `type`s like `double`s, because this function relies on `std::stringstream`.
	template<typename TYPE> inline
	void ConfigParameter<TYPE>::ConvertFromValue()
	{
		if(!fReadValues.size()) {
			CommonFunctions::Error::PrintWarning(Form("No values to convert for parameter \"%s\"", fIdentifier.c_str()));
			return;
		}
		std::istringstream ss(fReadValues.front());
		ss >> fValue;
	}


	/// Default `ConvertToValue` template function: works only for `type`s like `double`s, because this function relies on .
	template<typename TYPE> inline
	void ConfigParameter<TYPE>::ConvertFromValue()
	{
		if(!fReadValues.size()) {
			CommonFunctions::Error::PrintWarning(Form("No values to convert for parameter \"%s\"", fIdentifier.c_str()));
			return;
		}
		std::istringstream ss(fReadValues.front());
		ss >> fValue;
	}


	template<typename TYPE> inline
	void ConfigParameter<TYPE>::SetParameter(const std::string &parname, const std::string &parvalue, bool output)
	{
		if(name.compare(parname)) return;
		SetValue(parvalue);
		if(output) Print();
	}


	template<> void ConfigParameter<std::string>::SetParameter(const std::string &parname, const std::string &parvalue, bool output)
	{
		if(name.compare(parname)) return;
		value = parvalue;
		if(output) Print();
	}


	template<> void ConfigParameter<bool>::SetParameter(const std::string &parname, const std::string &parvalue, bool output)
	{
		if(name.compare(parname)) return;
		if(!parvalue.compare("false") || parvalue.front()=='0') value = false;
		else value = true;
		if(output) Print();
	}



#endif