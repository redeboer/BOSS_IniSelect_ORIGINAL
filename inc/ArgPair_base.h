#ifndef BOSS_Afterburner_ArgPair_base_H
#define BOSS_Afterburner_ArgPair_base_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include <iomanip>
	#include <iostream>
	#include <list>
	#include <unordered_map>
	#include <sstream>
	#include <fstream>
	#include <string>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// This is the base class for classes that can be used to load a final analysis configuration during runtime.
		///
		/// The configuration (`.config`) files of the *BOSS Afterburner* final selection framework can be used to load settings for your analysis during runtime from a configuration text file. This allows you to quickly modify plots without having to recompile your analysis script: you just change some of the parameters in your config file. The types of parameters (their identifiers and value types) are defined in the `ConfigLoader` class.
		///
		/// The `ArgPair` classes (base `ArgPair_base` and derived classes such as `ArgPair`) represent these certain types of parameters. The `ArgPair_base` class defines the most abstract form of a configuration parameter. It just has an @b identifier (or parameter name) and a @b value or set of values. Both are strings, as they are the raw data read from the data file. A line in your configuration file can for instance look like this:
		/// ```
		/// 	Plot raw data = true
		/// ```
		/// Here, the line `"Plot raw data"` is the parameter @b identifier, while `"true"` is the @b value. The line will only be read if there is an `ArgPair_base` object (or derived object with the name `"Plot raw data"`.) The value is in `string` format, but in this case you will want the value to be a boolean. For this you should define the parameter in the `ConfigLoader` in terms of the `ArgPair` class.
		///
		/// Another possibility would be to have the lines:
		/// ```
		/// 	Draw branches = {
		/// 		do something; this is OK;
		/// 		mctruth, E, 150, 0.,
		/// 		3.5, "E1"
		/// 		plo, 1, ;
		///			; empty:zero}
		/// 		
		/// ```
		/// Note this is a lazily formatted vector. What `ArgPair_base` does in this case is, it will read these lines as an arreay
		/// In this case, what will be read to the `list` of read values (`fReadValues`) for the parameter with name `"Draw branches"` is:
		/// ```
		/// 	do something
		/// 	this is OK
		/// 	mctruth, E, 150, 0.,3.5, "E1"
		/// 	plo, 1,
		/// 	empty:zero
		/// ```
		/// The contents of this example are not that useful, but you get the idea:
		/// 	-# a parameter will contain more than one value if its identifier is followed by an opening curly brace (`{`)
		/// 	-# its values are separated by a semicolumn (`;`).
		/// What you want to do with these read values is up to use to define in a derived class.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     January 7th, 2019
	class ArgPair_base {
	public:
	/// @name Constructor and destructor
		///@{
		ArgPair_base(const std::string &identifier);
		~ArgPair_base();
		///@}


	/// @name Class mapping
		///@{
		static void PrintAll();
		static ArgPair_base* GetParameter(const std::string &identifier);
		static const size_t GetNParameters() { return fParameterMapping.size(); }
		///@}


	/// @name Read handlers
		///@{
		void AddValue(const std::string &value) { fReadValues.push_back(value); }
		static size_t LoadConfiguration(const char* filename);
		///@}


		// static void SetParameters(const std::string &parname, const std::string &parvalue, bool output=false);
		// virtual void Print(int width=0) = 0;
		// virtual void SetParameter(const std::string &parname, const std::string &parvalue, bool output=false) = 0;
	protected:
	/// @name Parameter values
		///@{
		const std::string fIdentifier; ///< Unique identifier of the paramter. If this identifier is found in the configuration file you load with the `ConfigLoader`, its corresponding values will be loaded to `fReadValues`. @warning The executable will `terminate` if the identifier already exists in the mapping of parameters `fParameterMapping`.
		std::list<std::string> fReadValues; ///< Loaded string.
		///@}


	/// @name Read handlers
		///@{
		bool ImportValue(std::string line);
		bool ImportFirstValue(std::string line);
		bool ImportNextValue(std::string line);
		void ImportSingleValue(std::string line);
		static ArgPair_base* ObtainParameter(std::string line);
		///@}


	/// @name Class mapping
		///@{
		static std::unordered_map<std::string, ArgPair_base*> fParameterMapping;
		///@}
	};



/// @}
#endif