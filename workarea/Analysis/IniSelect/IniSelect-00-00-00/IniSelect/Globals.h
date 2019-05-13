#ifndef Analysis_IniSelect_IniSelect_H
#define Analysis_IniSelect_IniSelect_H

/// @defgroup BOSS_packages Event selection
/// Event selection **packages** for the BOSS framework. The core of these packages is the `IniSelectObjects` algorithm, which is the base class for all subalgorithms, such as `D0phi_KpiKK`.

/// @defgroup BOSS_objects Helper objects
/// Helper **classes** for the event selection packages. An example would be the `CutObject`, which is can be used in all packages for applying cuts and storing their paramters in the final ROOT output file.

/// @defgroup BOSS_functions Functions
/// Global **functions** encapsulated in namespaces.

/// @defgroup BOSS_globals Globals
/// Global **parameters** encapsulated in namespaces.

#include "CLHEP/Vector/LorentzVector.h"
#include "McTruth/McParticle.h"
#include "TString.h"
#include <vector>

/// @addtogroup BOSS_globals
/// @{

/// Namespace that contains some parameters (such as particle masses) that are useful for derived classes of the `IniSelectObjects` base algorithm.
namespace IniSelect
{
  namespace Math
  {
    inline const double pi       = 3.14159265358979323846;
    inline const double twopi    = 2 * pi;
    inline const double RadToDeg = 180. / pi;
  } // namespace Math
  namespace Mass
  {
    inline const double D0    = 1.86483;
    inline const double Jpsi  = 3.0969;
    inline const double K     = 0.493677;
    inline const double e     = 0.000511;
    inline const double mu    = 0.105658;
    inline const double omega = 0.78265;
    inline const double p     = 0.938272;
    inline const double phi   = 1.019461;
    inline const double pi    = 0.13957061;
    inline const double pi0   = 0.1349770;
    inline const double rho   = 0.77526;

    inline const double                     arr[] = {e, mu, pi, K, p};
    inline const std::vector<double> TOF(arr, arr + sizeof(arr) / sizeof(*arr));
  } // namespace Mass
  namespace McTruth
  {
    bool IsInitialCluster(Event::McParticle* mcparticle);
    bool IsJPsi(Event::McParticle* mcparticle);
    bool IsFromJPsi(Event::McParticle* mcparticle);
  } // namespace McTruth
  namespace Paths
  {
    inline const char* pdtTable =
      "/afs/ihep.ac.cn/bes3/offline/Boss/7.0.4/InstallArea/share/pdt.table";
  } // namespace Paths
  namespace Physics
  {
    inline const double Ecms = 3.097;
    ///< Center-of-mass energy.
    inline const CLHEP::HepLorentzVector EcmsVec(0.034, 0, 0, Ecms);
    ///< Center-of-mass 4-momentum.
    inline const double SpeedOfLight = 299.792458;
    inline const char* arr[] = {"pi-", "pi+", "K-", "K+", "e-", "e+", "mu-", "mu+", "p+", "anti-p-", "g"};
    inline const std::vector<TString> BESIIIparticles(arr, arr + sizeof(arr) / sizeof(*arr));
  } // namespace Physics
  namespace TerminalIO
  {
    enum EIniSelColor
    {
      Red    = 31,
      Green  = 32,
      Yellow = 33
    };
    void ColourPrintBold(const char code, const TString& message);
    void ColourPrint(const char code, const TString& message, const TString& qualifier = "");
    void PrintFatalError(const TString& message);
    void PrintSuccess(const TString& message);
    void PrintWarning(const TString& message);
    void PrintRed(const TString& message);
    void PrintGreen(const TString& message);
    void PrintYellow(const TString& message);
    void PrintBold(const TString& message);
  } // namespace TerminalIO
};  // namespace IniSelect

/// @}
#endif