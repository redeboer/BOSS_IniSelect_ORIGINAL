#ifndef JpsiToDPV_TreeFit_H
#define JpsiToDPV_TreeFit_H

#include "JpsiToDPV/Trees/TreeContainer.h"
#define BRANCHMOM(OBJ) \
  fTree->Branch(Form("m%s", TO_STRING(OBJ)), &OBJ.m); \
  fTree->Branch(Form("p%s", TO_STRING(OBJ)), &OBJ.p);

struct MomObj
{
  Double_t m;
  Double_t p;
};

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeFit : public TreeContainer
{
public:
  TreeFit(const char* name, const char* title = "") : TreeContainer(name, title)
  {
    BRANCH(chi2);
  }
  Double_t chi2;
};
/// @}
#endif