// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "TrackSelector/Containers/NTupleContainer.h"
#include "TString.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <utility> // for make_pair

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //

/// The constructor attempts to add the `NTuple::Tuple` to the `instances` mapping.
NTupleContainer::NTupleContainer(const std::string& name, const std::string& description,
                                 const bool write) :
  Container_base(name, description),
  fEntries(0),
  fTuple(nullptr),
  fWrite(Form("write_%s", name.c_str()), description, write)
{
  /// -# Abort if this `NTuple` key name already exists in the `instances` mapping.
  if(instances.find(name) != instances.end())
  {
    std::cout << "FATAL ERROR: NTuple \"" << name << "\" already exists!" << std::endl;
    std::terminate(); /// @warning Contains fatal `terminate` statement when double booking!
                      /// Consider turning into a `return` statement.
  }
  /// -# Insert a key to the `instances` mapping.
  instances.insert(make_pair(name, this));
}

/// The destructor removes this instances from the `instances` inventory.
NTupleContainer::~NTupleContainer()
{
  instances.erase(Name());
}

std::map<std::string, NTupleContainer*> NTupleContainer::instances;

// * =============================== * //
// * ------- NTUPLE HANDLERS ------- * //
// * =============================== * //

/// Write `NTuple` if `"write_"` `JobSwitch` has been set to `true`.
void NTupleContainer::Write()
{
  if(!fTuple) return;
  if(!DoWrite()) return;
  ++fEntries;
  fTuple->write();
}

/// Print table of `NTuple`s existing in `instances` with their entries.
void NTupleContainer::PrintFilledTuples()
{
  /// -# @b Abort if no `NTuple`s exist in `instances`.
  if(!instances.size()) return;

  /// -# Get name widths and `fEntries` widths.
  size_t                                            w_namemax(0), w_valmax(0), nentries(0);
  std::map<std::string, NTupleContainer*>::iterator it;
  for(it = instances.begin(); it != instances.end(); ++it)
  {
    if(!it->second->GetEntries()) continue;
    ++nentries;
    size_t w_val(ceil(log10(it->second->GetEntries())));
    if(it->first.size() > w_namemax) w_namemax = it->first.size();
    if(w_val > w_valmax) w_valmax = w_val;
  }
  ++w_valmax;

  /// -# @b Abort if there are no entries that have been filled.
  if(!nentries) return;

  /// -# Print header.
  std::cout << std::endl << "  There are " << nentries << " filled NTuples:" << std::endl;

  /// -# Print a row for each `NTupleContainer` in `instances` with name and number of entries
  for(it = instances.begin(); it != instances.end(); ++it)
  {
    if(!it->second->GetEntries()) continue;
    std::cout << "    ";
    std::cout << std::left << std::setw(w_namemax) << it->first << " : ";
    std::cout << std::right << std::setw(w_valmax) << it->second->GetEntries() << " entries";
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

/// Print list of `NTuple`s existing in `instances` with their items.
void NTupleContainer::PrintTuplesAndItems()
{
  /// <ol>
  /// <li> @b Abort if no `NTuple`s exist in `instances`.
  if(!instances.size()) return;
  /// <li> Print header line.
  std::cout << std::endl
            << std::endl
            << "THERE ARE " << instances.size() << " BOOKED NTUPLES" << std::endl;
  /// <li> Print a row for each `NTupleContainer` in `instances`.
  std::map<std::string, NTupleContainer*>::iterator it;
  for(it = instances.begin(); it != instances.end(); ++it)
  {
    /// <ol>
    /// <li> Print name and description.
    std::cout << std::endl << "NTuple \"" << it->first << "\" (write = ";
    /// <li> Print write `JobSwitch` and number of entries if available.
    std::cout << it->second->DoWrite();
    if(it->second->GetEntries()) std::cout << " | entries = " << it->second->GetEntries();
    std::cout << "):" << std::endl;
    /// <li> Print name and description.
    std::cout << it->second->Description() << std::endl;
    /// <li> Print list of added items.
    it->second->PrintItems<double>();
    it->second->PrintItems<int>();
    /// </ol>
  }
  /// </ol>
}

// * =============================== * //
// * ------- PRIVATE METHODS ------- * //
// * =============================== * //

/// Private helper function that terminates and prints a message if `fTuple` is a `nullptr`.
/// Has been added to avoid code duplication in `AddItem` and `AddIndexedItem`.
void NTupleContainer::PrintAtExceptionError(const std::string& key)
{
  std::cout << "FATAL ERROR: Could not find key \"" << key << "\" in NTupleContainer \"" << Name()
            << "\"" << std::endl;
  std::cout << "   -->> Check whether you called the correct type when you called "
               "GetArray<typename> or GetItem<typename>"
            << std::endl;
  std::terminate();
}