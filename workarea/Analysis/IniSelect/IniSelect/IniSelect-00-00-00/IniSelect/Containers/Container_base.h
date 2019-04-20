#ifndef Analysis_IniSelect_Container_base_H
#define Analysis_IniSelect_Container_base_H

#include <string>

/// @addtogroup BOSS_objects
/// @{

/// This class is the base class for objects requiring a name and a description. This can be useful for instance for defining derived containers containing properties and `NTuple`s. See [this page](https://besiii.gitbook.io/boss/the-boss-afterburner/initial/motivation#problems-with-booking-procedures) for the motivation.
/// @todo Encapsulate in `namespace` structure.
/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date   February 15th, 2019
class Container_base
{
public:
  Container_base(const std::string& name, const std::string& desc = "") :
    fName(name),
    fDescription(desc)
  {}
  virtual void SetDescription(const std::string& desc) { fDescription = desc; }
  virtual void SetName(const std::string& name) { fName = name; }
  virtual void SetName(const std::string& name, const std::string& desc)
  {
    SetName(name);
    SetDescription(desc);
  }

  const std::string& Name() const { return fName; }
  const std::string& Description() const { return fDescription; }

private:
  std::string fName;
  std::string fDescription;
};
/// @}
#endif