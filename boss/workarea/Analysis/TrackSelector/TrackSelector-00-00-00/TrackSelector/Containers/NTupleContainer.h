#ifndef Analysis_NTupleContainer_H
#define Analysis_NTupleContainer_H


// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "GaudiKernel/NTuple.h"
	#include "TrackSelector/Containers/Container_base.h"
	#include "TrackSelector/Containers/JobSwitch.h"
	#include <map>
	#include <string>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_objects
/// @{

	/// Container object that is to facilitate the booking procedure for `NTuple::Tuple`s and their `NTuple::Item`s.
	/// See <a href="https://besiii.gitbook.io/boss/the-boss-afterburner/initial/motivation#problems-with-booking-procedures">this page</a> for the motivation. This function also automatically declares a corresponding `JobSwitch` property.
	/// @remark This object can only be used within a derived class off the Gaudi `Algorithm` class because its `fTuple` requires to be booked by it (using e.g. `TrackSelector::BookNTuples`).
	/// @remark To allow for the addition of different types of items (such as integers), there are several mappings of the items (see for instance `fItems_double` and `fItems_int`). If you add new types, make sure to add them to this list of `fItems_*`, where `*` should be the `typename` and to add a template specialisation for `GetItems`. The rest of the templates then don't require template specialisation, as they can be defined in terms of `GetItems`. However, not that you will have to add the new mappings to functions like `PrintTuplesAndItems`, as you can not loop over templates.
	/// @todo Think about a solution for indexed items.
	/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date   February 19th, 2019
	class NTupleContainer : public Container_base
	{
	public:
		/// @name Constructors and destructors
			///@{
			NTupleContainer(const std::string &name, const std::string &description="", const bool write=true);
			~NTupleContainer();
			///@}


		/// @name Instance handlers
			///@{
			static std::map<std::string, NTupleContainer*> instances;
			static NTupleContainer& Get(const std::string &tuple_name) { return *instances.at(tuple_name); }
			static void PrintTuplesAndItems();
			static void PrintFilledTuples();
			template<typename TYPE> void PrintItems();
			///@}


		/// @name Setters
			///@{
			void SetTuplePtr(NTuplePtr& data) { fTuple = data.ptr(); }
			///@}


		/// @name Getters
			///@{
			template<typename TYPE>
			NTuple::Item<TYPE>& GetItem(const std::string &key);
			const ULong_t GetEntries() const { return fEntries; } ///< Read access to `fEntries`.
			///@}


		/// @name NTuple handlers and members
			///@{
			template<typename TYPE>
			void AddItem(const std::string &item_name);
			void AddItem(const std::string &item_name); ///< Shortcut for `AddItem<double>`, added for backward compatibilaty.
			void Write();
			///@}


		/// @name JobSwitch handlers
			///@{
			const bool DoWrite() const { return (bool)fWrite; } /// Return `true` if `fTuple` pointer exists and if `fWrite` property has been set to `true`.
			void SetWriteSwitch(const bool val=true) { fWrite.SetValue(val); }
			///@}


	protected:
		/// @name Data members
			///@{
			template<typename TYPE>
			const std::map<std::string, NTuple::Item<TYPE> >* GetItems();
			JobSwitch fWrite;   ///< Boolean job property that determines whether or not to write data stored to this `NTuple` to a `TTree` (property name starts with `"write_"` by default).
			NTuple::Tuple *fTuple; ///< Pointer to the encapsulated `NTuple::Tuple`.
			ULong_t fEntries;
			std::map<std::string, NTuple::Item<double> > fItems_double; ///< Inventory of added `double` items.
			std::map<std::string, NTuple::Item<int> >    fItems_int;    ///< Inventory of added `int` items.
			///@}


	private:
		/// @name Template helper functions
			///@{
			template<typename TYPE>
			void MapItem(std::map<std::string, NTuple::Item<TYPE> > &map, const std::string &item_name);
			template<typename TYPE>
			void MapItem<TYPE>(const std::string &item_name);
			template<typename TYPE>
			void PrintItems(const std::map<std::string, NTuple::Item<TYPE> > &map);
			///@}
	};



/// @}
// * ================================ * //
// * ------- TEMPLATE METHODS ------- * //
// * ================================ * //


	/// Specialisation of `GetItems`.
	/// Can be used to easily access the different mappings for each type. You don't need to specialise the template methods below, because you can just define them in terms of `GetItems<TYPE>`.
	template<> const std::map<std::string, NTuple::Item<double> >* NTupleContainer::GetItems<double>() { return &fItems_double; }
	template<> const std::map<std::string, NTuple::Item<int> >*    NTupleContainer::GetItems<int>   () { return &fItems_int; }


	///
	template<typename TYPE>
	NTuple::Item<TYPE>& NTupleContainer::GetItem(const std::string &key)
	{
		GetItems<TYPE>()->at(key);
	}


	/// Easier and expanded version of `NTuple::Tuple::addItem`.
	/// You will need to call this method using e.g. `AddItem<double>` to specify that this is a `double`.
	template<typename TYPE>
	void NTupleContainer::AddItem(const std::string &item_name)
	{
		/// -# @b Abort if the `"write_"` job switch property has been set to `false`.
			if(!DoWrite()) return;
		/// -# @b Abort if `fTuple` has not been booked.
			if(!fTuple) {
				std::cout << "FATAL ERROR: NTuple \"" << Name() << "\" has not been booked, so cannot add item \"" << item_name << "\"" << std::endl;
				std::cout << "  --> Check your code" << std::endl;
				std::terminate();
			}
		/// -# Call `MapItem<TYPE>`.
			MapItem<TYPE>(item_name);
	}


	/// Helper function for `AddItem<TYPE>`.
	/// This function adds an item with name `item_name` to one a `map` of `NTuple::Item`s (namely to either `fItems_double`, `fItems_int`, etc.). You will have to specify which ones in the specialisation of `MapItem(const std::string&)`. This function will then be called in `AddItem`.
	template<typename TYPE>
	void NTupleContainer::MapItem(std::map<std::string, NTuple::Item<TYPE> > &map, const std::string &item_name)
	{
		/// -# Create an `NTuple::Item` using the `items` mapping.
		map[item_name];
		/// -# Create an `NTuple::Item` using the `items` mapping to the `fTuple` and to the map of `items`, if allowed by the `perform` job switch.
		map->addItem(item_name, map.at(item_name));
	}
	template<TYPE> void NTupleContainer::MapItem(const std::string &item_name)
	{
		MapItem(GetItems<TYPE>(), item_name); }
	template<> void NTupleContainer::MapItem<int>   (const std::string &item_name) { MapItem(fItems_int,    item_name); }


	/// Template helper for the `PrintTuplesAndItems` method.
	template<typename TYPE>
	void NTupleContainer::PrintItems()
	{
		std::map<std::string, NTuple::Item<TYPE> >::const_iterator it = GetItems<TYPE>()->begin();
		for(; it != GetItems<TYPE>()->end(); ++it)
			std::cout << "  [+] " << it->first << std::endl;
	}


#endif