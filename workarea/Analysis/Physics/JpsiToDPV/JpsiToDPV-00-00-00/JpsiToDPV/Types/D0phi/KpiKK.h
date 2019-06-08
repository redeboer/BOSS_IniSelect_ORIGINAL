#ifndef Physics_JpsiToDPV_Types_D0phi_KpiKK_H
#define Physics_JpsiToDPV_Types_D0phi_KpiKK_H

#include "JpsiToDPV/Globals.h"
#include "JpsiToDPV/TrackCollection.h"
#include "JpsiToDPV/TreeCollection.h"
#include "JpsiToDPV/Trees/TreeFit.h"
#include "JpsiToDPV/Trees/TreeMC.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include <string>

namespace D0phi
{
  namespace KpiKK
  {
    struct Reconstructed
    {
      HepLorentzVector D0;
      HepLorentzVector phi;
      HepLorentzVector Jpsi;
    };

    struct LorentzVectors
    {
      HepLorentzVector Km1;
      HepLorentzVector Km2;
      HepLorentzVector Kp;
      HepLorentzVector pip;
      Reconstructed    comb1;
      Reconstructed    comb2;
    };
    struct Fit4C : public TreeFit, public TreeMC
    {
      Fit4C(const char* name, const char* title = "") : TreeFit(name, title)
      {
        BRANCHMOM(D0);
        BRANCHMOM(phi);
        BRANCHMOM(Jpsi);
        BookMC(fTree);
      }
      MomObj D0;
      MomObj phi;
      MomObj Jpsi;
    };

    struct Package
    {
      Package() : tree("D0phi_KpiKK") {}

      Bool_t DoFit(VertexParameter& vxpar, Double_t chi2max, TrackCollection& tracks)
      {
        RecMdcKalTrack* Km1Trk = tracks.Km[0]->mdcKalTrack();
        RecMdcKalTrack* Km2Trk = tracks.Km[1]->mdcKalTrack();
        RecMdcKalTrack* KpTrk  = tracks.Kp[0]->mdcKalTrack();
        RecMdcKalTrack* pipTrk = tracks.pip[0]->mdcKalTrack();

        WTrackParameter wvKm1Trk(IniSelect::mK, Km1Trk->getZHelix(), Km1Trk->getZError());
        WTrackParameter wvKm2Trk(IniSelect::mK, Km2Trk->getZHelix(), Km2Trk->getZError());
        WTrackParameter wvKpTrk(IniSelect::mK, KpTrk->getZHelix(), KpTrk->getZError());
        WTrackParameter wvpipTrk(IniSelect::mpi, pipTrk->getZHelix(), pipTrk->getZError());

        VertexFit* vtxfit = VertexFit::instance();
        vtxfit->init();
        vtxfit->AddTrack(0, wvKm1Trk);
        vtxfit->AddTrack(1, wvKm2Trk);
        vtxfit->AddTrack(2, wvKpTrk);
        vtxfit->AddTrack(3, wvpipTrk);
        vtxfit->AddVertex(0, vxpar, 0, 1);
        if(!vtxfit->Fit(0)) return false;
        vtxfit->Swim(0);

        WTrackParameter wKm1 = vtxfit->wtrk(0);
        WTrackParameter wKm2 = vtxfit->wtrk(1);
        WTrackParameter wKp  = vtxfit->wtrk(2);
        WTrackParameter wpip = vtxfit->wtrk(3);

        KalmanKinematicFit* kkmfit = KalmanKinematicFit::instance();
        kkmfit->init();
        kkmfit->AddTrack(0, wKm1);
        kkmfit->AddTrack(1, wKm2);
        kkmfit->AddTrack(2, wKp);
        kkmfit->AddTrack(3, wpip);
        kkmfit->AddFourMomentum(0, IniSelect::ecms);

        if(!kkmfit->Fit(0)) return false;
        if(!kkmfit->Fit()) return false;
        tree.chi2 = kkmfit->chisq();
        if(tree.chi2 > chi2max) return false;

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

        double m1 = abs(results.comb1.phi.m() - IniSelect::mphi);
        double m2 = abs(results.comb2.phi.m() - IniSelect::mphi);
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

        return true;
      }

      void Fill() { tree.Fill(); }

      Fit4C          tree;
      LorentzVectors results;
    };
  } // namespace KpiKK
} // namespace D0phi
/// @}
#endif