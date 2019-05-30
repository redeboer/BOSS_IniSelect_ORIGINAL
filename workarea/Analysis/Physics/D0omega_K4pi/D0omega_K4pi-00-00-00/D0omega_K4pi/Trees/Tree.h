#ifndef IniSelect_Tree_H
#define IniSelect_Tree_H

#include "TTree.h"
#define TO_STRING(x) #x
#define BRANCH(VAR) Branch(TO_STRING(VAR), &VAR)

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class Tree : public TTree
{
public:
  Tree(const char *name, const char *title = "") : TTree(name, title) {}
  Bool_t write;
  void FillSafe()
  {
    if(write) Fill();
  }
  void WriteSafe()
  {
    if(write) Write();
  }
  const char* PropertyName() const
  {
    return Form("write_%s", GetName());
  }
};
/// @}
#endif