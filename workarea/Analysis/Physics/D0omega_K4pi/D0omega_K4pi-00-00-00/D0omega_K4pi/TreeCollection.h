#ifndef IniSelect_TreeCollection_H
#define IniSelect_TreeCollection_H

#include "D0omega_K4pi/Trees/TreeCuts.h"
#include "D0omega_K4pi/Trees/TreeDedx.h"
#include "D0omega_K4pi/Trees/TreePID.h"
#include "D0omega_K4pi/Trees/TreePhoton.h"
#include "D0omega_K4pi/Trees/TreeToF.h"
#include "D0omega_K4pi/Trees/TreeVertex.h"

/// @addtogroup BOSS_objects
/// @{

/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class TreeCollection
{
public:
  TreeCollection() :
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