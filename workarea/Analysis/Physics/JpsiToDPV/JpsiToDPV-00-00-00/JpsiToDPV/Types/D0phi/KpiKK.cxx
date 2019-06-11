#include "JpsiToDPV/Types/D0phi/KpiKK.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Handlers/PID.h"
#include "IniSelect/Handlers/TrackCollectionNew.h"
#include "IniSelect/Handlers/KinematicFitter.h"
#include "IniSelect/Handlers/VertexFitter.h"

using namespace D0phi::KpiKK;
using namespace IniSelect;

void Package::DoFit(Double_t chi2max)
{
  VertexFitter::Initialize();
  VertexFitter::AddTrack(TrackCollectionNew::K.neg[0], Mass::K);
  VertexFitter::AddTrack(TrackCollectionNew::K.neg[1], Mass::K);
  VertexFitter::AddTrack(TrackCollectionNew::K.pos[0], Mass::K);
  VertexFitter::AddTrack(TrackCollectionNew::pi.pos[0], Mass::pi);
  VertexFitter::AddCleanVertex();
  VertexFitter::FitAndSwim();

  auto wKm1 = VertexFitter::GetTrack(0);
  auto wKm2 = VertexFitter::GetTrack(1);
  auto wKp  = VertexFitter::GetTrack(2);
  auto wpip = VertexFitter::GetTrack(3);

  auto kkmfit = KalmanKinematicFit::instance();
  kkmfit->init();
  kkmfit->AddTrack(0, wKm1);
  kkmfit->AddTrack(1, wKm2);
  kkmfit->AddTrack(2, wKp);
  kkmfit->AddTrack(3, wpip);
  kkmfit->AddFourMomentum(0, Physics::Ecms);

  if(!kkmfit->Fit(0)) throw Exception("4C fit failed");
  if(!kkmfit->Fit()) throw Exception("5C fit failed");
  tree.chi2 = kkmfit->chisq();
  if(tree.chi2 > chi2max) throw Exception("Chi2 failure: %g > %g", tree.chi2, chi2max);

  results.Km1 = kkmfit->pfit(0);
  results.Km2 = kkmfit->pfit(1);
  results.Kp  = kkmfit->pfit(2);
  results.pip = kkmfit->pfit(3);

  results.comb1.D0   = results.Km1 + results.pip;
  results.comb1.phi  = results.Km2 + results.Kp;
  results.comb1.Jpsi = results.comb1.D0 + results.comb1.phi;
  results.comb2.D0   = results.Km2 + results.pip;
  results.comb2.phi  = results.Km1 + results.Kp;
  results.comb2.Jpsi = results.comb1.D0 + results.comb1.phi;

  double m1 = abs(results.comb1.phi.m() - Mass::phi);
  double m2 = abs(results.comb2.phi.m() - Mass::phi);
  if(m1 < m2)
  {
    tree.D0.m   = results.comb1.D0.m();
    tree.phi.m  = results.comb1.phi.m();
    tree.Jpsi.m = results.comb1.Jpsi.m();
    tree.D0.p   = results.comb1.D0.rho();
    tree.phi.p  = results.comb1.phi.rho();
    tree.Jpsi.p = results.comb1.Jpsi.rho();
  }
  else
  {
    tree.D0.m   = results.comb2.D0.m();
    tree.phi.m  = results.comb2.phi.m();
    tree.Jpsi.m = results.comb2.Jpsi.m();
    tree.D0.p   = results.comb2.D0.rho();
    tree.phi.p  = results.comb2.phi.rho();
    tree.Jpsi.p = results.comb2.Jpsi.rho();
  }
}