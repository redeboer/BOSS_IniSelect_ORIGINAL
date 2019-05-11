#include "FSFilter/NTupleHelper.h"

// ********************************************************************
//    EVENT INFORMATION
// ********************************************************************

void NTupleHelper::fillEvent(SmartDataPtr<Event::EventHeader> eventHeader,
                             SmartDataPtr<EvtRecEvent> evtRecEvent, double cmEnergy)
{
  fillDouble("Run", eventHeader->runNumber());
  fillDouble("Event", eventHeader->eventNumber());
  fillDouble("BeamEnergy", cmEnergy / 2.0);
  fillDouble("NTracks", evtRecEvent->totalCharged());
  fillDouble("NShowers", evtRecEvent->totalNeutral());
}

// ********************************************************************
//    FOUR MOMENTA
// ********************************************************************

void NTupleHelper::fill4Momentum(int index, TString subindex, string tag, const HepLorentzVector& p)
{
  fillDouble(concatName(tag, "Px", index, subindex), p.px());
  fillDouble(concatName(tag, "Py", index, subindex), p.py());
  fillDouble(concatName(tag, "Pz", index, subindex), p.pz());
  fillDouble(concatName(tag, "En", index, subindex), p.e());
}

void NTupleHelper::fill4Momentum(int index, TString subindex, string tag)
{
  fillDouble(concatName(tag, "Px", index, subindex), 0.0);
  fillDouble(concatName(tag, "Py", index, subindex), 0.0);
  fillDouble(concatName(tag, "Pz", index, subindex), 0.0);
  fillDouble(concatName(tag, "En", index, subindex), 0.0);
}

void NTupleHelper::fill4Momentum(int index, TString subindex, string tag, const RecEmcShower& p)
{
  fillDouble(concatName(tag, "Px", index, subindex), p.energy() * sin(p.theta()) * cos(p.phi()));
  fillDouble(concatName(tag, "Py", index, subindex), p.energy() * sin(p.theta()) * sin(p.phi()));
  fillDouble(concatName(tag, "Pz", index, subindex), p.energy() * cos(p.theta()));
  fillDouble(concatName(tag, "En", index, subindex), p.energy());
}

// ********************************************************************
//    SHOWER INFORMATION
// ********************************************************************

void NTupleHelper::fillShower(int index, TString subindex, string tag, EvtRecTrack* tk,
                              vector<FSParticle*> pi0s, SmartDataPtr<EvtRecTrackCol> evtRecTrackCol)
{

  RecEmcShower* p = tk->emcShower();

  // find the best pi0 containing this shower

  double pi0Pull = 10000.0;
  for(unsigned int i = 0; i < pi0s.size(); i++)
  {
    if(p->cellId() == pi0s[i]->pi0Lo()->emcShower()->cellId() ||
       p->cellId() == pi0s[i]->pi0Hi()->emcShower()->cellId())
    {
      double pi0PullTest = sqrt(fabs(pi0s[i]->pi0()->chisq()));
      if(pi0PullTest < pi0Pull) pi0Pull = pi0PullTest;
    }
  }

  // find the closest track to this shower
  //  (copied from RhoPiAlg, mostly)

  Hep3Vector shpos(p->x(), p->y(), p->z());
  double     dang = 200.;
  for(EvtRecTrackIterator iTrk = evtRecTrackCol->begin(); iTrk != evtRecTrackCol->end(); iTrk++)
  {
    if(!(*iTrk)->isExtTrackValid()) continue;
    RecExtTrack* extTrk = (*iTrk)->extTrack();
    if(extTrk->emcVolumeNumber() == -1) continue;
    Hep3Vector extpos = extTrk->emcPosition();
    double     angd   = extpos.angle(shpos);
    if(angd < dang) { dang = angd; }
  }
  dang = dang * 180 / 3.1415927;

  // does this shower also have an associated track?

  double match = -1.0;
  if(tk->isMdcTrackValid() && tk->isEmcShowerValid() && tk->mdcTrack()->p() > 0) match = 1.0;

  // put information in the tree

  fillDouble(concatName(tag, "Time", index, subindex), p->time());
  fillDouble(concatName(tag, "Energy", index, subindex), p->energy());
  fillDouble(concatName(tag, "CosTheta", index, subindex), cos(p->theta()));
  fillDouble(concatName(tag, "E925", index, subindex), p->e3x3() / p->e5x5());
  fillDouble(concatName(tag, "Pi0Pull", index, subindex), pi0Pull);
  fillDouble(concatName(tag, "Dang", index, subindex), dang);
  fillDouble(concatName(tag, "Match", index, subindex), match);
}

// ********************************************************************
//    TRACK INFORMATION
// ********************************************************************

void NTupleHelper::fillTrack(int index, TString subindex, string tag, EvtRecTrack* p,
                             VertexParameter beamSpot, int trackIndex, bool pidStudies)
{

  // set up particle ID information

  ParticleID* pid = ParticleID::instance();
  pid->init();
  pid->setMethod(pid->methodProbability());
  pid->setRecTrack(p);
  pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2());
  pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton() | pid->onlyElectron() |
                pid->onlyMuon());
  pid->calculate();
  double ProbPi = pid->probPion();
  double ProbK  = pid->probKaon();
  double ProbP  = pid->probProton();
  double ProbE  = pid->probElectron();
  double ProbMu = pid->probMuon();

  // get vertex information
  //  (copied from RhoPiAlg)

  RecMdcTrack* mdcTrk = p->mdcTrack();
  HepVector    a      = mdcTrk->helix();
  HepSymMatrix Ea     = mdcTrk->err();
  HepPoint3D   point0(0., 0., 0.);
  HepPoint3D   IP(beamSpot.vx());
  VFHelix      helixip(point0, a, Ea);
  helixip.pivot(IP);
  HepVector vecipa = helixip.a();
  double    RVtx   = fabs(vecipa[0]);
  double    ZVtx   = vecipa[3];

  // calculate E/p

  double EP = -1.0;
  if(p->isMdcTrackValid() && p->isEmcShowerValid() && p->mdcTrack()->p() > 0)
  { EP = p->emcShower()->energy() / p->mdcTrack()->p(); } // muon chamber information

  double MucDepth = -1.0;
  if(p->isMucTrackValid())
  {
    RecMucTrack* muc = p->mucTrack();
    //int nLayers = muc->numLayers();
    //int nHits = muc->numHits();
    MucDepth = muc->depth();
  }

  // put information in the tree

  fillDouble(concatName(tag, "ProbPi", index, subindex), ProbPi);
  fillDouble(concatName(tag, "ProbK", index, subindex), ProbK);
  fillDouble(concatName(tag, "ProbP", index, subindex), ProbP);
  fillDouble(concatName(tag, "ProbMu", index, subindex), ProbMu);
  fillDouble(concatName(tag, "ProbE", index, subindex), ProbE);
  fillDouble(concatName(tag, "RVtx", index, subindex), RVtx);
  fillDouble(concatName(tag, "ZVtx", index, subindex), ZVtx);
  fillDouble(concatName(tag, "CosTheta", index, subindex), cos(p->mdcTrack()->theta()));
  fillDouble(concatName(tag, "EP", index, subindex), EP);
  fillDouble(concatName(tag, "MucDepth", index, subindex), MucDepth);

  // record extra PID information if pidStudies is true

  if(!pidStudies) return;

  double probPH  = -1.0;
  double normPH  = -1.0;
  double errorPH = -1.0;
  if(p->mdcDedx())
  {
    probPH  = p->mdcDedx()->probPH();
    normPH  = p->mdcDedx()->normPH();
    errorPH = p->mdcDedx()->errorPH();
  }
  fillDouble(concatName(tag, "ProbPH", index, subindex), probPH);
  fillDouble(concatName(tag, "NormPH", index, subindex), normPH);
  fillDouble(concatName(tag, "ErrorPH", index, subindex), errorPH);
  fillDouble(concatName(tag, "Index", index, subindex), trackIndex);
}

// ********************************************************************
//    PI0 INFORMATION
// ********************************************************************

void NTupleHelper::fillPi0(int index, TString subindex, string tag, EvtRecPi0* p)
{
  fillDouble(concatName(tag, "Mass", index, subindex), p->unconMass());
  fillDouble(concatName(tag, "Chi2", index, subindex), p->chisq());
}

// ********************************************************************
//    ETA INFORMATION
// ********************************************************************

void NTupleHelper::fillEta(int index, TString subindex, string tag, EvtRecEtaToGG* p)
{
  fillDouble(concatName(tag, "Mass", index, subindex), p->unconMass());
  fillDouble(concatName(tag, "Chi2", index, subindex), p->chisq());
}

// ********************************************************************
//    VEE INFORMATION
// ********************************************************************

void NTupleHelper::fillVee(int index, TString subindex, string tag, FSParticle* p)
{
  fillDouble(concatName(tag, "Mass", index, subindex), p->vee()->mass());
  fillDouble(concatName(tag, "Chi2", index, subindex), p->vee()->chi2());
  fillDouble(concatName(tag, "LSigma", index, subindex), p->veeLSigma());
  fillDouble(concatName(tag, "Sigma", index, subindex), p->veeSigma());
  fillDouble(concatName(tag, "2ndChi2", index, subindex), p->vee2ndChi2());
  fillDouble(concatName(tag, "Vx", index, subindex), p->veeVx());
  fillDouble(concatName(tag, "Vy", index, subindex), p->veeVy());
  fillDouble(concatName(tag, "Vz", index, subindex), p->veeVz());
}

// ********************************************************************
//    TRY TO TAG PSI(2S) --> PI+ PI- JPSI AND GAMMA GAMMA JPSI
// ********************************************************************

void NTupleHelper::fillJPsiFinder(SmartDataPtr<EvtRecTrackCol> evtRecTrackCol)
{

  HepLorentzVector pcm(3.686093 * sin(0.011), 0, 0, 3.686093);

  // look for psi(2S) --> pi+ pi- J/psi

  double jpsiPiPiRecoil = -10000.0;
  for(EvtRecTrackIterator iTrk1 = evtRecTrackCol->begin(); iTrk1 != evtRecTrackCol->end(); iTrk1++)
  {
    if((*iTrk1)->isMdcKalTrackValid())
    {
      RecMdcKalTrack* mdcKalTrack1 = (*iTrk1)->mdcKalTrack();
      mdcKalTrack1->setPidType(RecMdcKalTrack::pion);
      for(EvtRecTrackIterator iTrk2 = iTrk1; iTrk2 != evtRecTrackCol->end(); iTrk2++)
      {
        if((*iTrk2)->isMdcKalTrackValid() && (*iTrk2)->trackId() != (*iTrk1)->trackId())
        {
          RecMdcKalTrack* mdcKalTrack2 = (*iTrk2)->mdcKalTrack();
          if(mdcKalTrack1->charge() == mdcKalTrack2->charge()) continue;
          mdcKalTrack2->setPidType(RecMdcKalTrack::pion);
          HepLorentzVector p1    = mdcKalTrack1->p4(0.139570);
          HepLorentzVector p2    = mdcKalTrack2->p4(0.139570);
          double           mjpsi = (pcm - p1 - p2).m();
          if(fabs(mjpsi - 3.096916) < fabs(jpsiPiPiRecoil)) jpsiPiPiRecoil = mjpsi - 3.096916;
        }
      }
    }
  }

  // look for psi(2S) --> gamma gamma J/psi

  double jpsiGGRecoil = -10000.0;
  for(EvtRecTrackIterator iG1 = evtRecTrackCol->begin(); iG1 != evtRecTrackCol->end(); iG1++)
  {
    if((*iG1)->isEmcShowerValid())
    {
      RecEmcShower* sh1 = (*iG1)->emcShower();
      for(EvtRecTrackIterator iG2 = iG1; iG2 != evtRecTrackCol->end(); iG2++)
      {
        if((*iG2)->isEmcShowerValid() &&
           (*iG2)->emcShower()->cellId() != (*iG1)->emcShower()->cellId())
        {
          RecEmcShower*    sh2 = (*iG2)->emcShower();
          HepLorentzVector p1(sh1->energy() * sin(sh1->theta()) * cos(sh1->phi()),
                              sh1->energy() * sin(sh1->theta()) * sin(sh1->phi()),
                              sh1->energy() * cos(sh1->theta()), sh1->energy());
          HepLorentzVector p2(sh2->energy() * sin(sh2->theta()) * cos(sh2->phi()),
                              sh2->energy() * sin(sh2->theta()) * sin(sh2->phi()),
                              sh2->energy() * cos(sh2->theta()), sh2->energy());
          double           mjpsi = (pcm - p1 - p2).m();
          if(fabs(mjpsi - 3.096916) < fabs(jpsiGGRecoil)) jpsiGGRecoil = mjpsi - 3.096916;
        }
      }
    }
  }

  // put information in the tree

  fillDouble("JPsiPiPiRecoil", jpsiPiPiRecoil);
  fillDouble("JPsiGGRecoil", jpsiGGRecoil);
}

// ********************************************************************
//    MC TRUTH INFORMATION
// ********************************************************************

void NTupleHelper::fillMCTruth(MCTruthHelper* mc, FSInfo* fs, string tag)
{
  fillDouble("MCDecayCode1", mc->MCDecayCode1());
  fillDouble("MCDecayCode2", mc->MCDecayCode2());
  fillDouble("MCExtras", mc->MCExtras());
  fillDouble("MCOpenCharm1", mc->MCOpenCharm1());
  fillDouble("MCOpenCharm2", mc->MCOpenCharm2());
  fillDouble("MCOpenCharm3", mc->MCOpenCharm3());
  fillDouble("MCOpenCharmTag", mc->MCOpenCharmTag());
  fillDouble("MCTotalEnergy", mc->MCTotalEnergyInitial());
  fillDouble("MCChicProduction", mc->MCChicProduction());
  fillDouble("MCJPsiProduction", mc->MCJPsiProduction());
  fillDouble("MCHcProduction", mc->MCHcProduction());
  fillDouble("MCEtacProduction", mc->MCEtacProduction());
  fillDouble("MCX3872Decay", mc->MCX3872Decay());
  fillDouble("MCPi0Decay", mc->MCPi0Decay());
  fillDouble("MCEtaDecay", mc->MCEtaDecay());
  fillDouble("MCEtaprimeDecay", mc->MCEtaprimeDecay());
  fillDouble("MCPhiDecay", mc->MCPhiDecay());
  fillDouble("MCOmegaDecay", mc->MCOmegaDecay());
  fillDouble("MCKsDecay", mc->MCKsDecay());
  fillDouble("MCFSRGamma", mc->MCFSRGamma());
  fillDouble("MCDecayParticle1", mc->MCDecayParticleID(0));
  fillDouble("MCDecayParticle2", mc->MCDecayParticleID(1));
  fillDouble("MCDecayParticle3", mc->MCDecayParticleID(2));
  fillDouble("MCDecayParticle4", mc->MCDecayParticleID(3));
  fillDouble("MCDecayParticle5", mc->MCDecayParticleID(4));
  fillDouble("MCDecayParticle6", mc->MCDecayParticleID(5));
  fillDouble("MCDecayParticleEnP1", mc->MCDecayParticleEn(0));
  fillDouble("MCDecayParticleEnP2", mc->MCDecayParticleEn(1));
  fillDouble("MCDecayParticleEnP3", mc->MCDecayParticleEn(2));
  fillDouble("MCDecayParticleEnP4", mc->MCDecayParticleEn(3));
  fillDouble("MCDecayParticleEnP5", mc->MCDecayParticleEn(4));
  fillDouble("MCDecayParticleEnP6", mc->MCDecayParticleEn(5));
  fillDouble("MCDecayParticlePxP1", mc->MCDecayParticlePx(0));
  fillDouble("MCDecayParticlePxP2", mc->MCDecayParticlePx(1));
  fillDouble("MCDecayParticlePxP3", mc->MCDecayParticlePx(2));
  fillDouble("MCDecayParticlePxP4", mc->MCDecayParticlePx(3));
  fillDouble("MCDecayParticlePxP5", mc->MCDecayParticlePx(4));
  fillDouble("MCDecayParticlePxP6", mc->MCDecayParticlePx(5));
  fillDouble("MCDecayParticlePyP1", mc->MCDecayParticlePy(0));
  fillDouble("MCDecayParticlePyP2", mc->MCDecayParticlePy(1));
  fillDouble("MCDecayParticlePyP3", mc->MCDecayParticlePy(2));
  fillDouble("MCDecayParticlePyP4", mc->MCDecayParticlePy(3));
  fillDouble("MCDecayParticlePyP5", mc->MCDecayParticlePy(4));
  fillDouble("MCDecayParticlePyP6", mc->MCDecayParticlePy(5));
  fillDouble("MCDecayParticlePzP1", mc->MCDecayParticlePz(0));
  fillDouble("MCDecayParticlePzP2", mc->MCDecayParticlePz(1));
  fillDouble("MCDecayParticlePzP3", mc->MCDecayParticlePz(2));
  fillDouble("MCDecayParticlePzP4", mc->MCDecayParticlePz(3));
  fillDouble("MCDecayParticlePzP5", mc->MCDecayParticlePz(4));
  fillDouble("MCDecayParticlePzP6", mc->MCDecayParticlePz(5));
  if(fs && fs->exclusive())
  {
    vector<MCTruthParticle*> mcPList       = mc->MCParticleList();
    vector<string>           particleNames = fs->particleNames();
    if((fs->decayCode1() == mc->MCDecayCode1()) && (fs->decayCode2() == mc->MCDecayCode2()) &&
       (mc->MCExtras() == 0))
    {
      fillDouble("MCSignal", 1.0);
      if(mcPList.size() != particleNames.size())
      {
        cout << "FSFilter ERROR: fillMCTruth needs work!" << endl;
        mc->printInformation();
        exit(1);
      }
      for(unsigned int i = 0; i < particleNames.size(); i++)
      {
        fill4Momentum((i + 1), "", tag, mcPList[i]->original->initialFourMomentum());
        if((particleNames[i] == "pi0") || (particleNames[i] == "eta") ||
           (particleNames[i] == "Ks") || (particleNames[i] == "Lambda") ||
           (particleNames[i] == "ALambda"))
        {
          bool rightorder = true;
          if((mcPList[i]->original->daughterList().size()) != 2)
          {
            fill4Momentum((i + 1), "a", tag);
            fill4Momentum((i + 1), "b", tag);
            continue;
          }
          McParticle da = *(mcPList[i]->original->daughterList()[0]);
          McParticle db = *(mcPList[i]->original->daughterList()[1]);
          if(((particleNames[i] == "pi0") || (particleNames[i] == "eta")) &&
             (da.initialFourMomentum().e() > db.initialFourMomentum().e()))
            rightorder = false;
          if(((particleNames[i] == "Lambda") || (particleNames[i] == "Ks")) &&
             (da.particleProperty() < db.particleProperty()))
            rightorder = false;
          if((particleNames[i] == "ALambda") && (da.particleProperty() < db.particleProperty()))
            rightorder = false;
          if(rightorder)
          {
            fill4Momentum((i + 1), "a", tag, da.initialFourMomentum());
            fill4Momentum((i + 1), "b", tag, db.initialFourMomentum());
          }
          if(!rightorder)
          {
            fill4Momentum((i + 1), "b", tag, da.initialFourMomentum());
            fill4Momentum((i + 1), "a", tag, db.initialFourMomentum());
          }
        }
      }
    }
    else
    {
      fillDouble("MCSignal", 0.0);
      for(unsigned int i = 0; i < particleNames.size(); i++)
      {
        fill4Momentum((i + 1), "", tag);
        if((particleNames[i] == "pi0") || (particleNames[i] == "eta") ||
           (particleNames[i] == "Ks") || (particleNames[i] == "Lambda") ||
           (particleNames[i] == "ALambda"))
        {
          fill4Momentum((i + 1), "a", tag);
          fill4Momentum((i + 1), "b", tag);
        }
      }
    }
  }
}

// ********************************************************************
//    VERTEX INFORMATION
// ********************************************************************

void NTupleHelper::fillVertices(const VertexParameter& beamSpot,
                                const VertexParameter& primaryVertex)
{
  fillDouble("BeamVx", beamSpot.vx().x());
  fillDouble("BeamVy", beamSpot.vx().y());
  fillDouble("BeamVz", beamSpot.vx().z());
  fillDouble("PrimaryVx", primaryVertex.vx().x());
  fillDouble("PrimaryVy", primaryVertex.vx().y());
  fillDouble("PrimaryVz", primaryVertex.vx().z());
}

// ********************************************************************
//    HELPER FUNCTIONS
// ********************************************************************

NTupleHelper::NTupleHelper(NTuple::Tuple* Tree)
{
  m_Tree         = Tree;
  m_bookingStage = true;
  m_doubleMap.clear();
  if(m_Tree == NULL)
    cout << "ERROR:  null tree pointer -- "
         << "check for duplicate final states in configuration" << endl;
  assert(m_Tree != NULL);
}

void NTupleHelper::write()
{
  for(map<string, double>::iterator mapItr = m_doubleMap.begin(); mapItr != m_doubleMap.end();
      mapItr++)
  { m_ntupleDoubleMap[mapItr->first] = mapItr->second; } m_Tree->write();
  m_bookingStage = false;
}

void NTupleHelper::fillDouble(string name, double value)
{
  if(m_bookingStage && !containsEntry(name)) { m_Tree->addItem(name, m_ntupleDoubleMap[name]); }
  if(!m_bookingStage && !containsEntry(name))
  {
    cout << "NTUPLEHELPER:  Variable " << name << " has not been booked." << endl;
    exit(0);
  }
  m_doubleMap[name] = value;
}

bool NTupleHelper::containsEntry(string name)
{
  map<string, double>::iterator mapItr = m_doubleMap.find(name);
  if(mapItr != m_doubleMap.end()) return true;
  return false;
}

string NTupleHelper::concatName(string tag, string base, int index, TString subindex)
{
  stringstream name;
  name << tag;
  name << base;
  name << "P";
  name << index;
  name << subindex;
  return name.str();
}
