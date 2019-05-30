#ifndef JpsiToDPV_TreeMC_H
#define JpsiToDPV_TreeMC_H

#include "JpsiToDPV/Trees/TreeContainer.h"

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeMC : public TreeContainer
{
public:
  TreeMC(const char* name, const char* title) : TreeContainer(name, title)
  {
    BRANCH(runid);
    BRANCH(evtid);
    BRANCH(n);
    BRANCH(PDG);
    BRANCH(mother);
  }
  Int_t              runid;  ///< Run number ID.
  Int_t              evtid;  ///< Rvent number ID.
  Int_t              n;      ///< Number of MC particles stored for this event.
  std::vector<Int_t> PDG;    ///< PDG code for the particle in this array.
  std::vector<Int_t> mother; ///< Track index of the mother particle.
};
/// @}
#endif