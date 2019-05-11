#include "FSFilter/MCTruthHelper.h"

MCTruthHelper::~MCTruthHelper()
{
  for(unsigned int i = 0; i < m_mcVector.size(); i++) { delete m_mcVector[i]; }
}

bool MCTruthHelper::isSpecialParticle(int pdgID)
{
  if(abs(pdgID) >= 1 && abs(pdgID) <= 8) return true;
  if(abs(pdgID) >= 81 && abs(pdgID) <= 100) return true;
  if(abs(pdgID) == 21 || abs(pdgID) == 23) return true;
  if(abs(pdgID) == 1103 || abs(pdgID) == 2101) return true;
  if(abs(pdgID) == 2103 || abs(pdgID) == 2203) return true;
  if(abs(pdgID) == 3101 || abs(pdgID) == 3103) return true;
  if(abs(pdgID) == 3201 || abs(pdgID) == 3203) return true;
  if(abs(pdgID) == 3303) return true;
  return false;
}

int MCTruthHelper::MCDecayParticleID(unsigned int i)
{
  if(i >= m_mcInitial.size()) return 0;
  return m_mcInitial[i]->pdgID;
}

double MCTruthHelper::MCDecayParticleEn(unsigned int i)
{
  if(i >= m_mcInitial.size()) return 0;
  return m_mcInitial[i]->original->initialFourMomentum().e();
}
double MCTruthHelper::MCDecayParticlePx(unsigned int i)
{
  if(i >= m_mcInitial.size()) return 0;
  return m_mcInitial[i]->original->initialFourMomentum().px();
}
double MCTruthHelper::MCDecayParticlePy(unsigned int i)
{
  if(i >= m_mcInitial.size()) return 0;
  return m_mcInitial[i]->original->initialFourMomentum().py();
}
double MCTruthHelper::MCDecayParticlePz(unsigned int i)
{
  if(i >= m_mcInitial.size()) return 0;
  return m_mcInitial[i]->original->initialFourMomentum().pz();
}

MCTruthHelper::MCTruthHelper(SmartDataPtr<Event::McParticleCol> mcParticleCol)
{

  m_mcVector.clear();
  m_mcFinal.clear();
  m_mcInitial.clear();

  //   1. create new MC particle types to supplement McParticle

  for(Event::McParticleCol::iterator mcp = mcParticleCol->begin(); mcp != mcParticleCol->end();
      mcp++)
  {
    McParticle*      mcpold = *mcp;
    MCTruthParticle* mcpnew = new MCTruthParticle;
    mcpnew->original        = mcpold;
    mcpnew->pdgID           = mcpold->particleProperty();
    mcpnew->special         = isSpecialParticle(mcpnew->pdgID);
    mcpnew->primary         = mcpold->primaryParticle();
    m_mcVector.push_back(mcpnew);
  }

  //   2. create links between parents and daughters

  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    MCTruthParticle* mcp = m_mcVector[i];
    mcp->parent          = NULL;
    if(mcp->original->mother().particleProperty() == mcp->original->particleProperty())
    { continue; } for(unsigned int j = 0; j < m_mcVector.size(); j++)
    {
      MCTruthParticle* mcparent = m_mcVector[j];
      if(mcparent->original == &(mcp->original->mother()))
      {
        mcp->parent = mcparent;
        mcp->parent->daughters.push_back(mcp);
        break;
      }
    }
  }

  //   3.  sort the daughters

  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    vector<MCTruthParticle*> daughters = m_mcVector[i]->daughters;
    if(daughters.size() >= 2)
    {
      for(unsigned int j = 0; j < daughters.size() - 1; j++)
      {
        for(unsigned int k = j + 1; k < daughters.size(); k++)
        {
          if(daughters[j]->pdgID > daughters[k]->pdgID)
          {
            MCTruthParticle* mcptemp = daughters[k];
            daughters[k]             = daughters[j];
            daughters[j]             = mcptemp;
          }
        }
      }
      m_mcVector[i]->daughters = daughters;
    }
  }

  //  4. mark initial particles

  int  nTries                = 0;
  bool foundInitialParticles = false;
  while(!foundInitialParticles)
  {
    nTries++;
    int nInitialParticles = 0;
    for(unsigned int i = 0; i < m_mcVector.size(); i++)
    {
      MCTruthParticle* mcpnew = m_mcVector[i];
      mcpnew->initial         = false;
      if(mcpnew->special || mcpnew->primary) continue;
      if(mcpnew->parent == NULL)
      {
        mcpnew->initial = true;
        nInitialParticles++;
        continue;
      }
      if(mcpnew->parent->special || mcpnew->primary)
      {
        mcpnew->initial = true;
        nInitialParticles++;
        continue;
      }
    }
    foundInitialParticles = true;
    if(nInitialParticles == 1)
    {
      foundInitialParticles = false;
      for(unsigned int i = 0; i < m_mcVector.size(); i++)
      {
        if(m_mcVector[i]->initial)
        {
          m_mcVector[i]->initial = false;
          m_mcVector[i]->special = true;
        }
      }
    }
    if(nInitialParticles == 0)
    {
      foundInitialParticles = false;
      for(unsigned int i = 0; i < m_mcVector.size(); i++)
      {
        m_mcVector[i]->initial = false;
        m_mcVector[i]->primary = false;
      }
    }
    if(nTries == 10)
    {
      cout << "FSFILTER WARNING:  trouble finding initial particles, skipping event" << endl;
      printInformation();
      return;
    }
  }

  //   5. mark final particles based on type and decay

  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    MCTruthParticle* mcpnew = m_mcVector[i];
    mcpnew->final           = false;
    if(mcpnew->primary || mcpnew->special) continue;
    if(fsParticleIndex(mcpnew->pdgID) < 0) continue;
    if((mcpnew->pdgID == kPi0) && !(mcpnew->hasDaughters(kGamma, kGamma)) &&
       mcpnew->daughters.size() > 0)
      continue;
    if((mcpnew->pdgID == kEta) && !(mcpnew->hasDaughters(kGamma, kGamma)) &&
       mcpnew->daughters.size() > 0)
      continue;
    if((mcpnew->pdgID == kKs) && !(mcpnew->hasDaughters(kPip, kPim)) &&
       mcpnew->daughters.size() > 0)
      continue;
    if((mcpnew->pdgID == kLambda) && !(mcpnew->hasDaughters(kPp, kPim)) &&
       mcpnew->daughters.size() > 0)
      continue;
    if((mcpnew->pdgID == kALambda) && !(mcpnew->hasDaughters(kPm, kPip)) &&
       mcpnew->daughters.size() > 0)
      continue;
    mcpnew->final = true;
  }

  //   6. unmark final particles that are coming from other
  //        final particles or interactions

  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    MCTruthParticle* mcpnew = m_mcVector[i];
    if(mcpnew->final)
    {
      MCTruthParticle* mcparent = mcpnew->parent;
      while(mcparent)
      {
        if(mcparent->final)
        {
          mcpnew->final = false;
          break;
        }
        mcparent = mcparent->parent;
      }
    }
  }

  //  7. create a vector of the initial particles and sort them

  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    MCTruthParticle* mcpnew = m_mcVector[i];
    if(mcpnew->initial) m_mcInitial.push_back(mcpnew);
  }
  if(m_mcInitial.size() >= 2)
  {
    for(unsigned int j = 0; j < m_mcInitial.size() - 1; j++)
    {
      for(unsigned int k = j + 1; k < m_mcInitial.size(); k++)
      {
        if(m_mcInitial[j]->pdgID > m_mcInitial[k]->pdgID)
        {
          MCTruthParticle* mcptemp = m_mcInitial[k];
          m_mcInitial[k]           = m_mcInitial[j];
          m_mcInitial[j]           = mcptemp;
        }
      }
    }
  }

  //  8. create a vector of the final particles and sort them

  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    MCTruthParticle* mcpnew = m_mcVector[i];
    if(mcpnew->final) m_mcFinal.push_back(mcpnew);
  }
  if(m_mcFinal.size() >= 2)
  {
    for(unsigned int j = 0; j < m_mcFinal.size() - 1; j++)
    {
      for(unsigned int k = j + 1; k < m_mcFinal.size(); k++)
      {
        if(fsParticleIndex(m_mcFinal[j]->pdgID) < fsParticleIndex(m_mcFinal[k]->pdgID))
        {
          MCTruthParticle* mcptemp = m_mcFinal[k];
          m_mcFinal[k]             = m_mcFinal[j];
          m_mcFinal[j]             = mcptemp;
        }
      }
    }
  }

  // check that we found daughters of the initial particle (psi(2S),etc)

  if((m_mcInitial.size() == 0) && (m_mcVector.size() != 0))
  {
    cout << "FSFILTER ERROR:  empty decay list" << endl;
    printInformation();
    exit(0);
  }

  // initialize flags to -1

  m_mcDecayCode1         = -1;
  m_mcDecayCode2         = -1;
  m_mcExtras             = -1;
  m_mcOpenCharm1         = -1;
  m_mcOpenCharm2         = -1;
  m_mcOpenCharm3         = -1;
  m_mcOpenCharmTag       = -1;
  m_mcChicProduction     = -1;
  m_mcJPsiProduction     = -1;
  m_mcHcProduction       = -1;
  m_mcEtacProduction     = -1;
  m_mcX3872Decay         = -1;
  m_mcPi0Decay           = -1;
  m_mcEtaDecay           = -1;
  m_mcEtaprimeDecay      = -1;
  m_mcPhiDecay           = -1;
  m_mcOmegaDecay         = -1;
  m_mcKsDecay            = -1;
  m_mcFSRGamma           = -1;
  m_mcTotalEnergyInitial = -1;
  m_mcTotalEnergyFinal   = -1;
}

double MCTruthHelper::MCTotalEnergyFinal()
{
  if(m_mcTotalEnergyFinal > 0) {}
  else
  {
    m_mcTotalEnergyFinal = 0.0;
    for(unsigned int i = 0; i < m_mcFinal.size(); i++)
    { m_mcTotalEnergyFinal += m_mcFinal[i]->original->initialFourMomentum().e(); }
    for(unsigned int i = 0; i < m_mcVector.size(); i++)
    {
      if(m_mcVector[i]->pdgID == kFSRGamma && !m_mcVector[i]->primary && m_mcVector[i]->parent &&
         !m_mcVector[i]->parent->final)
      { m_mcTotalEnergyFinal += m_mcVector[i]->original->initialFourMomentum().e(); } }
  }
  return m_mcTotalEnergyFinal;
}

double MCTruthHelper::MCTotalEnergyInitial()
{
  if(m_mcTotalEnergyInitial > 0) {}
  else
  {
    m_mcTotalEnergyInitial = 0.0;
    for(unsigned int i = 0; i < m_mcInitial.size(); i++)
    { m_mcTotalEnergyInitial += m_mcInitial[i]->original->initialFourMomentum().e(); } }
  return m_mcTotalEnergyInitial;
}

int MCTruthHelper::fsParticleIndex(int idParticle)
{
  if(idParticle == kNuE) return 33;
  if(idParticle == kNuMu) return 33;
  if(idParticle == kNuTau) return 33;
  if(idParticle == kAntiNuE) return 33;
  if(idParticle == kAntiNuMu) return 33;
  if(idParticle == kAntiNuTau) return 33;
  if(idParticle == kKl) return 32;
  if(idParticle == kN) return 31;
  if(idParticle == kAntiN) return 30;
  if(idParticle == kLambda) return 28;
  if(idParticle == kALambda) return 27;
  if(idParticle == kEp) return 26;
  if(idParticle == kEm) return 25;
  if(idParticle == kMup) return 24;
  if(idParticle == kMum) return 23;
  if(idParticle == kPp) return 22;
  if(idParticle == kPm) return 21;
  if(idParticle == kEta) return 20;
  if(idParticle == kGamma) return 16;
  if(idParticle == kKp) return 15;
  if(idParticle == kKm) return 14;
  if(idParticle == kKs) return 13;
  if(idParticle == kPip) return 12;
  if(idParticle == kPim) return 11;
  if(idParticle == kPi0) return 10;
  return -1;
}

int MCTruthHelper::nFSParticles(int idParticle)
{
  int n = 0;
  for(unsigned int i = 0; i < m_mcFinal.size(); i++)
  {
    if(m_mcFinal[i]->pdgID == idParticle) n++;
  }
  if(n > 9)
  {
    cout << "FSFilter WARNING:  found " << n << " generated particles "
         << "with ID = " << idParticle << " but returning 9 " << endl;
    return 9;
  }
  return n;
}

int MCTruthHelper::nParticles(int idParticle)
{
  int n = 0;
  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    if(m_mcVector[i]->pdgID == idParticle) n++;
  }
  if(n > 9)
  {
    cout << "FSFilter WARNING:  found " << n << " generated particles "
         << "with ID = " << idParticle << " but returning 9 " << endl;
    return 9;
  }
  return n;
}

bool MCTruthHelper::hasParticle(int idParticle)
{
  return (nParticles(idParticle) > 0);
}

bool MCTruthParticle::hasDaughters(int idDaughter1, int idDaughter2, int idDaughter3,
                                   int idDaughter4, int idDaughter5, int idDaughter6,
                                   int idDaughter7, int idDaughter8, int idDaughter9,
                                   int idDaughter10)
{

  // create a vector of daughter id's to search for

  vector<int> idDaughters;
  if(idDaughter1 != 0) idDaughters.push_back(idDaughter1);
  if(idDaughter2 != 0) idDaughters.push_back(idDaughter2);
  if(idDaughter3 != 0) idDaughters.push_back(idDaughter3);
  if(idDaughter4 != 0) idDaughters.push_back(idDaughter4);
  if(idDaughter5 != 0) idDaughters.push_back(idDaughter5);
  if(idDaughter6 != 0) idDaughters.push_back(idDaughter6);
  if(idDaughter7 != 0) idDaughters.push_back(idDaughter7);
  if(idDaughter8 != 0) idDaughters.push_back(idDaughter8);
  if(idDaughter9 != 0) idDaughters.push_back(idDaughter9);
  if(idDaughter10 != 0) idDaughters.push_back(idDaughter10);
  if(idDaughters.size() < 2) return false;

  // create a vector of daughter id's from this MCTruthParticle

  vector<int> idDaughtersCheck;
  for(unsigned int j = 0; j < daughters.size(); j++)
  {
    int id = daughters[j]->pdgID;
    if(id != MCTruthHelper::kFSRGamma) idDaughtersCheck.push_back(id);
  }

  // if the numbers of daughters don't match return false

  if(idDaughters.size() != idDaughtersCheck.size()) return false;

  // sort the vectors of daughter id's

  for(unsigned int i1 = 0; i1 < idDaughters.size() - 1; i1++)
  {
    for(unsigned int i2 = i1 + 1; i2 < idDaughters.size(); i2++)
    {
      if(idDaughters[i1] > idDaughters[i2])
      {
        int temp        = idDaughters[i1];
        idDaughters[i1] = idDaughters[i2];
        idDaughters[i2] = temp;
      }
      if(idDaughtersCheck[i1] > idDaughtersCheck[i2])
      {
        int temp             = idDaughtersCheck[i1];
        idDaughtersCheck[i1] = idDaughtersCheck[i2];
        idDaughtersCheck[i2] = temp;
      }
    }
  }

  // check if the daughter id's match

  for(unsigned int i1 = 0; i1 < idDaughters.size(); i1++)
  {
    if(idDaughters[i1] != idDaughtersCheck[i1]) return false;
  }

  return true;
}

int MCTruthHelper::nVertices(int idParent, int idDaughter1, int idDaughter2, int idDaughter3,
                             int idDaughter4, int idDaughter5, int idDaughter6, int idDaughter7,
                             int idDaughter8, int idDaughter9, int idDaughter10)
{
  int n = 0;
  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    MCTruthParticle* mcp = m_mcVector[i];
    if(mcp->pdgID == idParent &&
       mcp->hasDaughters(idDaughter1, idDaughter2, idDaughter3, idDaughter4, idDaughter5,
                         idDaughter6, idDaughter7, idDaughter8, idDaughter9, idDaughter10))
      n++;
  }
  if(n > 9)
  {
    cout << "FSFilter WARNING:  found " << n << " generated vertices with "
         << "parent = " << idParent << " and "
         << "daughters = " << idDaughter1 << " " << idDaughter2 << " " << idDaughter3 << " "
         << idDaughter4 << " " << idDaughter5 << " " << idDaughter6 << " " << idDaughter7 << " "
         << idDaughter8 << " " << idDaughter9 << " " << idDaughter10 << " "
         << " but returning 9 " << endl;
    return 9;
  }
  return n;
}

bool MCTruthHelper::hasVertex(int idParent, int idDaughter1, int idDaughter2, int idDaughter3,
                              int idDaughter4, int idDaughter5, int idDaughter6, int idDaughter7,
                              int idDaughter8, int idDaughter9, int idDaughter10)
{
  return (nVertices(idParent, idDaughter1, idDaughter2, idDaughter3, idDaughter4, idDaughter5,
                    idDaughter6, idDaughter7, idDaughter8, idDaughter9, idDaughter10) > 0);
}

int MCTruthHelper::nDecays(int idDaughter1, int idDaughter2, int idDaughter3, int idDaughter4,
                           int idDaughter5, int idDaughter6, int idDaughter7, int idDaughter8,
                           int idDaughter9, int idDaughter10)
{
  int n = 0;
  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    MCTruthParticle* mcp = m_mcVector[i];
    if(mcp->hasDaughters(idDaughter1, idDaughter2, idDaughter3, idDaughter4, idDaughter5,
                         idDaughter6, idDaughter7, idDaughter8, idDaughter9, idDaughter10))
      n++;
  }
  if(n > 9)
  {
    cout << "FSFilter WARNING:  found " << n << " generated decays with "
         << "daughters = " << idDaughter1 << " " << idDaughter2 << " " << idDaughter3 << " "
         << idDaughter4 << " " << idDaughter5 << " " << idDaughter6 << " " << idDaughter7 << " "
         << idDaughter8 << " " << idDaughter9 << " " << idDaughter10 << " "
         << " but returning 9 " << endl;
    return 9;
  }
  return n;
}

bool MCTruthHelper::hasDecay(int idDaughter1, int idDaughter2, int idDaughter3, int idDaughter4,
                             int idDaughter5, int idDaughter6, int idDaughter7, int idDaughter8,
                             int idDaughter9, int idDaughter10)
{
  return (nDecays(idDaughter1, idDaughter2, idDaughter3, idDaughter4, idDaughter5, idDaughter6,
                  idDaughter7, idDaughter8, idDaughter9, idDaughter10) > 0);
}

string MCTruthHelper::particleType(int id)
{

  string name("");

  if(id == kPsi2S)
    name = "psi(2S)";
  else if(id == kGamma)
    name = "gamma";
  else if(id == kFSRGamma)
    name = "FSRgamma";
  else if(id == kHc)
    name = "h_c";
  else if(id == kChic0)
    name = "chi_c0";
  else if(id == kChic1)
    name = "chi_c1";
  else if(id == kChic2)
    name = "chi_c2";
  else if(id == kJpsi)
    name = "J/psi";
  else if(id == kEtac)
    name = "eta_c";
  else if(id == kPhi)
    name = "phi";
  else if(id == kOmega)
    name = "omega";
  else if(id == kPi0)
    name = "pi0";
  else if(id == kPip)
    name = "pi+";
  else if(id == kPim)
    name = "pi-";
  else if(id == kRho0)
    name = "rho0";
  else if(id == kRhop)
    name = "rho+";
  else if(id == kRhom)
    name = "rho-";
  else if(id == kEtaprime)
    name = "etaprime";
  else if(id == kEta)
    name = "eta";
  else if(id == kKs)
    name = "K_S0";
  else if(id == kKl)
    name = "K_L0";
  else if(id == kKp)
    name = "K+";
  else if(id == kKm)
    name = "K-";
  else if(id == kPp)
    name = "p+";
  else if(id == kPm)
    name = "p-";
  else if(id == kN)
    name = "N";
  else if(id == kAntiN)
    name = "antiN";
  else if(id == kDelta0)
    name = "Delta0";
  else if(id == kDeltap)
    name = "Delta+";
  else if(id == kDeltapp)
    name = "Delta++";
  else if(id == kEp)
    name = "e+";
  else if(id == kEm)
    name = "e-";
  else if(id == kMup)
    name = "mu+";
  else if(id == kMum)
    name = "mu-";
  else if(id == kTaup)
    name = "tau+";
  else if(id == kTaum)
    name = "tau-";
  else if(id == kNuE)
    name = "nu";
  else if(id == kNuMu)
    name = "nu";
  else if(id == kNuTau)
    name = "nu";
  else if(id == kAntiNuE)
    name = "nu";
  else if(id == kAntiNuMu)
    name = "nu";
  else if(id == kAntiNuTau)
    name = "nu";
  else if(id == kF0600)
    name = "f0(600)";
  else if(id == kK0)
    name = "K0";
  else if(id == kAntiK0)
    name = "K0";
  else if(id == kKstarp)
    name = "K*+";
  else if(id == kKstarm)
    name = "K*-";
  else if(id == kKstar0)
    name = "K*0";
  else if(id == kAntiKstar0)
    name = "K*0";
  else if(id == kLambda)
    name = "Lambda";
  else if(id == kALambda)
    name = "ALambda";
  else if(id == kDp)
    name = "D+";
  else if(id == kDm)
    name = "D-";
  else if(id == kD0)
    name = "D0";
  else if(id == kDA)
    name = "D0bar";
  else if(id == kDstarp)
    name = "D*+";
  else if(id == kDstarm)
    name = "D*-";
  else if(id == kDstar0)
    name = "D*0";
  else if(id == kDstarA)
    name = "D*0bar";
  else
  {
    stringstream ss;
    ss << id;
    name = ss.str();
  }

  return name;
}

void MCTruthHelper::printInformation()
{

  cout << "--------  NEW TRUTH INFORMATION ------------" << endl;
  cout << "MCDecayCode1         = " << MCDecayCode1() << endl;
  cout << "MCDecayCode2         = " << MCDecayCode2() << endl;
  cout << "MCExtras             = " << MCExtras() << endl;
  cout << "MCTotalEnergyInitial = " << MCTotalEnergyInitial() << endl;
  cout << "MCTotalEnergyFinal   = " << MCTotalEnergyFinal() << endl;
  cout << "----------------------------------------" << endl;
  cout << "MCDecayParticleID1 = " << particleType(MCDecayParticleID(0)) << endl;
  cout << "MCDecayParticleID2 = " << particleType(MCDecayParticleID(1)) << endl;
  cout << "MCDecayParticleID3 = " << particleType(MCDecayParticleID(2)) << endl;
  cout << "MCDecayParticleID4 = " << particleType(MCDecayParticleID(3)) << endl;
  cout << "MCDecayParticleID5 = " << particleType(MCDecayParticleID(4)) << endl;
  cout << "MCDecayParticleID6 = " << particleType(MCDecayParticleID(5)) << endl;
  cout << "MCDecayParticleID7 = " << particleType(MCDecayParticleID(6)) << endl;
  cout << "MCDecayParticleID8 = " << particleType(MCDecayParticleID(7)) << endl;
  cout << "----------------------------------------" << endl;
  for(unsigned int i = 0; i < m_mcVector.size(); i++)
  {
    MCTruthParticle* mcpnew = m_mcVector[i];
    McParticle*      mcpold = m_mcVector[i]->original;
    if(mcpnew->special) cout << "S";
    cout << "\t";
    if(mcpnew->primary) cout << "P";
    cout << "\t";
    if(mcpnew->initial) cout << "I";
    cout << "\t";
    if(mcpnew->final) cout << "F";
    cout << "\t";
    string parent("");
    if(mcpnew->parent) parent = particleType(mcpnew->parent->pdgID);
    cout << particleType(mcpnew->pdgID) << "\tfrom\t" << parent << endl;
  }
  cout << "----------------------------------------" << endl;
}
