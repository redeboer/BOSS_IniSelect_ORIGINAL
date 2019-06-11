#include "JpsiToDPV/Trees/TreeMC.h"
#include "IniSelect/Globals.h"
#include <iostream>
using namespace std;
using namespace IniSelect::McTruth;

void TreeMC::SetMC(SmartDataPtr<Event::McParticleCol> mcParticleCol)
{
  if(!mcParticleCol)
  {
    cout << "Could not retrieve McParticelCol" << endl;
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
    if(IsJPsi(*it))
    {
      incPdcy   = true;
      rootIndex = (*it)->trackIndex();
    }
    if(!incPdcy) continue;
    PDG.push_back((*it)->particleProperty());
    if(IsFromJPsi(*it))
      mother.push_back((*it)->mother().trackIndex() - rootIndex);
    else
      mother.push_back((*it)->mother().trackIndex() - rootIndex - 1);
    if(IsJPsi(*it)) mother[n] = 0;
    ++n;
  }
}