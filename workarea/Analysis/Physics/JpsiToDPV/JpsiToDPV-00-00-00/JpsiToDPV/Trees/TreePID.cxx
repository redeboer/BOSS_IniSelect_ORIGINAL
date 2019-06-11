#include "JpsiToDPV/Trees/TreePID.h"
#include "IniSelect/Handlers/PID.h"
#include "MdcRecEvent/RecMdcTrack.h"

void TreePID::Fill(EvtRecTrack* trk)
{
  if(!write) return;
  RecMdcTrack* mdcTrk = trk->mdcTrack();

  p     = mdcTrk->p();          // momentum of the track
  cost  = cos(mdcTrk->theta()); // theta angle of the track
  dedx  = PID::GetChiTofE();
  tofIB = PID::GetChiTofIB();
  tofOB = PID::GetChiTofOB();
  tofOB = PID::GetChiDedx();
  probp = PID::GetProbPion();
  probK = PID::GetProbKaon();
  fTree->Fill();
}