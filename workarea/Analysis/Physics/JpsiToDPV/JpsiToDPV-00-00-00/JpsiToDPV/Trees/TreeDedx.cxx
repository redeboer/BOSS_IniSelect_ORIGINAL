#include "JpsiToDPV/Trees/TreeDedx.h"

void TreeDedx::Fill(const vector<EvtRecTrack*>& vec)
{
  if(!write) return;
  for(const auto it : vec)
  {
    if(!it->isMdcTrackValid()) continue;
    if(!it->isMdcDedxValid()) continue;
    RecMdcTrack* mdcTrk  = it->mdcTrack();
    RecMdcDedx*  dedxTrk = it->mdcDedx();

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
    fTree->Fill();
  }
}