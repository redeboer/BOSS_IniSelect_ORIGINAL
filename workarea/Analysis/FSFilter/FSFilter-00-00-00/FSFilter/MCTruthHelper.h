#ifndef MCTRUTHHELPER_H
#define MCTRUTHHELPER_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "McTruth/McParticle.h"
#include <string>
#include <vector>

using namespace std;
using namespace Event;

class MCTruthParticle;

// ***************************************************************
//
//  CLASS McTruthHelper
//
//    Parse truth information using the McParticleCol object
//      provided by BOSS
//
// ***************************************************************

class MCTruthHelper
{

public:
  // constructor

  MCTruthHelper(SmartDataPtr<Event::McParticleCol> mcParticleCol);

  // destructor

  ~MCTruthHelper();

  // return a list of final state particles sorted like Lambda, ALambda, e+,...,pi-,pi0

  vector<MCTruthParticle*> MCParticleList() { return m_mcFinal; }

  // get the total MC energy (to check that all particles are accounted for)

  double MCTotalEnergyInitial();
  double MCTotalEnergyFinal();

  // flags for the decay code

  int MCDecayCode1()
  {
    if(m_mcDecayCode1 != -1) {}
    else
    {
      m_mcDecayCode1 = nFSParticles(kGamma) * 1000000 + nFSParticles(kKp) * 100000 +
                       nFSParticles(kKm) * 10000 + nFSParticles(kKs) * 1000 +
                       nFSParticles(kPip) * 100 + nFSParticles(kPim) * 10 + nFSParticles(kPi0) * 1;
    }
    return m_mcDecayCode1;
  }

  int MCDecayCode2()
  {
    if(m_mcDecayCode2 != -1) {}
    else
    {
      m_mcDecayCode2 = nFSParticles(kLambda) * 100000000 + nFSParticles(kALambda) * 10000000 +
                       nFSParticles(kEp) * 1000000 + nFSParticles(kEm) * 100000 +
                       nFSParticles(kMup) * 10000 + nFSParticles(kMum) * 1000 +
                       nFSParticles(kPp) * 100 + nFSParticles(kPm) * 10 + nFSParticles(kEta) * 1;
    }
    return m_mcDecayCode2;
  }

  // flag for extra final state particles

  int MCExtras()
  {
    if(m_mcExtras != -1) {}
    else
    {
      m_mcExtras = (nFSParticles(kNuE) + nFSParticles(kNuMu) + nFSParticles(kNuTau) +
                    nFSParticles(kAntiNuE) + nFSParticles(kAntiNuMu) + nFSParticles(kAntiNuTau)) *
                     1000 +
                   nFSParticles(kKl) * 100 + nFSParticles(kN) * 10 + nFSParticles(kAntiN) * 1;
    }
    return m_mcExtras;
  }

  // open charm flags

  int MCOpenCharm1()
  {
    if(m_mcOpenCharm1 != -1) {}
    else
    {
      m_mcOpenCharm1 =
        nParticles(kDp) * 1000 + nParticles(kDm) * 100 + nParticles(kD0) * 10 + nParticles(kDA) * 1;
    }
    return m_mcOpenCharm1;
  }

  int MCOpenCharm2()
  {
    if(m_mcOpenCharm2 != -1) {}
    else
    {
      m_mcOpenCharm2 = nVertices(kDstarp, kPip, kD0) * 1000 + nVertices(kDstarm, kPim, kDA) * 100 +
                       nVertices(kDstar0, kGamma, kD0) * 10 + nVertices(kDstarA, kGamma, kDA) * 1;
    }
    return m_mcOpenCharm2;
  }

  int MCOpenCharm3()
  {
    if(m_mcOpenCharm3 != -1) {}
    else
    {
      m_mcOpenCharm3 = nVertices(kDstarp, kPi0, kDp) * 1000 + nVertices(kDstarm, kPi0, kDm) * 100 +
                       nVertices(kDstar0, kPi0, kD0) * 10 + nVertices(kDstarA, kPi0, kDA) * 1;
    }
    return m_mcOpenCharm3;
  }

  int MCOpenCharmTag()
  {
    if(m_mcOpenCharmTag != -1) {}
    else
    {
      m_mcOpenCharmTag =
        nVertices(kDp, kKm, kPip, kPip) * 1000 + nVertices(kDp, kKm, kPip, kPip, kPi0) * 2000 +
        nVertices(kDp, kKs, kPip) * 3000 + nVertices(kDp, kKs, kPip, kPi0) * 4000 +
        nVertices(kDp, kKs, kPip, kPip, kPim) * 5000 + nVertices(kDm, kKp, kPim, kPim) * 100 +
        nVertices(kDm, kKp, kPim, kPim, kPi0) * 200 + nVertices(kDm, kKs, kPim) * 300 +
        nVertices(kDm, kKs, kPim, kPi0) * 400 + nVertices(kDm, kKs, kPim, kPim, kPip) * 500 +
        nVertices(kD0, kKm, kPip) * 10 + nVertices(kD0, kKm, kPip, kPi0) * 20 +
        nVertices(kDA, kKp, kPim) * 1 + nVertices(kDA, kKp, kPim, kPi0) * 2;
    }
    return m_mcOpenCharmTag;
  }

  // count the number of FSR gammas produced in this event

  int MCFSRGamma()
  {
    if(m_mcFSRGamma != -1) {}
    else
    {
      m_mcFSRGamma = nParticles(kFSRGamma);
    }
    return m_mcFSRGamma;
  }

  // id and four-momentum of particles coming from original particle (psi(2S),etc)

  int    MCDecayParticleID(unsigned int i);
  double MCDecayParticleEn(unsigned int i);
  double MCDecayParticlePx(unsigned int i);
  double MCDecayParticlePy(unsigned int i);
  double MCDecayParticlePz(unsigned int i);

  // flag for the production of chi_cJ

  int MCChicProduction()
  {
    if(m_mcChicProduction != -1) {}
    else if(hasDecay(kGamma, kChic0))
    {
      m_mcChicProduction = 10;
    }
    else if(hasDecay(kGamma, kChic1))
    {
      m_mcChicProduction = 11;
    }
    else if(hasDecay(kGamma, kChic2))
    {
      m_mcChicProduction = 12;
    }
    else if(hasParticle(kChic0))
    {
      m_mcChicProduction = 100;
    }
    else if(hasParticle(kChic1))
    {
      m_mcChicProduction = 101;
    }
    else if(hasParticle(kChic2))
    {
      m_mcChicProduction = 102;
    }
    else
    {
      m_mcChicProduction = 0;
    }
    return m_mcChicProduction;
  }

  // flag for the production of J/psi

  int MCJPsiProduction()
  {
    if(m_mcJPsiProduction != -1) {}
    else if(hasDecay(kJpsi, kPip, kPim))
    {
      m_mcJPsiProduction = 1;
    }
    else if(hasDecay(kJpsi, kPi0, kPi0))
    {
      m_mcJPsiProduction = 2;
    }
    else if(hasDecay(kJpsi, kEta))
    {
      m_mcJPsiProduction = 3;
    }
    else if(hasDecay(kJpsi, kGamma))
    {
      m_mcJPsiProduction = 4;
    }
    else if(hasDecay(kJpsi, kPi0))
    {
      m_mcJPsiProduction = 5;
    }
    else if(hasParticle(kJpsi))
    {
      m_mcJPsiProduction = 6;
    }
    else
    {
      m_mcJPsiProduction = 0;
    }
    return m_mcJPsiProduction;
  }

  // flag for the production of eta_c

  int MCEtacProduction()
  {
    if(m_mcEtacProduction != -1) {}
    else if(hasDecay(kEtac, kGamma))
    {
      m_mcEtacProduction = 1;
    }
    else if(hasParticle(kEtac))
    {
      m_mcEtacProduction = 2;
    }
    else
    {
      m_mcEtacProduction = 0;
    }
    return m_mcEtacProduction;
  }

  // flag for the production of h_c

  int MCHcProduction()
  {
    if(m_mcHcProduction != -1) {}
    else if(hasDecay(kHc, kPip, kPim))
    {
      m_mcHcProduction = 1;
    }
    else if(hasDecay(kHc, kPi0, kPi0))
    {
      m_mcHcProduction = 2;
    }
    else if(hasDecay(kHc, kEta))
    {
      m_mcHcProduction = 3;
    }
    else if(hasDecay(kHc, kGamma))
    {
      m_mcHcProduction = 4;
    }
    else if(hasDecay(kHc, kPi0))
    {
      m_mcHcProduction = 5;
    }
    else if(hasParticle(kHc))
    {
      m_mcHcProduction = 6;
    }
    else
    {
      m_mcHcProduction = 0;
    }
    return m_mcHcProduction;
  }

  // flag for the decay of the X(3872)

  int MCX3872Decay()
  {
    if(m_mcX3872Decay != -1) {}
    else if(hasVertex(kX3872, kRho0, kJpsi))
    {
      m_mcX3872Decay = 1;
    }
    else if(hasVertex(kX3872, kPip, kPim, kJpsi))
    {
      m_mcX3872Decay = 2;
    }
    else if(hasVertex(kX3872, kOmega, kJpsi))
    {
      m_mcX3872Decay = 3;
    }
    else if(hasVertex(kX3872, kGamma, kJpsi))
    {
      m_mcX3872Decay = 4;
    }
    else if(hasVertex(kX3872, kGamma, kPsi2S))
    {
      m_mcX3872Decay = 5;
    }
    else if(hasVertex(kX3872, kPi0, kChic0))
    {
      m_mcX3872Decay = 6;
    }
    else if(hasVertex(kX3872, kPi0, kChic1))
    {
      m_mcX3872Decay = 7;
    }
    else if(hasVertex(kX3872, kPi0, kChic2))
    {
      m_mcX3872Decay = 8;
    }
    else
    {
      m_mcX3872Decay = 0;
    }
    return m_mcX3872Decay;
  }

  // flag for the decay of the pi0

  int MCPi0Decay()
  {
    if(m_mcPi0Decay != -1) {}
    else
    {
      m_mcPi0Decay = nVertices(kPi0, kGamma, kGamma) * 1 + nVertices(kPi0, kGamma, kEp, kEm) * 10;
    }
    return m_mcPi0Decay;
  }

  // flag for the decay of the eta

  int MCEtaDecay()
  {
    if(m_mcEtaDecay != -1) {}
    else
    {
      m_mcEtaDecay = nVertices(kEta, kGamma, kGamma) * 1 + nVertices(kEta, kPi0, kPi0, kPi0) * 10 +
                     nVertices(kEta, kPip, kPim, kPi0) * 100 +
                     nVertices(kEta, kPip, kPim, kGamma) * 1000 +
                     nVertices(kEta, kEp, kEm, kGamma) * 10000;
    }
    return m_mcEtaDecay;
  }

  // flag for the decay of the etaprime

  int MCEtaprimeDecay()
  {
    if(m_mcEtaprimeDecay != -1) {}
    else
    {
      m_mcEtaprimeDecay = nVertices(kEtaprime, kPip, kPim, kEta) * 1 +
                          nVertices(kEtaprime, kPi0, kPi0, kEta) * 10 +
                          nVertices(kEtaprime, kGamma, kRho0) * 100 +
                          nVertices(kEtaprime, kGamma, kPip, kPim) * 1000 +
                          nVertices(kEtaprime, kGamma, kOmega) * 10000 +
                          nVertices(kEtaprime, kGamma, kGamma) * 100000 +
                          nVertices(kEtaprime, kPi0, kPi0, kPi0) * 1000000;
    }
    return m_mcEtaprimeDecay;
  }

  // flag for the decay of the phi

  int MCPhiDecay()
  {
    if(m_mcPhiDecay != -1) {}
    else
    {
      m_mcPhiDecay = nVertices(kPhi, kKp, kKm) * 1 + nVertices(kPhi, kKs, kKl) * 10 +
                     nVertices(kPhi, kPip, kPim, kPi0) * 100 +
                     (nVertices(kPhi, kRhop, kPim) + nVertices(kPhi, kRhom, kPip) +
                      nVertices(kPhi, kRho0, kPi0)) *
                       1000 +
                     nVertices(kPhi, kGamma, kEta) * 10000;
    }
    return m_mcPhiDecay;
  }

  // flag for the decay of the omega

  int MCOmegaDecay()
  {
    if(m_mcOmegaDecay != -1) {}
    else
    {
      m_mcOmegaDecay = nVertices(kOmega, kPip, kPim, kPi0) * 1 +
                       nVertices(kOmega, kPi0, kGamma) * 10 + nVertices(kOmega, kPip, kPim) * 100;
    }
    return m_mcOmegaDecay;
  }

  // flag for the decay of the ks

  int MCKsDecay()
  {
    if(m_mcKsDecay != -1) {}
    else
    {
      m_mcKsDecay = nVertices(kKs, kPip, kPim) * 1 + nVertices(kKs, kPi0, kPi0) * 10;
    }
    return m_mcKsDecay;
  }

  // helper function that returns the name of a particle

  string particleType(int id);

  // helper function that prints the mcParticleCol to the screen

  void printInformation();

  // pdg id's

  static const int kPsi2S      = 100443;
  static const int kGamma      = 22;
  static const int kFSRGamma   = -22;
  static const int kHc         = 10443;
  static const int kX3872      = 62;
  static const int kChic0      = 10441;
  static const int kChic1      = 20443;
  static const int kChic2      = 445;
  static const int kJpsi       = 443;
  static const int kEtac       = 441;
  static const int kPhi        = 333;
  static const int kOmega      = 223;
  static const int kPi0        = 111;
  static const int kPip        = 211;
  static const int kPim        = -211;
  static const int kRho0       = 113;
  static const int kRhop       = 213;
  static const int kRhom       = -213;
  static const int kEtaprime   = 331;
  static const int kEta        = 221;
  static const int kKs         = 310;
  static const int kKl         = 130;
  static const int kKp         = 321;
  static const int kKm         = -321;
  static const int kPp         = 2212;
  static const int kPm         = -2212;
  static const int kN          = 2112;
  static const int kAntiN      = -2112;
  static const int kDelta0     = 2114;
  static const int kDeltap     = 2214;
  static const int kDeltapp    = 2224;
  static const int kEp         = -11;
  static const int kEm         = 11;
  static const int kMup        = -13;
  static const int kMum        = 13;
  static const int kTaup       = -15;
  static const int kTaum       = 15;
  static const int kNuE        = 12;
  static const int kNuMu       = 14;
  static const int kNuTau      = 16;
  static const int kAntiNuE    = -12;
  static const int kAntiNuMu   = -14;
  static const int kAntiNuTau  = -16;
  static const int kF0600      = 9000221;
  static const int kK0         = 311;
  static const int kAntiK0     = -311;
  static const int kKstarp     = 323;
  static const int kKstarm     = -323;
  static const int kKstar0     = 313;
  static const int kAntiKstar0 = -313;
  static const int kLambda     = 3122;
  static const int kALambda    = -3122;
  static const int kDp         = 411;
  static const int kDm         = -411;
  static const int kD0         = 421;
  static const int kDA         = -421;
  static const int kDstarp     = 413;
  static const int kDstarm     = -413;
  static const int kDstar0     = 423;
  static const int kDstarA     = -423;

private:
  bool isSpecialParticle(int pdgID);
  int  fsParticleIndex(int idParticle);
  int  nFSParticles(int idParticle);
  int  nParticles(int idParticle);
  bool hasParticle(int idParticle);

  int  nVertices(int idParent, int idDaughter1, int idDaughter2, int idDaughter3 = 0,
                 int idDaughter4 = 0, int idDaughter5 = 0, int idDaughter6 = 0, int idDaughter7 = 0,
                 int idDaughter8 = 0, int idDaughter9 = 0, int idDaughter10 = 0);
  bool hasVertex(int idParent, int idDaughter1, int idDaughter2, int idDaughter3 = 0,
                 int idDaughter4 = 0, int idDaughter5 = 0, int idDaughter6 = 0, int idDaughter7 = 0,
                 int idDaughter8 = 0, int idDaughter9 = 0, int idDaughter10 = 0);

  int  nDecays(int idDaughter1, int idDaughter2, int idDaughter3 = 0, int idDaughter4 = 0,
               int idDaughter5 = 0, int idDaughter6 = 0, int idDaughter7 = 0, int idDaughter8 = 0,
               int idDaughter9 = 0, int idDaughter10 = 0);
  bool hasDecay(int idDaughter1, int idDaughter2, int idDaughter3 = 0, int idDaughter4 = 0,
                int idDaughter5 = 0, int idDaughter6 = 0, int idDaughter7 = 0, int idDaughter8 = 0,
                int idDaughter9 = 0, int idDaughter10 = 0);

  int    m_mcDecayCode1;
  int    m_mcDecayCode2;
  int    m_mcExtras;
  int    m_mcOpenCharm1;
  int    m_mcOpenCharm2;
  int    m_mcOpenCharm3;
  int    m_mcOpenCharmTag;
  int    m_mcChicProduction;
  int    m_mcJPsiProduction;
  int    m_mcHcProduction;
  int    m_mcEtacProduction;
  int    m_mcX3872Decay;
  int    m_mcPi0Decay;
  int    m_mcEtaDecay;
  int    m_mcEtaprimeDecay;
  int    m_mcPhiDecay;
  int    m_mcOmegaDecay;
  int    m_mcKsDecay;
  int    m_mcFSRGamma;
  double m_mcTotalEnergyInitial;
  double m_mcTotalEnergyFinal;

  vector<MCTruthParticle*> m_mcVector;
  vector<MCTruthParticle*> m_mcFinal;
  vector<MCTruthParticle*> m_mcInitial;
};

class MCTruthParticle
{

public:
  McParticle*              original;
  MCTruthParticle*         parent;
  vector<MCTruthParticle*> daughters;
  int                      pdgID;
  bool                     special;
  bool                     primary;
  bool                     initial;
  bool                     final;

  bool hasDaughters(int idDaughter1, int idDaughter2, int idDaughter3 = 0, int idDaughter4 = 0,
                    int idDaughter5 = 0, int idDaughter6 = 0, int idDaughter7 = 0,
                    int idDaughter8 = 0, int idDaughter9 = 0, int idDaughter10 = 0);
};

#endif
