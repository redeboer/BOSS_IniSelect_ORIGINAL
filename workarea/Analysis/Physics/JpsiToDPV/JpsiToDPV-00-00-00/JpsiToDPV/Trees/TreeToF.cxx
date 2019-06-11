#include "JpsiToDPV/Trees/TreeToF.h"
#include "IniSelect/Globals.h"
#include "DstEvent/TofHitStatus.h"

using namespace IniSelect;

void TreeToF::Fill(RecTofTrack* trk, Double_t ptrk)
{
  if(!write) return;
  p     = ptrk;
  path  = trk->path();  // distance of flight
  tof   = trk->tof();   // time of flight
  ph    = trk->ph();    // ToF pulse height
  zrhit = trk->zrhit(); // Track extrapolate Z or R Hit position
  qual  = 0. + trk->quality(); // data quality of reconstruction
  cntr  = 0. + trk->tofID();   // ToF counter ID
  Double_t texp[5];
  for(Int_t j = 0; j < 5; j++)
  {
    Double_t gb   = ptrk / Mass::TOF[j]; // v = p/m (non-relativistic velocity)
    Double_t beta = gb / sqrt(1 + gb * gb);
    texp[j]     = 10 * path / beta / Physics::SpeedOfLight;
  }
  te  = tof - texp[0]; // difference with ToF in electron hypothesis
  tmu = tof - texp[1]; // difference with ToF in muon hypothesis
  tpi = tof - texp[2]; // difference with ToF in charged pion hypothesis
  tk  = tof - texp[3]; // difference with ToF in charged kaon hypothesis
  tp  = tof - texp[4]; // difference with ToF in proton hypothesis
  fTree->Fill();
}

void TreeToF::WriteToF(vector<EvtRecTrack*>& vec, TreeToF& TofIB, TreeToF& TofOB, TreeToF& TofEC)
{
  if(!TofIB.write && !TofOB.write && !TofEC.write) return;
  vector<EvtRecTrack*>::iterator it = vec.begin();
  for(auto trk : vec)
  {
    if(!trk->isMdcTrackValid()) continue;
    if(!trk->isTofTrackValid()) continue;

    RecMdcTrack*                mdcTrk    = trk->mdcTrack();
    SmartRefVector<RecTofTrack> tofTrkCol = trk->tofTrack();

    double ptrk = mdcTrk->p();

    SmartRefVector<RecTofTrack>::iterator it_tof = tofTrkCol.begin();
    for(; it_tof != tofTrkCol.end(); ++it_tof)
    {
      TofHitStatus hitStatus;
      hitStatus.setStatus((*it_tof)->status());
      if(!hitStatus.is_barrel())
      {
        if(!(hitStatus.is_counter())) continue; // ?
        if(hitStatus.layer() == 0) TofEC.Fill(*it_tof, ptrk);
      }
      else
      {
        if(!hitStatus.is_counter()) continue;
        if(hitStatus.layer() == 1)
          TofIB.Fill(*it_tof, ptrk);
        else
          if(hitStatus.layer() == 2) TofOB.Fill(*it_tof, ptrk);
      }
    }
  } // loop all charged track
}