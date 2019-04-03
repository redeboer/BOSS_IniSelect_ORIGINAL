#ifndef BOSS_Afterburner_ConfigParameter_H
#define BOSS_Afterburner_ConfigParameter_H


// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //


	#include "AxisBinning.h"
	#include "BranchPlotOptions.h"
	#include "CommonFunctions.h"
	#include "ConfigParBase.h"
	#include "FitPars.h"
	#include "ReconstructedParticle.h"
	#include "RooFit.h"
	#include "TString.h"
	#include <iomanip>
	#include <iostream>
	#include <list>
	#include <map>
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
		TYPE& Get() { return fValue; }
    TYPE* operator->() { return &fValue; }
		TYPE& operator*() { return fValue; }
		void operator=(const TYPE &val) { fValue = val; ConvertValueToStrings(); }
		const bool operator==(const std::string &data) const { return !GetIdentifier().compare(data); }
		const bool operator!=(const std::string &data) const { return  GetIdentifier().compare(data); }
		operator TYPE() { return fValue; }
		void PrintValue() const;
	private:
		// const bool ConvertStringsToValue_impl_str();
		virtual const bool ConvertStringsToValue_impl();
		virtual const bool ConvertValueToStrings_impl();
		const bool HasSingleString() const;
		TYPE fValue; /// The object that contains the data read from the `fReadStrings` `list`.
	};



/// @}
// * =============================== * //
// * ------- SPECIALISATIONS ------- * //
// * =============================== * //
// ! This list needs to be expended if you are implementing new template specialisations. Please keep sorted in alphabetical order.


	template class ConfigParameter<AxisBinning>;
	template class ConfigParameter<RooFit::MsgLevel>;
	template class ConfigParameter<bool>;
	template class ConfigParameter<std::list<BranchPlotOptions> >;
	template class ConfigParameter<std::list<std::pair<ReconstructedParticle, BranchPlotOptions> > >;
	template class ConfigParameter<std::string>;


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
	const bool ConfigParameter<TYPE>::ConvertValueToStrings_impl()
	{
		std::stringstream ss;
		ss << fValue;
		AddValue(ss.str());
		return true;
	}


	/// `ConvertValueToStrings` implementation handler for `AxisBinning`.
	template<> inline
	const bool ConfigParameter<AxisBinning>::ConvertValueToStrings_impl()
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


	/// `ConvertValueToStrings` implementation handler for `RooFit::MsgLevel`.
	template<> inline
	const bool ConfigParameter<RooFit::MsgLevel>::ConvertValueToStrings_impl()
	{
		switch(fValue) {
			case RooFit::DEBUG:    AddValue("DEBUG");    break;
			case RooFit::INFO:     AddValue("INFO");     break;
			case RooFit::PROGRESS: AddValue("PROGRESS"); break;
			case RooFit::WARNING:  AddValue("WARNING");  break;
			case RooFit::ERROR:    AddValue("ERROR");    break;
			case RooFit::FATAL:    AddValue("FATAL");    break;
			default: return false;
		}
		return true;
	}


	/// `ConvertValueToStrings` implementation handler for `bool`.
	template<> inline
	const bool ConfigParameter<bool>::ConvertValueToStrings_impl()
	{
		if(fValue) AddValue((std::string)"true");
		else       AddValue((std::string)"false");
		return true;
	}


	/// `ConvertValueToStrings` implementation handler for a list of `BranchPlotOptions`.
		/// This parameter can for instance be used to [`TTree::Draw`](https://root.cern.ch/doc/master/classTTree.html#a8a2b55624f48451d7ab0fc3c70bfe8d7) a selection of branches (or of a combination of branches).
	template<> inline
	const bool ConfigParameter<std::list<BranchPlotOptions> >::ConvertValueToStrings_impl()
	{
		for(auto &it : fValue) {
			std::stringstream ss;
			ss << "{\"";
			ss << it.VarExp() << "\", \"";
			ss << it.CutSelection() << "\", \"";
			ss << it.DrawOption() << "\"}";
			AddValue(ss.str());
		}
		return true;
	}


	/// `ConvertValueToStrings` implementation handler for list of pairs of a `ReconstructedParticle` object (which characterise a signal that is to be fit) with a corresponding `BranchPlotOptions` object (which characterise the histogram that is to be fit).
	template<> inline
	const bool ConfigParameter<std::list<std::pair<ReconstructedParticle, BranchPlotOptions> > >::ConvertValueToStrings_impl()
	{
		for(auto &it : fValue) {
			AddValue(Form("%d", it.first.PDGCode()));
			AddValue(it.first.DaughterLabel());
			AddValue(it.second.BuildOriginalString().c_str());
		}
		return true;
	}



// * ===================================== * //
// * ------- ConvertStringsToValue ------- * //
// * ===================================== * //


	/// Default implementation handler for `ConvertStringsToValue` (only works for internal C++ types).
	template<typename TYPE> inline
	const bool ConfigParameter<TYPE>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		std::istringstream ss(fReadStrings.front());
		ss >> fValue;
		return true;
	}


	/// `ConvertStringsToValue` handler for `AxisBinning`s (has a `string` constructor).
	template<> inline
	const bool ConfigParameter<AxisBinning>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		fValue = fReadStrings.front();
		return true;
	}


	/// `ConvertStringsToValue` handler for `RooFit::MsgLevel`.
	template<> inline
	const bool ConfigParameter<RooFit::MsgLevel>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		std::string &str = fReadStrings.front();
		if     (!str.compare("DEBUG"))    fValue = RooFit::DEBUG;
		else if(!str.compare("INFO"))     fValue = RooFit::INFO;
		else if(!str.compare("PROGRESS")) fValue = RooFit::PROGRESS;
		else if(!str.compare("WARNING"))  fValue = RooFit::WARNING;
		else if(!str.compare("ERROR"))    fValue = RooFit::ERROR;
		else if(!str.compare("FATAL"))    fValue = RooFit::FATAL;
		else return false;
		return true;
	}


	/// `ConvertStringsToValue` handler for `bool`eans.
	template<> inline
	const bool ConfigParameter<bool>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		if(
			!fReadStrings.front().compare("false") ||
			!fReadStrings.front()[0] == '0') fValue = false;
		else fValue = true;
		return true;
	}


	/// `ConvertStringsToValue` handler for `bool`eans.
	template<> inline
	const bool ConfigParameter<std::list<BranchPlotOptions> >::ConvertStringsToValue_impl()
	{
		for(auto& it : fReadStrings) {
			fValue.push_back({it});
		}
		return true;
	}


	/// `ConvertStringsToValue` implementation handler for list of pairs of a `ReconstructedParticle` object (which characterise a signal that is to be fit) with a corresponding `BranchPlotOptions` object (which characterise the histogram that is to be fit).
	template<> inline
	const bool ConfigParameter<std::list<std::pair<ReconstructedParticle, BranchPlotOptions> > >::ConvertStringsToValue_impl()
	{
		/// <ol>
		/// <li> **Abort** if the number of read strings for this parameter is not a multiple of 3. Each batch of 3 read values is used to construct a pair of a `ReconstructedParticle` with a `BranchPlotOptions`.
			size_t numargs = 6;
			if(fReadStrings.size()%numargs != 0) {
				CommonFunctions::TerminalIO::PrintWarning(Form(
					"Converting parameter \"%s\" failed because it needs exactly %u read values", GetIdentifier().c_str(), numargs));
				return false;
			}
		/// <li> Convert each read value to values in the parameter object.
			for(auto it = fReadStrings.begin(); it != fReadStrings.end(); ++it) {
				/// <ol>
				/// <li> Read value 1: the PDG code or name of the decaying particle.
					TString pdgName{it->c_str()};
					auto pdgCode{pdgName.Atoi()};
				/// <li> Read value 2: the daughters of the decaying particle.
					++it;
					TString daughters{it->c_str()};
				/// <li> Read value 3: the input string for constructing the `BranchPlotOptions` object.
					++it;
					std::string options{it->c_str()};
				/// <li> Read value 4: the list of fit parameters for the signal.
					++it;
					TString fitspars_sig{it->c_str()};
				/// <li> Read value 5: the list of fit parameters for the signal.
					++it;
					TString fitspars_gauss{it->c_str()};
				/// <li> Read value 6: the list of fit parameters for the background.
					++it;
					TString fitspars_bkg{it->c_str()};
				/// <li> Insert a pair (using constructor brackets) into the `fValue` list.
					if(pdgCode) fValue.push_back(std::make_pair(ReconstructedParticle{pdgCode,        daughters.Data()}, BranchPlotOptions{options}));
					else        fValue.push_back(std::make_pair(ReconstructedParticle{pdgName.Data(), daughters.Data()}, BranchPlotOptions{options}));
				/// <li> Add the line of fit parameters to the `FitPars` object in `ReconstructedParticle`.
					fValue.back().first.SetFitPars(fitspars_sig, fitspars_bkg);
					fValue.back().first.SetGaussianPars(fitspars_gauss);
					fValue.back().first.ImportFitPars_DG();
				/// </ol>
			}
		/// </ol>
		return true;
	}


	/// `ConvertStringsToValue` handler for `string`s.
	template<> inline
	const bool ConfigParameter<std::string>::ConvertStringsToValue_impl()
	{
		if(!HasSingleString()) return false;
		fValue = fReadStrings.front();
		return true;
	}



// * ========================== * //
// * ------- PrintValue ------- * //
// * ========================== * //


	/// By default, `fReadStrings` are printed in a vector style as an indication of the value value.
	template<class TYPE> inline
	void ConfigParameter<TYPE>::PrintValue() const
	{
		if(!fReadStrings.size()) return;
		if(fReadStrings.size() == 1)
			std::cout << fReadStrings.front();
		else {
			std::cout << "{" << std::endl;
			auto val = fReadStrings.begin();
			while(val != fReadStrings.end()) {
				std::cout << *val;
				++val;
				if(val != fReadStrings.end()) std::cout << "," << std::endl;
			}
			std::cout << " }";
		}
	}


	/// In case of a `AxisBinning` `fValue`, encapsulate its value in double quotations marks (`"`).
	template<> inline
	void ConfigParameter<AxisBinning>::PrintValue() const
	{
		std::cout << fValue.NBins() << " bins, range " << fValue.From() << " to " << fValue.To();
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
	void ConfigParameter<std::list<BranchPlotOptions> >::PrintValue() const
	{
		std::cout << std::endl << std::endl;
		for(auto& it : fValue) it.Print();
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
	const bool ConfigParameter<TYPE>::HasSingleString() const
	{
		if(fReadStrings.size() > 1) {
			CommonFunctions::TerminalIO::PrintWarning(Form("Parameter \"%s\" is single value, but has multiple read values\n  --> Maybe define another template specialisation of ConvertStringsToValue?", GetIdentifier().c_str()));
			return false;
		}
		return true;
	}



#endif