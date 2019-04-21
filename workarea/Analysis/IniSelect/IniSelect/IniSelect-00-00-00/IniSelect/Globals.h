#ifndef Analysis_IniSelect_IniSelect_H
#define Analysis_IniSelect_IniSelect_H

/// @defgroup BOSS_packages Event selection
/// Event selection **packages** for the BOSS framework. The core of these packages is the `IniSelectObjects` algorithm, which is the base class for all subalgorithms, such as `D0phi_KpiKK`.

/// @defgroup BOSS_objects Helper objects
/// Helper **classes** for the event selection packages. An example would be the `CutObject`, which is can be used in all packages for applying cuts and storing their paramters in the final ROOT output file.

/// @defgroup BOSS_globals Globals
/// Global **parameters** encapsulated in namespaces.

#include "CLHEP/Vector/LorentzVector.h"
#include <vector>

/// @addtogroup BOSS_globals
/// @{

/// Namespace that contains some parameters (such as particle masses) that are useful for derived classes of the `IniSelectObjects` base algorithm.
namespace IniSelect
{
  static const double gEcms = 3.097;
  ///< Center-of-mass energy.
  static const CLHEP::HepLorentzVector gEcmsVec(0.034, 0, 0, gEcms);
  ///< Center-of-mass 4-momentum.
  static const double gSpeedOfLight = 299.792458; // tof path unit in mm
  namespace Math
  {
    static const double pi       = 3.14159265358979323846;
    static const double twopi    = 2 * pi;
    static const double RadToDeg = 180. / pi;
  } // namespace Math
  namespace Mass
  {
    static const double D0    = 1.86483;
    static const double Jpsi  = 3.0969;
    static const double K     = 0.493677;
    static const double e     = 0.000511;
    static const double mu    = 0.105658;
    static const double omega = 0.78265;
    static const double p     = 0.938272;
    static const double phi   = 1.019461;
    static const double pi    = 0.13957061;
    static const double pi0   = 0.1349770;
    static const double rho   = 0.77526;

    const double                     arr[] = {e, mu, pi, K, p};
    static const std::vector<double> TOF(arr, arr + sizeof(arr) / sizeof(*arr));
  } // namespace Mass
  namespace McTruth
  {
    static bool IsInitialCluster(Event::McParticle* mcparticle)
    {
      return (mcparticle->particleProperty() == 91 || mcparticle->particleProperty() == 92);
    } ///< Test whether an MC truth particle is `91` (the PDG code of a 'cluster') or `92` (the PDG code of a string). This function is used to characterised the initial cluster, e.g. \f$J/\psi\f$
    static bool IsJPsi(Event::McParticle* mcparticle)
    {
      return (mcparticle->particleProperty() == 443);
    } ///< Test whether an MC truth particle is \f$J/\psi\f$ (PDG code `91`).
    static bool IsFromJPsi(Event::McParticle* mcparticle)
    {
      return (mcparticle->mother().particleProperty() == 443);
    } ///< Test whether an MC truth particle is \f$J/\psi\f$ (PDG code `91`).
  }   // namespace McTruth
  namespace Paths
  {
    static const char* pdtTable =
      "/afs/ihep.ac.cn/bes3/offline/Boss/7.0.4/InstallArea/share/pdt.table";
  } // namespace Paths
};  // namespace IniSelect

/// @}
#endif