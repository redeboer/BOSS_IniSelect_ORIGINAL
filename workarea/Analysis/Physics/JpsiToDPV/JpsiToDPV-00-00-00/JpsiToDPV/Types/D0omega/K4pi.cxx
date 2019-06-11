#include "JpsiToDPV/Types/D0omega/K4pi.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Handlers/PID.h"
#include "IniSelect/Handlers/TrackCollectionNew.h"
#include "IniSelect/Handlers/KinematicFitter.h"
#include "IniSelect/Handlers/VertexFitter.h"

using namespace D0omega::K4pi;
using namespace IniSelect;

void Package::DoFit(Double_t chi2max)
{
  VertexFitter::Initialize();
  VertexFitter::AddTrack(TrackCollectionNew::K.neg[0], Mass::K);
  VertexFitter::AddTrack(TrackCollectionNew::pi.neg[0], Mass::pi);
  VertexFitter::AddTrack(TrackCollectionNew::pi.pos[0], Mass::pi);
  VertexFitter::AddTrack(TrackCollectionNew::pi.pos[1], Mass::pi);
  VertexFitter::AddCleanVertex();
  VertexFitter::FitAndSwim();

  auto wKm   = VertexFitter::GetTrack(0);
  auto wpim  = VertexFitter::GetTrack(1);
  auto wpip1 = VertexFitter::GetTrack(2);
  auto wpip2 = VertexFitter::GetTrack(3);

  tree.chi2   = 9999.;
  auto kkmfit = KalmanKinematicFit::instance();
  for(auto it1 = TrackCollectionNew::photon.begin(); it1 != TrackCollectionNew::photon.end(); ++it1)
  {
    for(auto it2 = TrackCollectionNew::photon.begin(); it2 != TrackCollectionNew::photon.end(); ++it2)
    {
      auto g1Trk = (*it1)->emcShower();
      auto g2Trk = (*it2)->emcShower();
      kkmfit->init();
      kkmfit->AddTrack(0, wKm);
      kkmfit->AddTrack(1, wpim);
      kkmfit->AddTrack(2, wpip1);
      kkmfit->AddTrack(3, wpip2);
      kkmfit->AddTrack(4, 0.0, g1Trk);
      kkmfit->AddTrack(5, 0.0, g2Trk);
      kkmfit->AddFourMomentum(0, Physics::Ecms);
      kkmfit->AddResonance(1, Mass::pi0, 4, 5);
      if(!kkmfit->Fit(0)) continue;
      results.g1     = kkmfit->pfit(4);
      results.g2     = kkmfit->pfit(5);
      results.pi0_4C = results.g1 + results.g2;
      if(!kkmfit->Fit(1)) continue;
      if(kkmfit->Fit())
      {
        double chi2 = kkmfit->chisq();
        if(chi2 < tree.chi2)
        {
          tree.chi2       = chi2;
          results.Km     = kkmfit->pfit(0);
          results.pim    = kkmfit->pfit(1);
          results.pip1   = kkmfit->pfit(2);
          results.pip2   = kkmfit->pfit(3);
          results.g1     = kkmfit->pfit(4);
          results.g2     = kkmfit->pfit(5);
          results.pi0_5C = results.g1 + results.g2;
        }
      }
    }
  }

  if(tree.chi2 > chi2max) Exception("Chi2 failure: %g > %g", tree.chi2, chi2max);

  results.comb1.D0    = results.pip1 + results.Km;
  results.comb1.omega = results.pip2 + results.pim + results.pi0_5C;
  results.comb2.D0    = results.pip2 + results.Km;
  results.comb2.omega = results.pip1 + results.pim + results.pi0_5C;

  tree.pi0_4C.m = results.pi0_4C.m();
  tree.pi0_4C.p = results.pi0_4C.rho();
  tree.pi0_5C.m = results.pi0_5C.m();
  tree.pi0_5C.p = results.pi0_5C.rho();

  double m1 = abs(results.comb1.omega.m() - Mass::omega);
  double m2 = abs(results.comb2.omega.m() - Mass::omega);
  if(m1 < m2)
  {
    tree.D0.m    = results.comb1.D0.m();
    tree.omega.m = results.comb1.omega.m();
    tree.D0.p    = results.comb1.D0.rho();
    tree.omega.p = results.comb1.omega.rho();
  }
  else
  {
    tree.D0.m    = results.comb2.D0.m();
    tree.omega.m = results.comb2.omega.m();
    tree.D0.p    = results.comb2.D0.rho();
    tree.omega.p = results.comb2.omega.rho();
  }

  // * Photon kinematics * //
  double eg1 = results.g1.e();
  double eg2 = results.g2.e();
  tree.fcos   = (eg1 - eg2) / results.pi0_5C.rho(); // E/p ratio for pi^0 candidate
  tree.Elow   = (eg1 < eg2) ? eg1 : eg2;            // lowest energy of the two gammas
}