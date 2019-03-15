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
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     March 15th, 2019
	class ConfigParBase {
	public:
		ConfigParBase(const std::string &identifier);
		~ConfigParBase();

		static void PrintAll();
		static ConfigParBase* GetParameter(const std::string &identifier);
		static const size_t GetNParameters() { return fInstances.size(); }

		const std::string &GetIdentifier() { return fIdentifier; }

		void AddValue(std::string value) { fReadValues.push_back(value); }
		const std::list<std::string>* GetListOfValues() { return &fReadValues; }


	protected:
		std::list<std::string> fReadValues; ///< Loaded values in string format. You can specify in derived classes how to use these values.


	private:
		const std::string fIdentifier; ///< Unique identifier of the paramter. If this identifier is found in the configuration file you loaded with the `ConfigLoader`, its corresponding values will be added to `fReadValues`. @warning The executable will `terminate` if the identifier already exists in the mapping of parameters `fInstances`.
		static std::unordered_map<std::string, ConfigParBase*> fInstances;
	};



/// @}
#endif