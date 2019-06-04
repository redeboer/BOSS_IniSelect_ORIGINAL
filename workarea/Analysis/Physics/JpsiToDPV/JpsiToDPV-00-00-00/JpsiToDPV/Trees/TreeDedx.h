#ifndef JpsiToDPV_TreeDedx_H
#define JpsiToDPV_TreeDedx_H

#include "JpsiToDPV/Trees/TreeContainer.h"
#include <vector>

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeDedx : public TreeContainer
{
public:
  TreeDedx() : TreeContainer("dedx", "Energy loss dE/dx")
  {
    BRANCH(p);
    BRANCH(chie);
    BRANCH(chimu);
    BRANCH(chipi);
    BRANCH(chik);
    BRANCH(chip);
    BRANCH(probPH);
    BRANCH(normPH);
    BRANCH(ghit);
    BRANCH(thit);
  }

  Double_t p;      ///< Momentum of the track
  Double_t chie;   ///< \f$\chi^2\f$ in case of electron
  Double_t chimu;  ///< \f$\chi^2\f$ in case of muon
  Double_t chipi;  ///< \f$\chi^2\f$ in case of pion
  Double_t chik;   ///< \f$\chi^2\f$ in case of kaon
  Double_t chip;   ///< \f$\chi^2\f$ in case of proton
  Double_t probPH; ///< Most probable pulse height from truncated mean
  Double_t normPH; ///< Normalized pulse height
  Double_t ghit;   ///< Number of good hits
  Double_t thit;   ///< Total number of hits

  void Fill(const vector<EvtRecTrack*>& vec)
  {
    if(!write) return;
    for(vector<EvtRecTrack*>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
      if(!(*it)->isMdcTrackValid()) continue;
      if(!(*it)->isMdcDedxValid()) continue;
      RecMdcTrack* mdcTrk  = (*it)->mdcTrack();
      RecMdcDedx*  dedxTrk = (*it)->mdcDedx();

      p      = mdcTrk->p();            // momentum of the track
      chie   = dedxTrk->chiE();        // chi2 in case of electron
      chimu  = dedxTrk->chiMu();       // chi2 in case of muon
      chipi  = dedxTrk->chiPi();       // chi2 in case of pion
      chik   = dedxTrk->chiK();        // chi2 in case of kaon
      chip   = dedxTrk->chiP();        // chi2 in case of proton
      probPH = dedxTrk->probPH();      // most probable pulse height from truncated mean
      normPH = dedxTrk->normPH();      // normalized pulse height
      ghit   = dedxTrk->numGoodHits(); // number of good hits
      thit   = dedxTrk->numTotalHits(); // total number of hits
      Fill();
    }
  }
};
/// @}
#endif