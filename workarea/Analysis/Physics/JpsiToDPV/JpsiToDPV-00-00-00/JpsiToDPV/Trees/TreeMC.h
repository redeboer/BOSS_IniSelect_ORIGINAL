#ifndef JpsiToDPV_TreeMC_H
#define JpsiToDPV_TreeMC_H

#include "JpsiToDPV/Trees/TreeContainer.h"
#include "McTruth/McParticle.h"
#include <iostream>

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
    BRANCH(PDG);
    BRANCH(mother);
  }
  Int_t              runid;  ///< Run number ID.
  Int_t              evtid;  ///< Rvent number ID.
  std::vector<Int_t> PDG;    ///< PDG code for the particle in this array.
  std::vector<Int_t> mother; ///< Track index of the mother particle.

  void Fill(SmartDataPtr<Event::McParticleCol> mcParticleCol)
  {
    if(!mcParticleCol)
    {
      std::cout << "Could not retrieve McParticelCol" << std::endl;
      return;
    }
    Int_t n = 0;
    bool doNotInclude(true);
    int  indexOffset = -1;
    bool incPdcy(false);
    int  rootIndex(-1);

    Event::McParticleCol::iterator it = mcParticleCol->begin();
    PDG.clear();
    mother.clear();
    for(; it != mcParticleCol->end(); it++)
    {
      if((*it)->primaryParticle()) continue;
      if(!(*it)->decayFromGenerator()) continue;
      if((*it)->particleProperty() == incPid)
      {
        incPdcy   = true;
        rootIndex = (*it)->trackIndex();
      }
      if(!incPdcy) continue;
      PDG.push_back((*it)->particleProperty());
      if((*it)->mother().particleProperty() == incPid)
        mother.push_back((*it)->mother().trackIndex() - rootIndex);
      else
        mother.push_back((*it)->mother().trackIndex() - rootIndex - 1);
      if((*it)->particleProperty() == incPid) mother[n] = 0;
      ++n;
    }
    fTree->Fill();
  }
};
/// @}
#endif