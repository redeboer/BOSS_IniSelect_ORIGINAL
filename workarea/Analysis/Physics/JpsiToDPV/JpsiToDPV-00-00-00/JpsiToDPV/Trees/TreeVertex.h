#ifndef JpsiToDPV_TreeVertex_H
#define JpsiToDPV_TreeVertex_H

#include "JpsiToDPV/Trees/Tree.h"

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeVertex : public Tree
{
public:
  TreeVertex() : Tree("vxyz", "Vertex information of the charged tracks")
  {
    BRANCH(p);
    BRANCH(x);
    BRANCH(y);
    BRANCH(z);
    BRANCH(r);
    BRANCH(rxy);
    BRANCH(rz);
    BRANCH(rphi);
  }
  Double_t p;    ///< Momentum
  Double_t x;    ///< Primary \f$x\f$-vertex as determined by MDC
  Double_t y;    ///< Primary \f$y\f$-vertex as determined by MDC
  Double_t z;    ///< Primary \f$z\f$-vertex as determined by MDC
  Double_t r;    ///< Distance from origin in \f$xy\f$-plane
  Double_t phi;
  Double_t rxy;  ///< Nearest distance to IP in \f$xy\f$ plane
  Double_t rz;   ///< Nearest distance to IP in \f$z\f$ direction
  Double_t rphi; ///< Angle in the \f$xy\f$-plane (?)
};
/// @}
#endif