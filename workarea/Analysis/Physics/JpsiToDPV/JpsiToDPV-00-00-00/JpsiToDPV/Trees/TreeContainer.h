#ifndef JpsiToDPV_TreeContainer_H
#define JpsiToDPV_TreeContainer_H

#include "TTree.h"
#define TO_STRING(x) #x
#define BRANCH(VAR) Branch(TO_STRING(VAR), &VAR)

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeContainer
{
public:
  TreeContainer(const char* name, const char* title = "") { fTree = new TTree(name, title); }
  ~TreeContainer() { delete fTree; }

  Bool_t write;
  void   Fill()
  {
    if(write && fTree) fTree->Fill();
  }
  void Write()
  {
    if(write && fTree) fTree->Write();
  }
  Int_t GetEntries()
  {
    if(fTree) return fTree->GetEntries();
    else return -1;
  }
  const char* PropertyName() const { return Form("write_%s", fTree->GetName()); }

private:
  TTree* fTree;
};
/// @}
#endif