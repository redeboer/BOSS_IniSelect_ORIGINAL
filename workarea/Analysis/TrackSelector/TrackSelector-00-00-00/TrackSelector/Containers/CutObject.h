#ifndef Analysis_CutObject_H
#define Analysis_CutObject_H

#include "TString.h"
#include "TrackSelector/Containers/Container_base.h"
#include <list>
#include <string>

/// @addtogroup BOSS_objects
/// @{

/// This object contains all information and tools for a cut you want to apply.
/// It contains `min` and `max` value of the cut parameter, a counter for the number of events or
/// tracks that passed this cut test, as well as a name and description for the cut.
/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date   January 3rd, 2019
class CutObject : public Container_base
{
public:
  CutObject(const std::string& name, const std::string& description = "");
  ~CutObject();
  static std::list<CutObject*> gCutObjects;

  bool FailsCut(const double& value);
  bool FailsMax(const double& value);
  bool FailsMin(const double& value);

  bool operator<(const double& value) const { return value < max; }
  bool operator==(const double& value) const { return value < max && value > min; }
  bool operator>(const double& value) const { return value > min; }
  void operator++() { ++counter; }
  template <typename T>
  void operator+=(T incr)
  {
    counter += incr;
  }
  const char*        NameMax() const { return Form("cut_%s_max", Name().c_str()); }
  const char*        NameMin() const { return Form("cut_%s_min", Name().c_str()); }
  void               Print(const int wname = 0, const int wmin = 0, const int wmax = 0,
                           const int wcounter = 0) const;
  double             max;
  double             min;
  unsigned long long counter;
  static void        PrintAll();
};

/// @}
#endif