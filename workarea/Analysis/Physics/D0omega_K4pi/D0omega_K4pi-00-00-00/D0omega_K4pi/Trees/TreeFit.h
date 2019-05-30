#ifndef IniSelect_TreeFit_H
#define IniSelect_TreeFit_H

#include "D0omega_K4pi/Trees/Tree.h"
#define BRANCHMOM(VAR) BranchMomObj(TO_STRING(VAR), VAR)

struct MomObj
{
  Double_t m;
  Double_t p;
};

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeFit : public Tree
{
public:
  TreeFit(const char* name, const char* title = "") : Tree(name, title)
  {
    BRANCH(chi2);
  }
  Double_t chi2;

protected:
  void BranchMomObj(const char* name, MomObj& obj)
  {
    Branch(Form("m%s", name), &obj.m);
    Branch(Form("p%s", name), &obj.p);
  }
};
/// @}
#endif