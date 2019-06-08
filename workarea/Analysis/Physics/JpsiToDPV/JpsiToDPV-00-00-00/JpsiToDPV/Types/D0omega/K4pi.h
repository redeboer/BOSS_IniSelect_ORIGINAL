#ifndef Physics_JpsiToDPV_Types_D0omega_K4pi_H
#define Physics_JpsiToDPV_Types_D0omega_K4pi_H

#include "JpsiToDPV/Globals.h"
#include "JpsiToDPV/TrackCollection.h"
#include "JpsiToDPV/TreeCollection.h"
#include "JpsiToDPV/Trees/TreeFit.h"
#include "JpsiToDPV/Trees/TreeMC.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include <string>

namespace D0omega
{
  namespace K4pi
  {
    struct Reconstructed
    {
      HepLorentzVector D0;
      HepLorentzVector omega;
      HepLorentzVector Jpsi;
    };

    struct LorentzVectors
    {
      HepLorentzVector pim;
      HepLorentzVector pip1;
      HepLorentzVector pip2;
      HepLorentzVector Km;
      HepLorentzVector g1;
      HepLorentzVector g2;
      HepLorentzVector pi0_4C;
      HepLorentzVector pi0_5C;
      Reconstructed    comb1;
      Reconstructed    comb2;
    };
    struct Fit4C : public TreeFit, public TreeMC
    {
      Fit4C(const char* name, const char* title = "") : TreeFit(name, title)
      {
        BRANCHMOM(pi0_4C);
        BRANCHMOM(D0);
        BRANCHMOM(omega);
        BRANCHMOM(Jpsi);
        BookMC(fTree);
      }
      MomObj pi0_4C;
      MomObj D0;
      MomObj omega;
      MomObj Jpsi;
    };

    struct Fit5C : public Fit4C
    {
      Fit5C(const char* name, const char* title = "") : Fit4C(name, title)
      {
        BRANCH(chi2_1C);
        BRANCH(fcos);
        BRANCH(Elow);
      }
      MomObj   pi0_5C;
      Double_t chi2_1C; ///< \f$chi^2\f$ of the extra 1-constraint.
      Double_t fcos;    ///< \f$E/|\vec{p}|\f$ ratio for \f$\pi^0\f$ candidate
      Double_t Elow;    ///< Lowest energy of the two photons
    };

    struct Package
    {
      Package() : tree("D0omega_K4pi") {}

      Bool_t DoFit(VertexParameter& vxpar, Double_t chi2max, TrackCollection& tracks)
      {
        RecMdcKalTrack* KmTrk   = tracks.Km[0]->mdcKalTrack();
        RecMdcKalTrack* pimTrk  = tracks.pim[0]->mdcKalTrack();
        RecMdcKalTrack* pip1Trk = tracks.pip[0]->mdcKalTrack();
        RecMdcKalTrack* pip2Trk = tracks.pip[1]->mdcKalTrack();

        WTrackParameter wvKmTrk(IniSelect::mK, KmTrk->getZHelix(), KmTrk->getZError());
        WTrackParameter wvpimTrk(IniSelect::mpi, pimTrk->getZHelix(), pimTrk->getZError());
        WTrackParameter wvpip1Trk(IniSelect::mpi, pip1Trk->getZHelix(), pip1Trk->getZError());
        WTrackParameter wvpip2Trk(IniSelect::mpi, pip2Trk->getZHelix(), pip2Trk->getZError());

        VertexFit* vtxfit = VertexFit::instance();
        vtxfit->init();
        vtxfit->AddTrack(0, wvKmTrk);
        vtxfit->AddTrack(1, wvpimTrk);
        vtxfit->AddTrack(2, wvpip1Trk);
        vtxfit->AddTrack(3, wvpip2Trk);
        vtxfit->AddVertex(0, vxpar, 0, 1);
        if(!vtxfit->Fit(0)) return false;
        vtxfit->Swim(0);

        WTrackParameter wKm   = vtxfit->wtrk(0);
        WTrackParameter wpim  = vtxfit->wtrk(1);
        WTrackParameter wpip1 = vtxfit->wtrk(2);
        WTrackParameter wpip2 = vtxfit->wtrk(3);

        tree.chi2                              = 9999.;
        KalmanKinematicFit*            kkmfit = KalmanKinematicFit::instance();
        vector<EvtRecTrack*>::iterator it1;
        for(it1 = tracks.photon.begin(); it1 != tracks.photon.end(); ++it1)
        {
          RecEmcShower*                  g1Trk = (*it1)->emcShower();
          vector<EvtRecTrack*>::iterator it2;
          for(it2 = tracks.photon.begin(); it2 != tracks.photon.end(); ++it2)
          {
            RecEmcShower* g2Trk = (*it2)->emcShower();
            kkmfit->init();
            kkmfit->AddTrack(0, wKm);
            kkmfit->AddTrack(1, wpim);
            kkmfit->AddTrack(2, wpip1);
            kkmfit->AddTrack(3, wpip2);
            kkmfit->AddTrack(4, 0.0, g1Trk);
            kkmfit->AddTrack(5, 0.0, g2Trk);
            kkmfit->AddFourMomentum(0, IniSelect::ecms);
            kkmfit->AddResonance(1, IniSelect::mpi0, 4, 5);
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

        if(tree.chi2 > chi2max) return false;

        results.comb1.D0    = results.pip1 + results.Km;
        results.comb1.omega = results.pip2 + results.pim + results.pi0_5C;
        results.comb2.D0    = results.pip2 + results.Km;
        results.comb2.omega = results.pip1 + results.pim + results.pi0_5C;

        tree.pi0_4C.m = results.pi0_4C.m();
        tree.pi0_4C.p = results.pi0_4C.rho();
        tree.pi0_5C.m = results.pi0_5C.m();
        tree.pi0_5C.p = results.pi0_5C.rho();

        double m1 = abs(results.comb1.omega.m() - IniSelect::momega);
        double m2 = abs(results.comb2.omega.m() - IniSelect::momega);
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

        return true;
      }

      void Fill() { tree.Fill(); }

      Fit5C          tree;
      LorentzVectors results;
    };
  } // namespace K4pi
} // namespace D0omega
/// @}
#endif