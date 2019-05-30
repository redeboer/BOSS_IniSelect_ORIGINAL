#ifndef IniSelect_TreePID_H
#define IniSelect_TreePID_H

#include "D0omega_K4pi/Trees/Tree.h"

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreePID : public Tree
{
public:
  TreePID(const char* name, const char* title = "") : Tree(name, title)
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
};
/// @}
#endif