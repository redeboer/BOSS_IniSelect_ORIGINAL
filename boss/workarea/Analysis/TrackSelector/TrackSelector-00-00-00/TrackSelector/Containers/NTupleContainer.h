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
	#include <iostream>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_objects
/// @{

	/// Container object that is to facilitate the booking procedure for `NTuple::Tuple`s and their `NTuple::Item`s.
	/// @remark This object can only be used within a derived class off the Gaudi `Algorithm` class because its `fTuple` requires to be booked by it (using e.g. `TrackSelector::BookNTuples`).
	/// @remark To allow for the addition of different types of items (such as integers), there are several mappings of the items (see for instance `fItems_double` and `fItems_int`). If you add new types, make sure to add them to this list of `fItems_*`, where `*` should be the `typename` and to add a template specialisation for `GetItems`. The rest of the templates then don't require template specialisation, as they can be defined in terms of `GetItems`. However, not that you will have to add the new mappings to functions like `PrintTuplesAndItems`, as you can not loop over templates.
	/// @todo Think about a solution for indexed items.
	/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// See <a href="https://besiii.gitbook.io/boss/the-boss-afterburner/initial/motivation#problems-with-booking-procedures">this page</a> for the motivation. This function also automatically declares a corresponding `JobSwitch` property.
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
			template<typename TYPE> inline
			void PrintItems();
			///@}


		/// @name Setters
			///@{
			void SetTuplePtr(NTuplePtr& data) { fTuple = data.ptr(); }
			///@}


		/// @name Getters
			///@{
			template<typename TYPE> inline
			NTuple::Item<TYPE>& GetItem(const std::string &key);
			const ULong_t GetEntries() const { return fEntries; } ///< Read access to `fEntries`.
			///@}


		/// @name NTuple handlers and members
			///@{
			template<typename TYPE> inline
			void AddItem(const std::string &item_name);
			void AddItem(const std::string &item_name) { AddItem<double>(item_name); }; ///< Shortcut for `AddItem<double>`, added for backward compatibilaty.
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
			template<typename TYPE> inline
			std::map<std::string, NTuple::Item<TYPE> >* GetItems();
			JobSwitch fWrite;   ///< Boolean job property that determines whether or not to write data stored to this `NTuple` to a `TTree` (property name starts with `"write_"` by default).
			NTuple::Tuple *fTuple; ///< Pointer to the encapsulated `NTuple::Tuple`.
			ULong_t fEntries;
			std::map<std::string, NTuple::Item<double> > fItems_double; ///< Inventory of added `double` items.
			std::map<std::string, NTuple::Item<int> >    fItems_int;    ///< Inventory of added `int` items.
			///@}
	};



/// @}
// * ================================ * //
// * ================================ * //


	/// Specialisation of `GetItems`.
	template<> inline std::map<std::string, NTuple::Item<double> >* NTupleContainer::GetItems<double>() { return &fItems_double; }
	template<> inline std::map<std::string, NTuple::Item<int> >*    NTupleContainer::GetItems<int>   () { return &fItems_int; }


	/// Method that is comparable to `std::map::at`, but allows to access the diffent types of maps (`fItems_double` etc) through a template call of this method.
	/// Call e.g. using `GetItem<double>("fit4c")`.
	template<typename TYPE> inline
	NTuple::Item<TYPE>& NTupleContainer::GetItem(const std::string &key)
	{
		try {
			return GetItems<TYPE>()->at(key);
		} catch(std::exception) {
			std::cout << "FATAL ERROR: Could not find key \"" << key << "\" in NTupleContainer \"" << Name() << "\"" << std::endl;
			std::cout << "   -->> Check whether you called the correct type in GetItem<typename>" << std::endl;
			std::terminate();
		}
	}


	/// Easier and expanded version of `NTuple::Tuple::addItem`.
	/// You will need to call this method using e.g. `AddItem<double>` to specify that this is a `double`.
	template<typename TYPE> inline
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
		/// -# Create an `NTuple::Item` using the `operator[]` of a `std::map`.
			(*GetItems<TYPE>())[item_name];
		/// -# Add the `NTuple::Item<TYPE>` that you created in the previous step to the `fTuple`.
			fTuple->addItem(item_name, GetItems<TYPE>()->at(item_name));
	}


	/// Template helper for the `PrintTuplesAndItems` method.
	template<typename TYPE> inline
	void NTupleContainer::PrintItems()
	{
		typename std::map<std::string, NTuple::Item<TYPE> >::const_iterator it = GetItems<TYPE>()->begin();
		for(; it != GetItems<TYPE>()->end(); ++it)
			std::cout << "  [+] " << it->first << std::endl;
	}


#endif