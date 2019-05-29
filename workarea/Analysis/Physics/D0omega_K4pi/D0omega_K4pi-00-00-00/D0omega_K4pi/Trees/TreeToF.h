#ifndef IniSelect_TreeToF_H
#define IniSelect_TreeToF_H

#include "D0omega_K4pi/Trees/Tree.h"

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeToF : public Tree
{
public:
  TreeToF(const char* name, const char* title) : Tree(name, title)
  {
    BRANCH(ptrk);
    BRANCH(path);
    BRANCH(tof);
    BRANCH(cntr);
    BRANCH(ph);
    BRANCH(zhit);
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
};
/// @}
#endif