#ifndef Physics_JpsiToDPV_Types_D0omega_K4pi_H
#define Physics_JpsiToDPV_Types_D0omega_K4pi_H

#include "JpsiToDPV/TrackCollection.h"
#include "JpsiToDPV/TreeCollection.h"
#include "JpsiToDPV/Trees/TreeFit.h"
#include "JpsiToDPV/Trees/TreeMC.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include <string>

#define DECLAREWRITE(TREE) declareProperty(TREE.PropertyName(), TREE.write)

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
    struct Fit4C : public TreeFit
    {
      Fit4C(const char* name, const char* title = "") : TreeFit(name, title)
      {
        BRANCHMOM(pi0_4C);
        BRANCHMOM(D0);
        BRANCHMOM(omega);
      }
      MomObj pi0_4C;
      MomObj D0;
      MomObj omega;
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

    struct TopoAna : public TreeMC
    {
      TopoAna(const char* name, const char* title = "") : TreeMC(name, title)
      {
        BRANCH(chi2);
        BRANCH(mpi0_4C);
        BRANCH(mpi0_5C);
        BRANCH(mD0);
        BRANCH(momega);
      }
      Double_t chi2;
      Double_t mpi0_4C;
      Double_t mpi0_5C;
      Double_t mD0;
      Double_t momega;

      void Reset()
      {
        if(!write) return;
        chi2    = 99999.;
        mpi0_4C = 99999.;
        mpi0_5C = 99999.;
        mD0     = 99999.;
        momega  = 99999.;
      }
    };

    struct Package
    {
      Package() : fit("fit"), MC("topology") {}

      Bool_t DoFit(Double_t chi2max, TrackCollection& tracks)
      {
        fit.chi2 = 9999.;

        KalmanKinematicFit* kkmfit = KalmanKinematicFit::instance();
        VertexFit*          vtxfit = VertexFit::instance();

        WTrackParameter wKm   = vtxfit->wtrk(0);
        WTrackParameter wpim  = vtxfit->wtrk(1);
        WTrackParameter wpip1 = vtxfit->wtrk(2);
        WTrackParameter wpip2 = vtxfit->wtrk(3);

        vector<EvtRecTrack*>::iterator it1 = tracks.photon.begin();
        for(; it1 != tracks.photon.end(); ++it1)
        {
          RecEmcShower*                  g1Trk = (*it1)->emcShower();
          vector<EvtRecTrack*>::iterator it2   = tracks.photon.begin();
          for(; it2 != tracks.photon.end(); ++it2)
          {
            RecEmcShower* g2Trk = (*it2)->emcShower();
            kkmfit->init();
            kkmfit->AddTrack(0, wKm);
            kkmfit->AddTrack(1, wpim);
            kkmfit->AddTrack(2, wpip1);
            kkmfit->AddTrack(3, wpip2);
            kkmfit->AddTrack(4, 0.0, g1Trk);
            kkmfit->AddTrack(5, 0.0, g2Trk);
            kkmfit->AddFourMomentum(0, ecms);
            kkmfit->AddResonance(1, mpi0, 4, 5);
            if(!kkmfit->Fit(0)) continue;
            results.g1     = kkmfit->pfit(4);
            results.g2     = kkmfit->pfit(5);
            results.pi0_4C = results.g1 + results.g2;
            if(!kkmfit->Fit(1)) continue;
            if(kkmfit->Fit())
            {
              double chi2 = kkmfit->chisq();
              if(chi2 < fit.chi2)
              {
                fit.chi2       = chi2;
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

        if(fit.chi2 > chi2max) return false;

        results.comb1.D0    = results.pip1 + results.Km;
        results.comb1.omega = results.pip2 + results.pim + results.pi0_5C;
        results.comb2.D0    = results.pip2 + results.Km;
        results.comb2.omega = results.pip1 + results.pim + results.pi0_5C;

        fit.pi0_4C.m = results.pi0_4C.m();
        fit.pi0_4C.p = results.pi0_4C.rho();
        fit.pi0_5C.m = results.pi0_5C.m();
        fit.pi0_5C.p = results.pi0_5C.rho();
        if(MC.write)
        {
          MC.mpi0_4C = fit.pi0_4C.m;
          MC.mpi0_5C = fit.pi0_5C.m;
        }

        double m1 = abs(results.comb1.omega.m() - momega);
        double m2 = abs(results.comb2.omega.m() - momega);
        if(m1 < m2)
        {
          fit.D0.m    = results.comb1.D0.m();
          fit.omega.m = results.comb1.omega.m();
          fit.D0.p    = results.comb1.D0.rho();
          fit.omega.p = results.comb1.omega.rho();
          if(MC.write)
          {
            MC.mD0    = results.comb1.D0.m();
            MC.momega = results.comb1.omega.m();
          }
        }
        else
        {
          fit.D0.m    = results.comb2.D0.m();
          fit.omega.m = results.comb2.omega.m();
          fit.D0.p    = results.comb2.D0.rho();
          fit.omega.p = results.comb2.omega.rho();
          if(MC.write)
          {
            MC.mD0    = results.comb2.D0.m();
            MC.momega = results.comb2.omega.m();
          }
        }

        // * Photon kinematics * //
        double eg1 = results.g1.e();
        double eg2 = results.g2.e();
        fit.fcos   = (eg1 - eg2) / results.pi0_5C.rho(); // E/p ratio for pi^0 candidate
        fit.Elow   = (eg1 < eg2) ? eg1 : eg2;            // lowest energy of the two gammas

        // * WRITE pi^0 information from EMCal ("fit" branch) *
        fit.Fill(); // "fit" branch
        return true;
      }

      Fit5C          fit;
      TopoAna        MC;
      LorentzVectors results;
    };
  } // namespace K4pi
} // namespace D0omega
/// @}
#endif