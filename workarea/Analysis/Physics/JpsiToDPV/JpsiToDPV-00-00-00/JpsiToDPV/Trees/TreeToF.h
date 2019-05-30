#ifndef JpsiToDPV_TreeToF_H
#define JpsiToDPV_TreeToF_H

#include "JpsiToDPV/Trees/TreeContainer.h"
#include "JpsiToDPV/Globals.h"
#include "DstEvent/TofHitStatus.h"


/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeToF : public TreeContainer
{
public:
  TreeToF(const char* name, const char* title) : TreeContainer(name, title)
  {
    BRANCH(p);
    BRANCH(path);
    BRANCH(tof);
    BRANCH(cntr);
    BRANCH(ph);
    BRANCH(zrhit);
    BRANCH(qual);
    BRANCH(te);
    BRANCH(tmu);
    BRANCH(tpi);
    BRANCH(tk);
    BRANCH(tp);
  }

  Double_t p;     ///< Momentum of the track as reconstructed by MDC
  Double_t path;  ///< Path length
  Double_t tof;   ///< Time of flight
  Double_t cntr;  ///< ToF counter ID
  Double_t ph;    ///< ToF pulse height
  Double_t zrhit; ///< Track extrapolate Z or R Hit position
  Double_t qual;  ///< Data quality of reconstruction
  Double_t te;    ///< Difference with ToF in electron hypothesis
  Double_t tmu;   ///< Difference with ToF in muon hypothesis
  Double_t tpi;   ///< Difference with ToF in charged pion hypothesis
  Double_t tk;    ///< Difference with ToF in charged kaon hypothesis
  Double_t tp;    ///< Difference with ToF in proton hypothesis

  void Check(RecTofTrack* trk, Double_t ptrk)
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
      Double_t gb   = ptrk / IniSelect::xmass[j]; // v = p/m (non-relativistic velocity)
      Double_t beta = gb / sqrt(1 + gb * gb);
      texp[j]     = 10 * path / beta / IniSelect::velc_mm; // hypothesis ToF
    }
    te  = tof - texp[0]; // difference with ToF in electron hypothesis
    tmu = tof - texp[1]; // difference with ToF in muon hypothesis
    tpi = tof - texp[2]; // difference with ToF in charged pion hypothesis
    tk  = tof - texp[3]; // difference with ToF in charged kaon hypothesis
    tp  = tof - texp[4]; // difference with ToF in proton hypothesis
    Fill();
  }
  static void WriteToF(vector<EvtRecTrack*>& vec, TreeToF& TofIB, TreeToF& TofOB, TreeToF& TofEC)
  {
    if(!TofIB.write && !TofOB.write && !TofEC.write) return;
    vector<EvtRecTrack*>::iterator it = vec.begin();
    for(; it != vec.end(); ++it)
    {
      if(!(*it)->isMdcTrackValid()) continue;
      if(!(*it)->isTofTrackValid()) continue;

      RecMdcTrack*                mdcTrk    = (*it)->mdcTrack();
      SmartRefVector<RecTofTrack> tofTrkCol = (*it)->tofTrack();

      double ptrk = mdcTrk->p();

      SmartRefVector<RecTofTrack>::iterator it_tof = tofTrkCol.begin();
      for(; it_tof != tofTrkCol.end(); ++it_tof)
      {
        TofHitStatus hitStatus;
        hitStatus.setStatus((*it_tof)->status());
        if(!hitStatus.is_barrel())
        {
          if(!(hitStatus.is_counter())) continue; // ?
          if(hitStatus.layer() == 0) TofEC.Check(*it_tof, ptrk);
        }
        else
        {
          if(!hitStatus.is_counter()) continue;
          if(hitStatus.layer() == 1)
            TofIB.Check(*it_tof, ptrk);
          else
            if(hitStatus.layer() == 2) TofOB.Check(*it_tof, ptrk);
        }
      }
    } // loop all charged track
  }
};
/// @}
#endif