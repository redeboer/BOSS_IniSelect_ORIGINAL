// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "IniSelect/Containers/CutObject.h"
#include <float.h>
#include <iomanip>
#include <iostream>
#include <sstream>

// * =========================== * //
// * ------- CONSTRUCTOR ------- * //
// * =========================== * //

CutObject::CutObject(const std::string& name, const std::string& description) :
  Container_base(name, description),
  min(-DBL_MAX),
  max(DBL_MAX),
  counter(0)
{
  gCutObjects.push_back(this);
}

CutObject::~CutObject()
{
  gCutObjects.remove(this);
}

std::list<CutObject*> CutObject::gCutObjects;

// * =========================== * //
// * ------- CUT METHODS ------- * //
// * =========================== * //

bool CutObject::FailsCut(const double& value)
{
  if(value >= max || value <= min) return true;
  ++counter;
  return false;
}

bool CutObject::FailsMax(const double& value)
{
  if(value >= max) return true;
  ++counter;
  return false;
}

bool CutObject::FailsMin(const double& value)
{
  if(value <= min) return true;
  ++counter;
  return false;
}

// * =========================== * //
// * ------- INFORMATION ------- * //
// * =========================== * //

/// Print the cut name, `min` value, `max` value, and `counter` value of the cut object. If all
/// arguments are set, the output will be print in table format.
/// @param wname    With of the cut name column
/// @param wmin     With of the `min` column.
/// @param wmax     With of the `max` column.
/// @param wcounter With of the `counter` column.
void CutObject::Print(const int wname, const int wmin, const int wmax, const int wcounter) const
{
  /// -# Print a table line if required by `print`.
  if(wname && wmin && wmax && wcounter)
  {
    /// <ol>
    /// <li> Print cut name.
    std::cout << "  " << std::setw(wname) << std::left << Name();
    std::cout << " | ";
    /// <li> Print minimum if available.
    std::cout << std::setw(wmin) << std::right;
    if(min > -DBL_MAX)
      std::cout << min;
    else
      std::cout << "";
    std::cout << " | ";
    /// <li> Print maximum if available.
    std::cout << std::setw(wmax) << std::right;
    if(max < DBL_MAX)
      std::cout << max;
    else
      std::cout << "";
    std::cout << " | ";
    /// <li> Print counter if available.
    std::cout << std::setw(wcounter) << std::right << counter << std::endl;
    /// </ol>
  }
  else
  {
    // if only a min value
    if((min > -DBL_MAX) && (max == DBL_MAX))
    {
      std::cout << Name() << " > " << min << "    " << counter << std::endl;
      return;
    }
    // if only a max value
    if((max < DBL_MAX) && (min == -DBL_MAX))
    {
      std::cout << Name() << " < " << max << "    " << counter << std::endl;
      return;
    }
    // if both min and max value
    std::cout << min << " < " << Name() << " < " << max << "    " << counter << std::endl;
  }
}

// * ============================== * //
// * ------- STATIC MEMBERS ------- * //
// * ============================== * //

/// Print a table of cut names and their min/max and counter values.
void CutObject::PrintAll()
{
  /// -# Check if any instances of `CutObject` have been loaded.
  if(!CutObject::gCutObjects.size())
  {
    std::cout << std::endl << "WARNING: no cuts defined" << std::endl << std::endl;
    return;
  }
  /// -# Get widths of the columns.
  const std::string               h_name("CUT NAME");
  const std::string               h_min("MIN");
  const std::string               h_max("MAX");
  const std::string               h_count("COUNT");
  int                             w_name(h_name.size());
  int                             w_min(h_min.size());
  int                             w_max(h_max.size());
  int                             w_count(h_count.size());
  std::list<CutObject*>::iterator cut;
  for(cut = CutObject::gCutObjects.begin(); cut != CutObject::gCutObjects.end(); ++cut)
  {
    // * Name *
    if((*cut)->Name().size() > w_name) w_name = (*cut)->Name().size();
    // * Minimum value *
    if((*cut)->min > -DBL_MAX)
    {
      std::ostringstream os;
      os << (*cut)->min;
      if(os.str().size() > w_min) w_min = os.str().size();
    }
    // * Maximum value *
    if((*cut)->max < DBL_MAX)
    {
      std::ostringstream os;
      os << (*cut)->max;
      if(os.str().size() > w_max) w_max = os.str().size();
    }
    // * Count value *
    std::ostringstream os;
    os << (int)(*cut)->counter;
    if(os.str().size() > w_count) w_count = os.str().size();
  }
  /// -# Print header of the table.
  std::cout << std::endl
            << "  " << std::setw(w_name) << std::left << h_name << " | " << std::setw(w_min)
            << std::right << h_min << " | " << std::setw(w_max) << std::right << h_max << " | "
            << std::setw(w_count) << std::right << h_count << std::endl;
  /// -# Print horizontal line beneath it.
  std::cout << "  " << std::setfill('-') << std::setw(w_name + w_min + w_max + w_count + 9) << ""
            << std::endl;
  std::cout << std::setfill(' ');
  /// -# Print one row for each cut using `CutObject::Print`. Print a horizontal line just before
  /// counter `"N_events"`.
  for(cut = CutObject::gCutObjects.begin(); cut != CutObject::gCutObjects.end(); ++cut)
  {
    if(!(*cut)->Name().compare("N_events"))
    {
      std::cout << "  " << std::setfill('-') << std::setw(w_name + w_min + w_max + w_count + 9)
                << "" << std::endl;
      std::cout << std::setfill(' ');
    }
    (*cut)->Print(w_name, w_min, w_max, w_count);
  }
  /// -# Skip one line.
  std::cout << std::endl;
}