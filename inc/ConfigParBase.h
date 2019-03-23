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


	/// This is the base class for objects that hold parameters from a config file.
		/// This base class is necessary to allow one to loop over all different types (manifestations of `template`s) for `ConfigParameter`.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     March 15th, 2019
	class ConfigParBase {
	public:
		ConfigParBase(const std::string &identifier);
		~ConfigParBase();

		static void PrintAll();
		static ConfigParBase* GetParameter(const std::string &identifier);
		static ConfigParBase* GetCleanParameter(const std::string &identifier);
		static const size_t GetNParameters() { return fInstances.size(); }
		static const std::unordered_map<std::string, ConfigParBase*>* GetMapOfParameters() { return &fInstances; }

		const std::string &GetIdentifier() const { return fIdentifier; }

		void AddValue(std::string value);
		void ResetIfHasValue() { if(fValueIsSet) ClearValues(); }
		const size_t GetNReadValues() const { return fReadStrings.size(); }
		const std::list<std::string>* GetListOfValues() { return &fReadStrings; }
		bool ConvertStringsToValue();
		bool ConvertValueToStrings();
		virtual void PrintValue() const = 0;
		bool ValueIsSet() const { return fValueIsSet; }


	protected:
		virtual bool ConvertStringsToValue_impl() = 0;
		virtual bool ConvertValueToStrings_impl() = 0;
		std::list<std::string> fReadStrings; ///< Loaded values in string format. You can specify in derived classes how to use these values.
		bool fValueIsSet; /// Switch that is used to prevent from double adding values to the `fReadStrings` `list`.


	private:
		void ClearValues() { fReadStrings.clear(); fValueIsSet = false; }
		const std::string fIdentifier; ///< Unique identifier of the paramter. If this identifier is found in the configuration file you loaded with the `ConfigLoader`, its corresponding values will be added to `fReadStrings`. @warning The executable will `terminate` if the identifier already exists in the mapping of parameters `fInstances`.
		static std::unordered_map<std::string, ConfigParBase*> fInstances;
	};



/// @}
#endif