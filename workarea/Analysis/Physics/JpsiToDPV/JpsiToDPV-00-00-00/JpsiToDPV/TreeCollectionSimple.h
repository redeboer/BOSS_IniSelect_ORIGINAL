#ifndef JpsiToDPV_TreeCollectionSimple_H
#define JpsiToDPV_TreeCollectionSimple_H

#include "JpsiToDPV/Trees/TreeCuts.h"
#include "JpsiToDPV/Trees/TreeDedx.h"
#include "JpsiToDPV/Trees/TreePID.h"
#include "JpsiToDPV/Trees/TreePhoton.h"
#include "JpsiToDPV/Trees/TreeToF.h"
#include "JpsiToDPV/Trees/TreeVertex.h"

/// @addtogroup BOSS_objects
/// @{

/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeCollectionSimple
{
public:
  TreeCollectionSimple() :
    TofEC("TofEC", "End cap ToF decector"),
    TofIB("TofIB", "Inner barrel ToF decector"),
    TofOB("TofOB", "Outer barrel ToF decector"),
    PID("PID")
  {}

  TreeCuts   cuts;
  TreeVertex v;      ///< Vertex information of the charged tracks
  TreePhoton photon; ///< Photon kinematics
  TreeDedx   dedx;   ///< Energy loss dE/dx
  TreeToF    TofEC;  ///< End cap ToF decector
  TreeToF    TofIB;  ///< Inner barrel ToF decector
  TreeToF    TofOB;  ///< Outer barrel ToF decector
  TreePID    PID;    ///< Particle ID info
};

/// @}
#endif