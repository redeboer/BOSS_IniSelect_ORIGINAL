#ifndef BOSS_Afterburner_ConfigParBase_H
#define BOSS_Afterburner_ConfigParBase_H


// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include <list>
	#include <string>
	#include <unordered_map>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// This is the base class for classes that can be used to load a final analysis configuration during runtime.
		/// The configuration (`.config`) files of the *BOSS Afterburner* final selection framework can be used to load settings for your analysis during runtime from a configuration text file. This allows you to quickly modify plots without having to recompile your analysis script: you just change some of the parameters in your config file. The types of parameters (their identifiers and value types) are defined in the `ConfigLoader` class.
		///
		/// The `ConfigParameter` classes (base `ConfigParBase` and derived classes such as `ConfigParameter`) represent these certain types of parameters. The `ConfigParBase` class defines the most abstract form of a configuration parameter. It just has an @b identifier (or parameter name) and a @b value or set of values. Both are strings, as they are the raw data read from the data file. A line in your configuration file can for instance look like this:
		/// ```
		/// 	Plot raw data = true
		/// ```
		/// Here, the line `"Plot raw data"` is the parameter @b identifier, while `"true"` is the @b value. The line will only be read if there is an `ConfigParBase` object (or derived object with the name `"Plot raw data"`.) The value is in `string` format, but in this case you will want the value to be a boolean. For this you should define the parameter in the `ConfigLoader` in terms of the `ConfigParameter` class.
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
		/// Note this is a lazily formatted vector. What `ConfigParBase` does in this case is, it will read these lines as an arreay
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
	class ConfigParBase {
	public:
		ConfigParBase(const std::string &identifier);
		~ConfigParBase();

		static void PrintAll();
		static ConfigParBase* GetParameter(const std::string &identifier);
		static const size_t GetNParameters() { return fInstances.size(); }

		const std::string &GetIdentifier() { return fIdentifier; }

		void AddValue(const std::string &value) { fReadValues.push_back(value); }
		const std::list<std::string>* GetListOfValues() { return &fReadValues; }


	protected:
		std::list<std::string> fReadValues; ///< Loaded values in string format. You can specify in derived classes how to use these values.


	private:
		const std::string fIdentifier; ///< Unique identifier of the paramter. If this identifier is found in the configuration file you loaded with the `ConfigLoader`, its corresponding values will be added to `fReadValues`. @warning The executable will `terminate` if the identifier already exists in the mapping of parameters `fInstances`.
		static std::unordered_map<std::string, ConfigParBase*> fInstances;
	};



/// @}
#endif