#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"

#include "McTruth/McParticle.h"
//#include "EventTag/IEventTagSvc.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "VertexFit/Helix.h"
#include "VertexFit/IVertexDbSvc.h"

#include "DstEvent/TofHitStatus.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "RooFit.h"
#include "TMath.h"
#include "TROOT.h"
#include "TRandom.h"
using CLHEP::Hep2Vector;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;

#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "FSFilter/YUPING.h"
#include "ParticleID/ParticleID.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include <vector>
//const double twopi = 6.2831853;
//const double pi = 3.1415927;
const double mpi      = 0.13957;
const double mk       = 0.49368;
const double mpi0     = 0.1349766;
const double xmass[5] = {0.000511, 0.105658, 0.139570, 0.493677, 0.938272};
//const double velc = 29.9792458;  tof_path unit in cm.
const double                          velc = 299.792458; // tof path unit in mm
typedef std::vector<int>              Vint;
typedef std::vector<double>           Vdouble;
typedef std::vector<HepLorentzVector> Vp4;

//declare one counter
static int counter[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//*************************************************************************
// *****************************************************************
// ** A macro to create correlated Gaussian-distributed variables **
// *****************************************************************

// corset(): sets up the generation by calculating C from V.
void YUPING::corset(HepSymMatrix& V, HepMatrix& C, int n)
{
  //cout<<"v="<<V<<endl;
  //cout<<"c="<<C<<endl;
  double sum;

  // Compute square root of matrix sigma
  for(int j = 0; j < n; j++)
  {
    sum = 0;
    for(int k = 0; k < j; k++) { sum = sum + C[j][k] * C[j][k]; }
    //cout<<"sum="<<sum<<endl;
    //cout<<"v("<<j<<","<<j<<")="<<V[j][j]<<endl;
    C[j][j] = sqrt(abs(V[j][j] - sum));
    //cout<<"c("<<j<<","<<j<<")="<<C[j][j]<<endl;
    // Off Diagonal terms
    for(int i = j + 1; i < n; i++)
    {
      sum = 0;
      for(int k = 0; k < j; k++) { sum = sum + C[i][k] * C[j][k]; }
      C[i][j] = (V[i][j] - sum) / C[j][j];
      //cout<<"C("<<i<<","<<j<<")="<<C[i][j]<<endl;
    }
  }
}

// corgen(): generates a set of n random numbers Gaussian-distributed with covariance
// matrix V (V = C*C') and mean values zero.
void YUPING::corgen(HepMatrix& C, HepVector& x, int n)
{
  int i, j;
  int nmax = 100;

  if(n > nmax) { printf("Error in corgen: array overflown"); }

  double tmp[3];
  for(int p = 0; p < n; p++)
  {
    tmp[p] = gRandom->Gaus(0, 1);
    //cout<<"tmp["<<p<<"]="<<tmp[p]<<endl;
  }
  for(i = 0; i < n; i++)
  {
    x[i] = 0.0;
    for(j = 0; j <= i; j++) { x[i] = x[i] + C[i][j] * tmp[j]; }
  }
}

void YUPING::calibration(RecMdcKalTrack* trk, HepVector& wtrk_zHel, int n)
{

  HepVector track_calerr_d2(5, 0);

  // pi+,pi-,K+,K- from Chunhua's omega chi_c0 memo

  // pi+
  if(trk->charge() > 0 && n == 0)
  {
    track_calerr_d2[0] = 1.0;
    track_calerr_d2[1] = 1.16;
    track_calerr_d2[2] = 1.13;
    track_calerr_d2[3] = 1.0;
    track_calerr_d2[4] = 1.10;
  }
  // pi-
  if(trk->charge() < 0 && n == 0)
  {
    track_calerr_d2[0] = 1.0;
    track_calerr_d2[1] = 1.16;
    track_calerr_d2[2] = 1.13;
    track_calerr_d2[3] = 1.0;
    track_calerr_d2[4] = 1.09;
  }
  // K+
  if(trk->charge() > 0 && n == 1)
  {
    track_calerr_d2[0] = 1.0;
    track_calerr_d2[1] = 1.17;
    track_calerr_d2[2] = 1.12;
    track_calerr_d2[3] = 1.0;
    track_calerr_d2[4] = 1.08;
  }
  // K-
  if(trk->charge() < 0 && n == 1)
  {
    track_calerr_d2[0] = 1.0;
    track_calerr_d2[1] = 1.16;
    track_calerr_d2[2] = 1.11;
    track_calerr_d2[3] = 1.0;
    track_calerr_d2[4] = 1.09;
  }

  // e+,e-,mu+,mu- from Zhihong's eta J/psi memo

  // e+
  if(trk->charge() > 0 && n == 2)
  {
    track_calerr_d2[0] = 1.0;
    track_calerr_d2[1] = 1.10;
    track_calerr_d2[2] = 1.01;
    track_calerr_d2[3] = 1.0;
    track_calerr_d2[4] = 0.98;
  }
  // e-
  if(trk->charge() < 0 && n == 2)
  {
    track_calerr_d2[0] = 1.0;
    track_calerr_d2[1] = 1.10;
    track_calerr_d2[2] = 1.04;
    track_calerr_d2[3] = 1.0;
    track_calerr_d2[4] = 1.02;
  }
  // mu+
  if(trk->charge() > 0 && n == 3)
  {
    track_calerr_d2[0] = 1.0;
    track_calerr_d2[1] = 1.19;
    track_calerr_d2[2] = 1.09;
    track_calerr_d2[3] = 1.0;
    track_calerr_d2[4] = 1.09;
  }
  // mu-
  if(trk->charge() < 0 && n == 3)
  {
    track_calerr_d2[0] = 1.0;
    track_calerr_d2[1] = 1.18;
    track_calerr_d2[2] = 1.11;
    track_calerr_d2[3] = 1.0;
    track_calerr_d2[4] = 1.08;
  }

  HepVector track_calmean_d2(5, 0);

  track_calmean_d2[0] = 0;
  track_calmean_d2[1] = 0;
  track_calmean_d2[2] = 0;
  track_calmean_d2[3] = 0;
  track_calmean_d2[4] = 0;

  HepSymMatrix track_zerr(5, 0);
  if(n == 0) track_zerr = trk->getZError();
  if(n == 1) track_zerr = trk->getZErrorK();
  if(n == 2) track_zerr = trk->getZErrorE();
  if(n == 3) track_zerr = trk->getZErrorMu();
  HepSymMatrix track_zcal(3, 0);

  track_zcal[0][0] = (track_calerr_d2[1] * track_calerr_d2[1] - 1) * track_zerr[1][1];
  track_zcal[1][1] = (track_calerr_d2[2] * track_calerr_d2[2] - 1) * track_zerr[2][2];
  track_zcal[2][2] = (track_calerr_d2[4] * track_calerr_d2[4] - 1) * track_zerr[4][4];

  HepMatrix track_zerrc(3, 3, 0);
  YUPING::corset(track_zcal, track_zerrc, 3);
  HepVector track_zgen(3, 0);
  YUPING::corgen(track_zerrc, track_zgen, 3);

  if(n == 0)
  {
    wtrk_zHel[0] = trk->getZHelix()[0];
    wtrk_zHel[1] =
      trk->getZHelix()[1] + track_calmean_d2[1] * sqrt(track_zerr[1][1]) + track_zgen[0];
    wtrk_zHel[2] =
      trk->getZHelix()[2] + track_calmean_d2[2] * sqrt(track_zerr[2][2]) + track_zgen[1];
    wtrk_zHel[3] = trk->getZHelix()[3];
    wtrk_zHel[4] =
      trk->getZHelix()[4] + track_calmean_d2[4] * sqrt(track_zerr[4][4]) + track_zgen[2];
  }
  if(n == 1)
  {
    wtrk_zHel[0] = trk->getZHelixK()[0];
    wtrk_zHel[1] =
      trk->getZHelixK()[1] + track_calmean_d2[1] * sqrt(track_zerr[1][1]) + track_zgen[0];
    wtrk_zHel[2] =
      trk->getZHelixK()[2] + track_calmean_d2[2] * sqrt(track_zerr[2][2]) + track_zgen[1];
    wtrk_zHel[3] = trk->getZHelixK()[3];
    wtrk_zHel[4] =
      trk->getZHelixK()[4] + track_calmean_d2[4] * sqrt(track_zerr[4][4]) + track_zgen[2];
  }
  if(n == 2)
  {
    wtrk_zHel[0] = trk->getZHelixE()[0];
    wtrk_zHel[1] =
      trk->getZHelixE()[1] + track_calmean_d2[1] * sqrt(track_zerr[1][1]) + track_zgen[0];
    wtrk_zHel[2] =
      trk->getZHelixE()[2] + track_calmean_d2[2] * sqrt(track_zerr[2][2]) + track_zgen[1];
    wtrk_zHel[3] = trk->getZHelixE()[3];
    wtrk_zHel[4] =
      trk->getZHelixE()[4] + track_calmean_d2[4] * sqrt(track_zerr[4][4]) + track_zgen[2];
  }
  if(n == 3)
  {
    wtrk_zHel[0] = trk->getZHelixMu()[0];
    wtrk_zHel[1] =
      trk->getZHelixMu()[1] + track_calmean_d2[1] * sqrt(track_zerr[1][1]) + track_zgen[0];
    wtrk_zHel[2] =
      trk->getZHelixMu()[2] + track_calmean_d2[2] * sqrt(track_zerr[2][2]) + track_zgen[1];
    wtrk_zHel[3] = trk->getZHelixMu()[3];
    wtrk_zHel[4] =
      trk->getZHelixMu()[4] + track_calmean_d2[4] * sqrt(track_zerr[4][4]) + track_zgen[2];
  }
}
