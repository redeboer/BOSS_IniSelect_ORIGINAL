#ifndef Physics_JpsiToDPV_Types_D0omega_K4pi_H
#define Physics_JpsiToDPV_Types_D0omega_K4pi_H

#include "JpsiToDPV/TreeCollection.h"
#include "JpsiToDPV/Trees/TreeFit.h"
#include "JpsiToDPV/Trees/TreeMC.h"
#include <string>

#define DECLAREWRITE(TREE) declareProperty(TREE.PropertyName(), TREE.write)

namespace D0omega
{
  namespace K4pi
  {
    struct Fit4C : public TreeFit
    {
      Fit4C(const char* name, const char* title = "") : TreeFit(name, title)
      {
        BRANCHMOM(pi0);
        BRANCHMOM(D0);
        BRANCHMOM(omega);
      }
      MomObj pi0;   ///< Inv. mass and 3-momentum for \f$\pi^0 \rightarrow \gamma\gamma\f$.
      MomObj D0;    ///< Inv. mass and 3-momentum for \f$D^0 \rightarrow K^-\pi^+\f$.
      MomObj omega; ///< Inv. mass and 3-momentum for \f$\omega \rightarrow \pi^0\pi^-\pi^+\f$.
    };

    struct Fit5C : public Fit4C
    {
      Fit5C(const char* name, const char* title = "") : Fit4C(name, title)
      {
        BRANCH(chi2_1C);
        BRANCH(fcos);
        BRANCH(Elow);
      }
      Double_t chi2_1C; ///< \f$chi^2\f$ of the extra 1-constraint.
      Double_t fcos;    ///< \f$E/|\vec{p}|\f$ ratio for \f$\pi^0\f$ candidate
      Double_t Elow;    ///< Lowest energy of the two photons
    };

    struct TopoAna : public TreeMC
    {
      TopoAna(const char* name, const char* title = "") : TreeMC(name, title)
      {
        BRANCH(chi2_4C);
        BRANCH(chi2_5C);
        BRANCH(mD0_4C);
        BRANCH(mD0_5C);
        BRANCH(momega_4C);
        BRANCH(momega_5C);
      }
      Double_t chi2_4C;
      Double_t chi2_5C;
      Double_t mD0_4C;
      Double_t mD0_5C;
      Double_t momega_4C;
      Double_t momega_5C;
    };

    struct Package
    {
      Package() : fit4c("fit4c"), fit5c("fit5c"), MC("topology") {}

      Fit4C   fit4c; ///< 4-constraint (4C) fit information
      Fit5C   fit5c; ///< 5-constraint (5C) fit information
      TopoAna MC;

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
  } // namespace K4pi
} // namespace D0omega
/// @}
#endif