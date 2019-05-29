#ifndef IniSelect_FitResults_D0omega_K4pi_H
#define IniSelect_FitResults_D0omega_K4pi_H

#include "D0omega_K4pi/Trees/TreeFit.h"
#include "D0omega_K4pi/Trees/TreeMC.h"

namespace D0omega_K4pi_space
{
  /// @addtogroup BOSS_objects
  /// @{
  /// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
  /// @date     May 29th, 2018
  class Fit4C : public TreeFit
  {
  public:
    Fit4C(const char* name, const char* title = "") : TreeFit(name, title){}
    {
      BRANCHMOM(pi0);
      BRANCHMOM(D0);
      BRANCHMOM(omega);
    }
    MomObj pi0;   ///< Inv. mass and 3-momentum for \f$\pi^0 \rightarrow \gamma\gamma\f$.
    MomObj D0;    ///< Inv. mass and 3-momentum for \f$D^0 \rightarrow K^-\pi^+\f$.
    MomObj omega; ///< Inv. mass and 3-momentum for \f$\omega \rightarrow \pi^0\pi^-\pi^+\f$.
  };
  /// @}

  /// @addtogroup BOSS_objects
  /// @{
  /// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
  /// @date     May 29th, 2018
  class Fit5C : public Fit4C
  {
  public:
    Fit5C(const char* name, const char* title = "") :
      Fit4C(name, title){}
    {
      BRANCH(chi2_1C);
      BRANCH(fcos);
      BRANCH(Elow);
    }
    Double_t chi2_1C; ///< \f$chi^2\f$ of the extra 1-constraint.
    Double_t fcos;    ///< \f$E/|\vec{p}|\f$ ratio for \f$\pi^0\f$ candidate
    Double_t Elow;    ///< Lowest energy of the two photons
  };
  /// @}

  /// @addtogroup BOSS_objects
  /// @{
  /// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
  /// @date     May 29th, 2018
  class MCresults : public TreeMC
  {
  public:
    MCresults(const char* name, const char* title = "") : TreeMC(name, title)
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
  /// @}
} // namespace D0omega_K4pi
#endif