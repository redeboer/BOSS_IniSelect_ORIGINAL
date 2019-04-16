#ifndef Analysis_TSGlobals_H
#define Analysis_TSGlobals_H

/// @defgroup BOSS_packages Event selection
/// Event selection **packages** for the BOSS framework. The core of these packages is the `TrackSelector` algorithm, which is the base class for all subalgorithms, such as `D0phi_KpiKK`.

/// @defgroup BOSS_objects Helper objects
/// Helper **classes** for the event selection packages. An example would be the `CutObject`, which is can be used in all packages for applying cuts and storing their paramters in the final ROOT output file.

/// @defgroup BOSS_globals Globals
/// Global **parameters** encapsulated in namespaces.

#include "CLHEP/Vector/LorentzVector.h"

/// @addtogroup BOSS_globals
/// @{

/// Namespace that contains some parameters (such as particle masses) that are useful for derived classes of the `TrackSelector` base algorithm.
namespace TSGlobals
{
  static const double gEcms = 3.097;
  ///< Center-of-mass energy.
  static const CLHEP::HepLorentzVector gEcmsVec(0.034, 0, 0, gEcms);
  namespace Mass
  {
    static const double rho = 0.77526;
    ///< Mass of \f$\rho^{0\pm}\f$, see
    ///< [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-rho-770.pdf).
    static const double pi0 = 0.1349770;
    ///< Mass of \f$\pi^0\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-pi-zero.pdf).
    static const double pi = 0.13957061;
    ///< Mass of \f$\pi^\pm\f$, see
    ///< [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-pi-plus-minus.pdf).
    static const double K = 0.493677;
    ///< Mass of \f$K^\pm\f$.
    static const double D0 = 1.86483;
    ///< Mass of \f$D^0\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-D-zero.pdf).
    static const double phi = 1.019461;
    ///< Mass of \f$\phi\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-phi-1020.pdf).
    static const double omega = 0.78265;
    ///< Mass of \f$\omega\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-omega-782.pdf).
    static const double Jpsi = 3.0969;
    ///< Mass of \f$J/\psi\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-J-psi-1S.pdf).
  } // namespace Masses
};  // namespace TSGlobals

/// @}
#endif