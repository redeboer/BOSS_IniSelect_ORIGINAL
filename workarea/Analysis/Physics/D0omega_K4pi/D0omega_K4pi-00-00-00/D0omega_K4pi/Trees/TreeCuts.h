#ifndef IniSelect_TreeCuts_H
#define IniSelect_TreeCuts_H

#include "D0omega_K4pi/Trees/Tree.h"
#include <vector>

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeCuts : public Tree
{
public:
  TreeCuts() : Tree("vxyz", "dCuts information of the charged tracks")
  {
    BRANCH(cuts);
  }
  ULong64_t& operator[](size_t i) { return cuts[i]; }
  std::vector<ULong64_t> cuts;
};
/// @}
#endif