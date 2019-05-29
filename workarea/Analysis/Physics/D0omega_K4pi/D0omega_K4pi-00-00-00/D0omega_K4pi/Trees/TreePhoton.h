#ifndef IniSelect_TreePhoton_H
#define IniSelect_TreePhoton_H

#include "D0omega_K4pi/Trees/Tree.h"

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreePhoton : public Tree
{
public:
  TreePhoton() : Tree("photon", "Photon kinematics")
  {
    BRANCH(dthe);
    BRANCH(dphi);
    BRANCH(dang);
    BRANCH(eraw);
  }
  Double_t dthe; ///< \f$\theta\f$ angle difference with nearest charged track (degrees)
  Double_t dphi; ///< \f$\phi\f$ angle difference with nearest charged track (degrees)
  Double_t dang; ///< Angle difference with nearest charged track
  Double_t eraw; ///< Energy of the photon
};
/// @}
#endif