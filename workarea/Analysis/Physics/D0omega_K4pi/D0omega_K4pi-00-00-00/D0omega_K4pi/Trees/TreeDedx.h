#ifndef IniSelect_TreeDedx_H
#define IniSelect_TreeDedx_H

#include "D0omega_K4pi/Trees/Tree.h"

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeDedx : public Tree
{
public:
  TreeDedx() : Tree("dedx", "Energy loss dE/dx")
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
};
/// @}
#endif