#ifndef Physics_JpsiToDPV_H
#define Physics_JpsiToDPV_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "JpsiToDPV/TrackCollection.h"
#include "JpsiToDPV/TreeCollection.h"
#include "JpsiToDPV/Types/D0omega.h"
#include "ParticleID/ParticleID.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include <string>

#define DECLAREWRITE(TREE) declareProperty(TREE.PropertyName(), TREE.write)

/// @addtogroup BOSS_packages
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class JpsiToDPV : public Algorithm
{
public:
  JpsiToDPV(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  MsgStream   log;
  std::string fFileName;

  static ParticleID*         pid;
  static VertexFit*          vtxfit;
  static KalmanKinematicFit* kkmfit;

  TrackCollection  tracks;
  TreeCollection   fTrees;
  D0omega::Package fD0omega;

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