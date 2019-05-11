#include "FSFilter/FSHelper.h"

//**********************************************
//
//   FSInfo constructor
//
//**********************************************

FSInfo::FSInfo(string FSName, NTupleHelper* nt, NTupleHelper* ntgen) :
  m_FSName(FSName),
  m_NT(nt),
  m_NTGen(ntgen),
  m_missingMassFit(false)
{

  // print out final state name

  cout << "FSFilter:  Initializing Final State " << FSName << endl;

  // set up member data

  m_particleNames          = FSInfo::getParticleNamesFromFSName(FSName);
  m_nChargedParticles      = FSInfo::getNChargedParticlesFromParticleNames(m_particleNames);
  pair<int, int> decayCode = FSInfo::getDecayCodeFromParticleNames(m_particleNames);
  m_decayCode1             = decayCode.first;
  m_decayCode2             = decayCode.second;

  // print out particle list

  for(unsigned int i = 0; i < m_particleNames.size(); i++)
  { cout << "FSFilter:      " << m_particleNames[i] << endl; } }

//**********************************************
//
//   FSParticle TRACK
//
//**********************************************

FSParticle::FSParticle(EvtRecTrack* tk, int trackIndex, string name, bool yuping) :
  m_track(tk),
  m_shower(NULL),
  m_pi0(NULL),
  m_eta(NULL),
  m_vee(NULL),
  m_name(name),
  m_trackIndex(trackIndex)
{

  // set mass

  m_mass = mass(m_name);

  // get kalman track

  m_kalTrack = m_track->mdcKalTrack();

  // set pid type (setPidType is static for some reason)

  if((name == "pi-") || (name == "pi+"))
    m_kalTrack->setPidType(RecMdcKalTrack::pion);
  else if((name == "K-") || (name == "K+"))
    m_kalTrack->setPidType(RecMdcKalTrack::kaon);
  else if((name == "p-") || (name == "p+"))
    m_kalTrack->setPidType(RecMdcKalTrack::proton);
  else if((name == "e-") || (name == "e+"))
    m_kalTrack->setPidType(RecMdcKalTrack::electron);
  else if((name == "mu-") || (name == "mu+"))
    m_kalTrack->setPidType(RecMdcKalTrack::muon);

  // setup for wtrack

  if((name == "pi-") || (name == "pi+"))
    m_initialWTrack = WTrackParameter(m_mass, m_kalTrack->getZHelix(), m_kalTrack->getZError());
  else if((name == "K-") || (name == "K+"))
    m_initialWTrack = WTrackParameter(m_mass, m_kalTrack->getZHelixK(), m_kalTrack->getZErrorK());
  else if((name == "p-") || (name == "p+"))
    m_initialWTrack = WTrackParameter(m_mass, m_kalTrack->getZHelixP(), m_kalTrack->getZErrorP());
  else if((name == "e-") || (name == "e+"))
    m_initialWTrack = WTrackParameter(m_mass, m_kalTrack->getZHelixE(), m_kalTrack->getZErrorE());
  else if((name == "mu-") || (name == "mu+"))
    m_initialWTrack = WTrackParameter(m_mass, m_kalTrack->getZHelixMu(), m_kalTrack->getZErrorMu());

  // YUPING recalibration of tracking errors

  if(yuping)
  {

    HepVector track_zHel(5, 0);
    if((name == "pi-") || (name == "pi+"))
    {
      YUPING::calibration(m_kalTrack, track_zHel, 0);
      m_initialWTrack = WTrackParameter(m_mass, track_zHel, m_kalTrack->getZError());
    }
    else if((name == "K-") || (name == "K+"))
    {
      YUPING::calibration(m_kalTrack, track_zHel, 1);
      m_initialWTrack = WTrackParameter(m_mass, track_zHel, m_kalTrack->getZErrorK());
    }
    else if((name == "e-") || (name == "e+"))
    {
      YUPING::calibration(m_kalTrack, track_zHel, 2);
      m_initialWTrack = WTrackParameter(m_mass, track_zHel, m_kalTrack->getZErrorE());
    }
    else if((name == "mu-") || (name == "mu+"))
    {
      YUPING::calibration(m_kalTrack, track_zHel, 3);
      m_initialWTrack = WTrackParameter(m_mass, track_zHel, m_kalTrack->getZErrorMu());
    }
  }

  // keep track of id's

  m_trackId.push_back(tk->trackId());

  // initial four-momentum

  setRawFourMomentum(m_kalTrack->p4(m_mass));
}

//**********************************************
//
//   FSParticle SHOWER
//
//**********************************************

FSParticle::FSParticle(EvtRecTrack* sh, string name) :
  m_track(NULL),
  m_shower(sh),
  m_pi0(NULL),
  m_eta(NULL),
  m_vee(NULL),
  m_name(name),
  m_trackIndex(0)
{

  m_mass = mass(m_name);

  m_showerId.push_back(sh->emcShower()->cellId());

  setRawFourMomentum(showerFourMomentum(sh->emcShower()));
}

//**********************************************
//
//   FSParticle PI0
//
//**********************************************

FSParticle::FSParticle(EvtRecPi0* pi0, string name) :
  m_track(NULL),
  m_shower(NULL),
  m_pi0(pi0),
  m_eta(NULL),
  m_vee(NULL),
  m_name(name),
  m_trackIndex(0)
{

  m_mass = mass(m_name);

  m_pi0Lo = const_cast<EvtRecTrack*>(pi0->loEnGamma());
  m_pi0Hi = const_cast<EvtRecTrack*>(pi0->hiEnGamma());

  m_showerId.push_back(m_pi0Lo->emcShower()->cellId());
  m_showerId.push_back(m_pi0Hi->emcShower()->cellId());

  setRawFourMomentum(showerFourMomentum(m_pi0Lo->emcShower()) +
                     showerFourMomentum(m_pi0Hi->emcShower()));
  setRawFourMomentumA(showerFourMomentum(m_pi0Lo->emcShower()));
  setRawFourMomentumB(showerFourMomentum(m_pi0Hi->emcShower()));
}

//**********************************************
//
//   FSParticle ETA
//
//**********************************************

FSParticle::FSParticle(EvtRecEtaToGG* eta, string name) :
  m_track(NULL),
  m_shower(NULL),
  m_pi0(NULL),
  m_eta(eta),
  m_vee(NULL),
  m_name(name),
  m_trackIndex(0)
{

  m_mass = mass(m_name);

  m_etaLo = const_cast<EvtRecTrack*>(eta->loEnGamma());
  m_etaHi = const_cast<EvtRecTrack*>(eta->hiEnGamma());

  m_showerId.push_back(m_etaLo->emcShower()->cellId());
  m_showerId.push_back(m_etaHi->emcShower()->cellId());

  setRawFourMomentum(showerFourMomentum(m_etaLo->emcShower()) +
                     showerFourMomentum(m_etaHi->emcShower()));
  setRawFourMomentumA(showerFourMomentum(m_etaLo->emcShower()));
  setRawFourMomentumB(showerFourMomentum(m_etaHi->emcShower()));
}

//**********************************************
//
//   FSParticle KSHORT AND LAMBDA
//
//**********************************************

FSParticle::FSParticle(EvtRecVeeVertex* vee, string name) :
  m_track(NULL),
  m_shower(NULL),
  m_pi0(NULL),
  m_eta(NULL),
  m_vee(vee),
  m_name(name),
  m_trackIndex(0)
{

  m_mass = mass(m_name);

  double mass1 = 0.0;
  double mass2 = 0.0;

  if(name == "Ks")
  {
    mass1 = mass("pi+");
    mass2 = mass("pi-");
  }
  if(name == "Lambda")
  {
    mass1 = mass("p+");
    mass2 = mass("pi-");
  }
  if(name == "ALambda")
  {
    mass1 = mass("p-");
    mass2 = mass("pi+");
  }

  // what is the particle order?

  bool rightorder = true;
  if(name == "Ks" && vee->pairDaughters().first->mdcKalTrack()->charge() < 0) rightorder = false;
  if(name == "Lambda" && vee->pairDaughters().first->mdcKalTrack()->charge() < 0)
    rightorder = false;
  if(name == "ALambda" && vee->pairDaughters().first->mdcKalTrack()->charge() > 0)
    rightorder = false;

  // now store information for the first and second particles

  if(rightorder) m_veeTrack1 = vee->pairDaughters().first;
  if(!rightorder) m_veeTrack1 = vee->pairDaughters().second;
  m_veeKalTrack1 = m_veeTrack1->mdcKalTrack();
  if(name == "Ks")
  {
    m_veeInitialWTrack1 =
      WTrackParameter(mass1, m_veeKalTrack1->getZHelix(), m_veeKalTrack1->getZError());
  }
  else
  {
    m_veeInitialWTrack1 =
      WTrackParameter(mass1, m_veeKalTrack1->getZHelixP(), m_veeKalTrack1->getZErrorP());
  }

  if(rightorder) m_veeTrack2 = vee->pairDaughters().second;
  if(!rightorder) m_veeTrack2 = vee->pairDaughters().first;
  m_veeKalTrack2 = m_veeTrack2->mdcKalTrack();
  m_veeInitialWTrack2 =
    WTrackParameter(mass2, m_veeKalTrack2->getZHelix(), m_veeKalTrack2->getZError());

  m_trackId.push_back(m_veeTrack1->trackId());
  m_trackId.push_back(m_veeTrack2->trackId());

  //m_rawFourMomentum = HepLorentzVector(vee->w()[0], vee->w()[1], vee->w()[2], vee->w()[3]);

  setRawFourMomentum(m_veeInitialWTrack1.p() + m_veeInitialWTrack2.p());
  setRawFourMomentumA(m_veeInitialWTrack1.p());
  setRawFourMomentumB(m_veeInitialWTrack2.p());
}

//**********************************************
//
//   FSParticle: check for duplicate tracks or showers
//
//**********************************************

bool FSParticle::duplicate(FSParticle* fsp)
{

  vector<int> trackId2 = fsp->trackId();
  if(m_trackId.size() > 0 && trackId2.size() > 0)
  {
    for(unsigned int i = 0; i < m_trackId.size(); i++)
    {
      for(unsigned int j = 0; j < trackId2.size(); j++)
      {
        if(m_trackId[i] == trackId2[j]) return true;
      }
    }
    if(name() == fsp->name() && m_trackId[0] > trackId2[0]) return true;
  }

  vector<int> showerId2 = fsp->showerId();
  if(m_showerId.size() > 0 && showerId2.size() > 0)
  {
    for(unsigned int i = 0; i < m_showerId.size(); i++)
    {
      for(unsigned int j = 0; j < showerId2.size(); j++)
      {
        if(m_showerId[i] == showerId2[j]) return true;
      }
    }
    if(name() == fsp->name() && m_showerId[0] > showerId2[0]) return true;
  }

  return false;
}

//**********************************************
//
//   FSCut constructor
//
//**********************************************

FSCut::FSCut(const string& initialization)
{

  // divide the initialization string into seperate words

  vector<string> words = FSInfo::parseString(initialization);

  if(words.size() != 5)
  {
    cout << "FSFilter ERROR: wrong arguments to FSCut parameter: " << endl;
    cout << initialization << endl;
    exit(0);
  }

  // save input as member data

  m_FSName      = words[0];
  m_submodeName = words[1];
  m_cutType     = words[2];
  m_lowCut      = atof(words[3].c_str());
  m_highCut     = atof(words[4].c_str());

  // quick checks

  if(m_cutType != "RawRecoil" && m_cutType != "RawMass" && m_cutType != "IntRecoil" &&
     m_cutType != "IntMass" && m_cutType != "FitRecoil" && m_cutType != "FitMass" &&
     m_cutType != "RawRecoilSquared" && m_cutType != "RawMassSquared" &&
     m_cutType != "IntRecoilSquared" && m_cutType != "IntMassSquared" &&
     m_cutType != "FitRecoilSquared" && m_cutType != "FitMassSquared")
  {
    cout << "FSFilter ERROR: wrong arguments to FSCut parameter: " << endl;
    cout << initialization << endl;
    cout << "cutType must be RawRecoil, RawMass, IntRecoil, FitRecoil, etc. " << endl;
    exit(0);
  }
}

//**********************************************
//
//   FSInfo functions to evaluate FSCuts
//
//**********************************************

vector<vector<unsigned int> >& FSInfo::submodeIndices(const string& submodeName)
{

  if(m_submodeIndices.find(submodeName) != m_submodeIndices.end())
    return m_submodeIndices[submodeName];

  vector<string> submodeParticles = FSInfo::getParticleNamesFromFSName(submodeName);

  vector<vector<unsigned int> > indices;

  for(unsigned int i = 0; i < submodeParticles.size(); i++)
  {

    vector<vector<unsigned int> > indicesTemp = indices;
    indices.clear();

    vector<unsigned int> pList;
    for(unsigned int j = 0; j < m_particleNames.size(); j++)
    {
      if(submodeParticles[i] == m_particleNames[j]) pList.push_back(j);
    }
    if(pList.size() == 0) return indices;

    for(unsigned int ipl = 0; ipl < pList.size(); ipl++)
    {
      if(i == 0)
      {
        vector<unsigned int> combo;
        combo.push_back(pList[ipl]);
        indices.push_back(combo);
      }
      else
      {
        for(unsigned int itmp = 0; itmp < indicesTemp.size(); itmp++)
        {
          vector<unsigned int> combo     = indicesTemp[itmp];
          bool                 duplicate = false;
          for(unsigned int ic = 0; ic < combo.size(); ic++)
          {
            if(pList[ipl] <= combo[ic])
            {
              duplicate = true;
              continue;
            }
          }
          if(!duplicate)
          {
            combo.push_back(pList[ipl]);
            indices.push_back(combo);
          }
        }
      }
    }
  }

  m_submodeIndices[submodeName] = indices;
  return m_submodeIndices[submodeName];
}

bool FSInfo::evaluateFSCuts(const vector<FSParticle*>& particleCombination,
                            const HepLorentzVector& pInitial, string fourVectorType)
{

  for(unsigned int icut = 0; icut < m_FSCuts.size(); icut++)
  {
    FSCut* fscut = m_FSCuts[icut];
    bool   lRaw  = fscut->Raw();
    bool   lInt  = fscut->Int();
    bool   lFit  = fscut->Fit();
    if((lRaw && fourVectorType != "Raw") || (lInt && fourVectorType != "Int") ||
       (lFit && fourVectorType != "Fit"))
      continue;
    bool pass = false;

    vector<vector<unsigned int> > indices = submodeIndices(fscut->submodeName());

    for(unsigned int i = 0; i < indices.size(); i++)
    {
      vector<unsigned int> indexCombo = indices[i];

      HepLorentzVector pTotal(0.0, 0.0, 0.0, 0.0);
      for(unsigned int j = 0; j < indexCombo.size(); j++)
      {
        if(lRaw) pTotal += particleCombination[indexCombo[j]]->rawFourMomentum();
        if(lInt) pTotal += particleCombination[indexCombo[j]]->intFourMomentum();
        if(lFit) pTotal += particleCombination[indexCombo[j]]->fitFourMomentum();
      }

      if(fscut->Mass())
      {
        double x;
        if(fscut->Squared()) { x = pTotal.m2(); }
        else
        {
          x = pTotal.m();
        }
        if(x > fscut->lowCut() && x < fscut->highCut())
        {
          pass = true;
          break;
        }
      }

      else if(fscut->Recoil())
      {
        HepLorentzVector pMiss = pInitial - pTotal;
        double           x;
        if(fscut->Squared()) { x = pMiss.m2(); }
        else
        {
          x = pMiss.m();
        }
        if(x > fscut->lowCut() && x < fscut->highCut())
        {
          pass = true;
          break;
        }
      }
    }

    if(!pass) return false;
  }

  return true;
}

//**********************************************
//
//   FSConstraint constructor
//
//**********************************************

FSConstraint::FSConstraint(const string& initialization)
{

  // divide the initialization string into seperate words

  vector<string> words = FSInfo::parseString(initialization);

  if(words.size() != 4)
  {
    cout << "FSFilter ERROR: wrong arguments to FSConstraint parameter: " << endl;
    cout << initialization << endl;
    exit(0);
  }

  // save input as member data

  m_initialization  = initialization;
  m_FSName          = words[0];
  m_submodeName     = words[1];
  m_constraintType  = words[2];
  m_constraintValue = atof(words[3].c_str());

  // quick checks

  if(m_constraintType != "Mass")
  {
    cout << "FSFilter ERROR: wrong arguments to FSConstraint parameter: " << endl;
    cout << initialization << endl;
    cout << "constraintType must be Mass (only Mass is currently allowed) " << endl;
    exit(0);
  }
}

//**********************************************
//
//   FSInfo functions to help with FSConstraints
//
//**********************************************

void FSInfo::addFSConstraint(FSConstraint* fsConstraint)
{

  if(submodeIndices(fsConstraint->submodeName()).size() == 0)
  {
    cout << "FSFilter ERROR: could not create a constraint for the " << endl;
    cout << "  FSConstraint with these arguments: " << endl;
    cout << fsConstraint->initialization() << endl;
    cout << "  and final state = " << fsConstraint->FSName() << endl;
    exit(0);
  }

  //if (submodeIndices(fsConstraint->submodeName()).size() > 1){
  //  cout << "FSFilter ERROR: could not create a constraint for the " << endl;
  //  cout << "  FSConstraint with these arguments: " << endl;
  //  cout << fsConstraint->initialization() << endl;
  //  cout << "  and final state = " << fsConstraint->FSName() << endl;
  //  cout << "... More than one combination per FS is currently not allowed." << endl;
  //  exit(0);
  //}

  vector<string> pnames = getParticleNamesFromFSName(fsConstraint->submodeName());
  for(unsigned int i = 0; i < pnames.size(); i++)
  {
    if((pnames[i] != "pi-") && (pnames[i] != "pi+") && (pnames[i] != "K-") && (pnames[i] != "K+") &&
       (pnames[i] != "p-") && (pnames[i] != "p+") && (pnames[i] != "mu-") && (pnames[i] != "mu+") &&
       (pnames[i] != "e-") && (pnames[i] != "e+"))
    {
      cout << "FSFilter ERROR: could not create a constraint for the " << endl;
      cout << "  FSConstraint with these arguments: " << endl;
      cout << fsConstraint->initialization() << endl;
      cout << "  and final state = " << fsConstraint->FSName() << endl;
      cout << "... Currently only tracks are allowed." << endl;
      cout << "(Maybe an easy extension to allow others)" << endl;
      exit(0);
    }
  }

  if((pnames.size() < 2) || (pnames.size() > 5))
  {
    cout << "FSFilter ERROR: could not create a constraint for the " << endl;
    cout << "  FSConstraint with these arguments: " << endl;
    cout << fsConstraint->initialization() << endl;
    cout << "  and final state = " << fsConstraint->FSName() << endl;
    cout << "... Currently only 2 to 5 decay particles are allowed." << endl;
    exit(0);
  }

  m_FSConstraints.push_back(fsConstraint);
}

//**********************************************
//
//   FSParticle UTILITY FUNCTIONS
//
//**********************************************

HepLorentzVector FSParticle::showerFourMomentum(RecEmcShower* sh)
{
  return HepLorentzVector(sh->energy() * sin(sh->theta()) * cos(sh->phi()),
                          sh->energy() * sin(sh->theta()) * sin(sh->phi()),
                          sh->energy() * cos(sh->theta()), sh->energy());
}

double FSParticle::mass(string name)
{
  if(name == "pi0")
    return 0.1349766;
  else if((name == "pi-") || (name == "pi+"))
    return 0.139570;
  else if(name == "Ks")
    return 0.497614;
  else if((name == "K-") || (name == "K+"))
    return 0.493677;
  else if(name == "gamma")
    return 0.0;
  else if(name == "eta")
    return 0.547853;
  else if((name == "p-") || (name == "p+"))
    return 0.938272;
  else if((name == "mu-") || (name == "mu+"))
    return 0.105658;
  else if((name == "e-") || (name == "e+"))
    return 0.000511;
  else if((name == "ALambda") || (name == "Lambda"))
    return 1.115683;
  return 0.0;
}

//**********************************************
//
//   FSInfo: MODE NUMBERING UTILITIES, ETC.
//
//**********************************************

vector<string> FSInfo::getParticleNamesFromFSName(const string& FSName)
{

  // some quick checks

  if((FSName.size() == 0) || (FSName.find("_") == string::npos))
  {
    cout << "FSFilter ERROR: error in final state name: " << FSName << endl;
    exit(1);
  }

  // a list of allowed particle names

  string singleParticles[16] = {"pi0", "pi-", "pi+", "Ks",  "K-", "K+", "gamma",   "eta",
                                "p-",  "p+",  "mu-", "mu+", "e-", "e+", "ALambda", "Lambda"};

  // parse FSName digit by digit, starting at the end

  int            index  = 0;
  bool           lcode2 = false;
  vector<string> particleNamesTmp;
  for(int i = FSName.size() - 1; i >= 0 && index < 16; i--)
  {
    string digit = FSName.substr(i, 1);
    if((digit == "0") || (digit == "1") || (digit == "2") || (digit == "3") || (digit == "4") ||
       (digit == "5") || (digit == "6") || (digit == "7") || (digit == "8") || (digit == "9"))
    {
      int num = atoi(digit.c_str());
      for(int j = 0; j < num; j++) { particleNamesTmp.push_back(singleParticles[index]); }
      index++;
    }
    else if(digit == "_" && !lcode2)
    {
      if(index > 7)
      {
        cout << "FSFilter ERROR: error in final state name: " << FSName << endl;
        exit(1);
      }
      lcode2 = true;
      index  = 7;
    }
    else
    {
      break;
    }
  }

  // make sure we have particles

  if(particleNamesTmp.size() == 0)
  {
    cout << "FSFilter ERROR: error in final state name: " << FSName << endl;
    exit(1);
  }

  // now reverse the order of the particle names

  vector<string> particleNames;
  for(int i = particleNamesTmp.size() - 1; i >= 0; i--)
  { particleNames.push_back(particleNamesTmp[i]); } return particleNames;
}

pair<int, int> FSInfo::getDecayCodeFromParticleNames(const vector<string>& particleNames)
{

  int decayCode1 = 0;
  int decayCode2 = 0;
  for(unsigned int i = 0; i < particleNames.size(); i++)
  {
    if(particleNames[i] == "pi0") { decayCode1 += 1; }
    else if(particleNames[i] == "pi-")
    {
      decayCode1 += 10;
    }
    else if(particleNames[i] == "pi+")
    {
      decayCode1 += 100;
    }
    else if(particleNames[i] == "Ks")
    {
      decayCode1 += 1000;
    }
    else if(particleNames[i] == "K-")
    {
      decayCode1 += 10000;
    }
    else if(particleNames[i] == "K+")
    {
      decayCode1 += 100000;
    }
    else if(particleNames[i] == "gamma")
    {
      decayCode1 += 1000000;
    }
    else if(particleNames[i] == "eta")
    {
      decayCode2 += 1;
    }
    else if(particleNames[i] == "p-")
    {
      decayCode2 += 10;
    }
    else if(particleNames[i] == "p+")
    {
      decayCode2 += 100;
    }
    else if(particleNames[i] == "mu-")
    {
      decayCode2 += 1000;
    }
    else if(particleNames[i] == "mu+")
    {
      decayCode2 += 10000;
    }
    else if(particleNames[i] == "e-")
    {
      decayCode2 += 100000;
    }
    else if(particleNames[i] == "e+")
    {
      decayCode2 += 1000000;
    }
    else if(particleNames[i] == "ALambda")
    {
      decayCode2 += 10000000;
    }
    else if(particleNames[i] == "Lambda")
    {
      decayCode2 += 100000000;
    }
  }

  return pair<int, int>(decayCode1, decayCode2);
}

pair<int, int> FSInfo::getDecayCodeFromFSName(const string& FSName)
{

  return getDecayCodeFromParticleNames(getParticleNamesFromFSName(FSName));
}

int FSInfo::getNChargedParticlesFromParticleNames(const vector<string>& particleNames)
{

  int nChargedParticles = 0;
  for(unsigned int i = 0; i < particleNames.size(); i++)
  {
    if(particleNames[i] == "pi0") { nChargedParticles += 0; }
    else if(particleNames[i] == "pi-")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "pi+")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "Ks")
    {
      nChargedParticles += 2;
    }
    else if(particleNames[i] == "K-")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "K+")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "gamma")
    {
      nChargedParticles += 0;
    }
    else if(particleNames[i] == "eta")
    {
      nChargedParticles += 0;
    }
    else if(particleNames[i] == "p-")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "p+")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "mu-")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "mu+")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "e-")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "e+")
    {
      nChargedParticles += 1;
    }
    else if(particleNames[i] == "ALambda")
    {
      nChargedParticles += 2;
    }
    else if(particleNames[i] == "Lambda")
    {
      nChargedParticles += 2;
    }
  }

  return nChargedParticles;
}

vector<string> FSInfo::parseString(const string& inputString)
{

  vector<string> words;
  string         word("");
  for(unsigned int j = 0; j < inputString.size(); j++)
  {
    if(!isspace(inputString[j]))
    {
      word += inputString[j];
      if((j == (inputString.size() - 1)) && (!word.empty()))
      {
        words.push_back(word);
        word = "";
      }
    }
    else if(!word.empty())
    {
      words.push_back(word);
      word = "";
    }
  }

  return words;
}
