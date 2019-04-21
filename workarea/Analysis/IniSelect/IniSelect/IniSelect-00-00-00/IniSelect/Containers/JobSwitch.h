#ifndef Analysis_IniSelect_JobSwitch_H
#define Analysis_IniSelect_JobSwitch_H

#include "IniSelect/Containers/Container_base.h"
#include "TString.h"
#include <list>
#include <string>

/// @addtogroup BOSS_objects
/// @{

/// This simple object encapsulates a `bool`ean job option.
/// Use a `JobSwitch` instead of a simple `bool`ean if you want to be able to set the switch during runtime through the joboptions file. Implementation in the code: The constructor only needs a name for this switch. It is convention to start this name with e.g. `"write_"` if this determines whether or not to write a certain `TTree`/`NTuple`.
/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date   January 3rd, 2019
class JobSwitch : public Container_base
{
public:
  JobSwitch(const std::string& name, const std::string& description = "", const bool val = true);
  ~JobSwitch();
  static std::list<JobSwitch*> gJobSwitches;
  static void                  PrintSwitches();
  explicit   operator bool() const { return fValue; } /// Convertor for `bool`ean arithmatic.
  const bool Value() const { return fValue; }         /// Read access to `fValue` data member.
  void SetValue(const bool value) { fValue = value; } /// Write access to `fValue` data member.

  // private:
  bool fValue;
  /// The actual switch. Not set to `private` because of `Algorithm::declareProperty` needs it in `IniSelect`.
};

/// @}
#endif