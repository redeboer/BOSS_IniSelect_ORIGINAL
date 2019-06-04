#ifndef JpsiToDPV_TreePID_H
#define JpsiToDPV_TreePID_H

#include "JpsiToDPV/Trees/TreeContainer.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "ParticleID/ParticleID.h"

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreePID : public TreeContainer
{
public:
  TreePID(const char* name, const char* title = "") : TreeContainer(name, title)
  {
    BRANCH(p);
    BRANCH(cost);
    BRANCH(dedx);
    BRANCH(tofIB);
    BRANCH(tofOB);
    BRANCH(probp);
    BRANCH(probK);
  }
  Double_t p;     ///< Momentum of the track
  Double_t cost;  ///< Theta angle of the track
  Double_t dedx;  ///< \f$\chi^2\f$ of the \f$dE/dx\f$ of the track
  Double_t tofIB; ///< \f$\chi^2\f$ of the inner barrel ToF of the track
  Double_t tofOB; ///< \f$\chi^2\f$ of the outer barrel ToF of the track
  Double_t probp; ///< Probability that it is a pion
  Double_t probK; ///< Probability that it is a kaon
  void     Check(ParticleID* pid, EvtRecTrack* trk)
  {
    if(!write) return;
    RecMdcTrack* mdcTrk = trk->mdcTrack();

    p     = mdcTrk->p();          // momentum of the track
    cost  = cos(mdcTrk->theta()); // theta angle of the track
    dedx  = pid->chiDedx(2);      // Chi squared of the dedx of the track
    tofIB = pid->chiTof1(2);      // Chi squared of the inner barrel ToF of the track
    tofOB = pid->chiTof2(2);      // Chi squared of the outer barrel ToF of the track
    probp = pid->probPion();      // probability that it is a pion
    probK = pid->probKaon();      // probability that it is a kaon
    fTree->Fill();
  }
};
/// @}
#endif