#ifndef Physics_Analysis_D0omega_K4pi_H
#define Physics_Analysis_D0omega_K4pi_H

#include "D0omega_K4pi/FitResults/D0omega_K4pi.h"
#include "D0omega_K4pi/TreeCollection.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include <string>

#define DECLAREWRITE(TREE) declareProperty(TREE.PropertyName(), TREE.write)

/// @addtogroup BOSS_objects
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class Results_D0omega_KpiKK : public TreeCollection
{
public:
  Results_D0omega_KpiKK() :
    fit4c("fit4c"),
    fit5c("fit5c"),
    MC("topology")
  {}

  D0omega_K4pi_space::Fit4C     fit4c; ///< 4-constraint (4C) fit information
  D0omega_K4pi_space::Fit5C     fit5c; ///< 5-constraint (5C) fit information
  D0omega_K4pi_space::MCresults MC;
};
/// @}

/// @addtogroup BOSS_packages
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class D0omega_K4pi : public Algorithm
{
public:
  D0omega_K4pi(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  MsgStream          log;
  Results_D0omega_KpiKK f;

  std::string fFileName;

  // * Declare r0, z0 cut for charged tracks
  double fVr0cut;
  double fVz0cut;
  double fRvz0cut;
  double fRvxy0cut;

  // * Declare energy, dphi, dthe cuts for fake gamma's
  double fEnergyThreshold;
  double fGammaPhiCut;
  double fGammaThetaCut;
  double fGammaAngleCut;

  // * Declare whether to test the success of the 4- and 5-constraint fits
  bool   fDo_fit4c;
  bool   fDo_fit5c;
  double fMaxChiSq;
  double fMinPID;
};
/// @}
#endif