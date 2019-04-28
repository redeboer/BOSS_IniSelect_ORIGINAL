#ifndef FSHELPER_H
#define FSHELPER_H

using namespace std;

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEtaToGG.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include <map>
#include <string>

#include "VertexFit/KalmanKinematicFit.h"
//#include "VertexFit/KinematicFit.h"
#include "VertexFit/Helix.h"
#include "VertexFit/VertexFit.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "FSFilter/NTupleHelper.h"
#include "FSFilter/YUPING.h"

class NTupleHelper;
class FSParticle;
class FSCut;
class FSConstraint;

//********************************************************************
//********************************************************************
//********************************************************************
//
//   The FSInfo class:  this class holds information about a
//                      given final state (FS)
//
//********************************************************************
//********************************************************************
//********************************************************************

class FSInfo
{

public:
  FSInfo(string FSName, NTupleHelper* nt, NTupleHelper* ntgen);

  // the name of the final state
  //  (this encodes a list of the final state particles
  //   and directions about whether to reconstruct them
  //   inclusively or exclusively)

  string FSName() { return m_FSName; }

  // is this an inclusive or exclusive final state?

  bool exclusive() { return (m_FSName.find("EXC") != string::npos); }
  bool inclusive() { return (m_FSName.find("INC") != string::npos); }

  // a list of particles associated with this final state

  vector<string>& particleNames() { return m_particleNames; }

  // check to see if a particle is contained in the list

  bool hasParticle(const string& particleName)
  {
    for(unsigned int i = 0; i < m_particleNames.size(); i++)
    {
      if(m_particleNames[i] == particleName) return true;
    }
    return false;
  }

  // count the number of charged particles in this final state

  int nChargedParticles() { return m_nChargedParticles; }

  // is this an all neutral final state?

  bool allNeutral() { return m_nChargedParticles == 0; }

  // ntuples associated with this final state

  NTupleHelper* NT() { return m_NT; }
  NTupleHelper* NTGen() { return m_NTGen; }

  // final state decay codes (encoding the list of final state particles)

  int decayCode1() { return m_decayCode1; }
  int decayCode2() { return m_decayCode2; }

  // missing mass fits

  bool   missingMassFit() { return m_missingMassFit; }
  double missingMassValue() { return m_missingMassValue; }
  bool   setMissingMass(double missingMass)
  {
    if(inclusive()) m_missingMassFit = true;
    m_missingMassValue = missingMass;
  }

  // all combinations of reconstructed particles
  //  (the inner vector is a given event hypothesis,
  //   the outer vector holds all hypotheses)

  vector<vector<FSParticle*> >& particleCombinations() { return m_particleCombinations; }

  // check to see if a certain event hypothesis satisfies the list of
  //  cuts on intermediate masses, etc.

  bool evaluateFSCuts(const vector<FSParticle*>& particleCombination,
                      const HepLorentzVector& pInitial, string fourVectorType);

  // externally set the list of particle combinations

  void setParticleCombinations(const vector<vector<FSParticle*> >& particleCombinations)
  {
    m_particleCombinations = particleCombinations;
  }

  // add a new FSCut (a cut on intermediate mass, for example)

  void addFSCut(FSCut* fsCut) { m_FSCuts.push_back(fsCut); }

  // add and retrieve FSConstraint

  void                  addFSConstraint(FSConstraint* fsConstraint);
  vector<FSConstraint*> getFSConstraints() { return m_FSConstraints; }

  // static functions to unpack the final state name and parse strings

  static vector<string> getParticleNamesFromFSName(const string& FSName);
  static pair<int, int> getDecayCodeFromParticleNames(const vector<string>& particleNames);
  static pair<int, int> getDecayCodeFromFSName(const string& FSName);
  static int            getNChargedParticlesFromParticleNames(const vector<string>& particleNames);
  static vector<string> parseString(const string& inputString);

  // returns all combinations of indices for a submode of this final state
  //    example:  final state = K+ K- pi+ pi+ pi- pi- pi0  submode = pi+ pi0
  //                  returns 2,6; 3,6

  vector<vector<unsigned int> >& submodeIndices(const string& submodeName);

private:
  // private member data

  string         m_FSName;
  vector<string> m_particleNames;
  int            m_nChargedParticles;
  NTupleHelper*  m_NT;
  NTupleHelper*  m_NTGen;

  int m_decayCode1;
  int m_decayCode2;

  bool   m_missingMassFit;
  double m_missingMassValue;

  vector<vector<FSParticle*> > m_particleCombinations;

  vector<FSCut*>        m_FSCuts;
  vector<FSConstraint*> m_FSConstraints;

  map<string, vector<vector<unsigned int> > > m_submodeIndices;
};

//********************************************************************
//********************************************************************
//********************************************************************
//
//   The FSParticle class:  this class holds information about
//                          a reconstructed final state particle
//
//********************************************************************
//********************************************************************
//********************************************************************

class FSParticle
{

public:
  FSParticle(EvtRecTrack* tk, int trackIndex, string name, bool yuping = false);
  FSParticle(EvtRecTrack* sh, string name);
  FSParticle(EvtRecPi0* pi0, string name);
  FSParticle(EvtRecEtaToGG* eta, string name);
  FSParticle(EvtRecVeeVertex* vee, string name);

  // TRACK

  EvtRecTrack*    track() { return m_track; }
  RecMdcKalTrack* kalTrack() { return m_kalTrack; }
  WTrackParameter initialWTrack() { return m_initialWTrack; }
  WTrackParameter vertexFitWTrack() { return m_vertexFitWTrack; }
  int             trackIndex() { return m_trackIndex; }
  void            setVertexFitWTrack(const WTrackParameter& wtrack) { m_vertexFitWTrack = wtrack; }

  // SHOWER

  EvtRecTrack* shower() { return m_shower; }

  // PI0

  EvtRecPi0*   pi0() { return m_pi0; }
  EvtRecTrack* pi0Lo() { return m_pi0Lo; }
  EvtRecTrack* pi0Hi() { return m_pi0Hi; }

  // ETA

  EvtRecEtaToGG* eta() { return m_eta; }
  EvtRecTrack*   etaLo() { return m_etaLo; }
  EvtRecTrack*   etaHi() { return m_etaHi; }

  // KSHORT AND LAMBDA

  EvtRecVeeVertex* vee() { return m_vee; }
  EvtRecTrack*     veeTrack1() { return m_veeTrack1; }
  RecMdcKalTrack*  veeKalTrack1() { return m_veeKalTrack1; }
  WTrackParameter  veeInitialWTrack1() { return m_veeInitialWTrack1; }
  WTrackParameter  veeVertexFitWTrack1() { return m_veeVertexFitWTrack1; }
  EvtRecTrack*     veeTrack2() { return m_veeTrack2; }
  RecMdcKalTrack*  veeKalTrack2() { return m_veeKalTrack2; }
  WTrackParameter  veeInitialWTrack2() { return m_veeInitialWTrack2; }
  WTrackParameter  veeVertexFitWTrack2() { return m_veeVertexFitWTrack2; }
  WTrackParameter  veeVertexFitWTrack() { return m_veeVertexFitWTrack; }
  double           veeLSigma() { return m_veeLSigma; }
  double           veeSigma() { return m_veeSigma; }
  double           vee2ndChi2() { return m_vee2ndChi2; }
  double           veeVx() { return m_veeVertex.x(); }
  double           veeVy() { return m_veeVertex.y(); }
  double           veeVz() { return m_veeVertex.z(); }
  void setVeeVertexFitWTrack1(const WTrackParameter& wtrack) { m_veeVertexFitWTrack1 = wtrack; }
  void setVeeVertexFitWTrack2(const WTrackParameter& wtrack) { m_veeVertexFitWTrack2 = wtrack; }
  void setVeeVertexFitWTrack(const WTrackParameter& wtrack) { m_veeVertexFitWTrack = wtrack; }
  void setVeeLSigma(double veeLSigma) { m_veeLSigma = veeLSigma; }
  void setVeeSigma(double veeSigma) { m_veeSigma = veeSigma; }
  void setVee2ndChi2(double chi2) { m_vee2ndChi2 = chi2; }
  void setVeeVertex(HepPoint3D veeVertex) { m_veeVertex = veeVertex; }

  // COMMON INFORMATION

  string name() { return m_name; }
  double mass() { return m_mass; }

  // the raw, intermediate, and kinematically fit (and extra) four-vectors
  //   (A and B are for daughters -- for example the pions from a Ks)

  HepLorentzVector rawFourMomentum() { return m_rawFourMomentum; }
  HepLorentzVector rawFourMomentumA() { return m_rawFourMomentumA; }
  HepLorentzVector rawFourMomentumB() { return m_rawFourMomentumB; }
  HepLorentzVector intFourMomentum() { return m_intFourMomentum; }
  HepLorentzVector intFourMomentumA() { return m_intFourMomentumA; }
  HepLorentzVector intFourMomentumB() { return m_intFourMomentumB; }
  HepLorentzVector fitFourMomentum() { return m_fitFourMomentum; }
  HepLorentzVector fitFourMomentumA() { return m_fitFourMomentumA; }
  HepLorentzVector fitFourMomentumB() { return m_fitFourMomentumB; }
  HepLorentzVector extFourMomentum() { return m_extFourMomentum; }
  HepLorentzVector extFourMomentumA() { return m_extFourMomentumA; }
  HepLorentzVector extFourMomentumB() { return m_extFourMomentumB; }

  void setRawFourMomentum(const HepLorentzVector& p)
  {
    m_rawFourMomentum = p;
    m_intFourMomentum = p;
    m_fitFourMomentum = p;
  }
  void setRawFourMomentumA(const HepLorentzVector& p)
  {
    m_rawFourMomentumA = p;
    m_intFourMomentumA = p;
    m_fitFourMomentumA = p;
  }
  void setRawFourMomentumB(const HepLorentzVector& p)
  {
    m_rawFourMomentumB = p;
    m_intFourMomentumB = p;
    m_fitFourMomentumB = p;
  }
  void setIntFourMomentum(const HepLorentzVector& p)
  {
    m_intFourMomentum = p;
    m_fitFourMomentum = p;
  }
  void setIntFourMomentumA(const HepLorentzVector& p)
  {
    m_intFourMomentumA = p;
    m_fitFourMomentumA = p;
  }
  void setIntFourMomentumB(const HepLorentzVector& p)
  {
    m_intFourMomentumB = p;
    m_fitFourMomentumB = p;
  }
  void setFitFourMomentum(const HepLorentzVector& p) { m_fitFourMomentum = p; }
  void setFitFourMomentumA(const HepLorentzVector& p) { m_fitFourMomentumA = p; }
  void setFitFourMomentumB(const HepLorentzVector& p) { m_fitFourMomentumB = p; }
  void setExtFourMomentum(const HepLorentzVector& p) { m_extFourMomentum = p; }
  void setExtFourMomentumA(const HepLorentzVector& p) { m_extFourMomentumA = p; }
  void setExtFourMomentumB(const HepLorentzVector& p) { m_extFourMomentumB = p; }

  // checks to see if this particle uses the same showers or tracks
  //   as another particle

  bool        duplicate(FSParticle* fsp);
  vector<int> trackId() { return m_trackId; }
  vector<int> showerId() { return m_showerId; }

  // a few utility functions

  HepLorentzVector showerFourMomentum(RecEmcShower* sh);

  double mass(string name);

private:
  EvtRecTrack*    m_track;
  RecMdcKalTrack* m_kalTrack;
  WTrackParameter m_initialWTrack;
  WTrackParameter m_vertexFitWTrack;
  int             m_trackIndex;

  EvtRecTrack* m_shower;

  EvtRecPi0*   m_pi0;
  EvtRecTrack* m_pi0Lo;
  EvtRecTrack* m_pi0Hi;

  EvtRecEtaToGG* m_eta;
  EvtRecTrack*   m_etaLo;
  EvtRecTrack*   m_etaHi;

  EvtRecVeeVertex* m_vee;
  EvtRecTrack*     m_veeTrack1;
  RecMdcKalTrack*  m_veeKalTrack1;
  WTrackParameter  m_veeInitialWTrack1;
  WTrackParameter  m_veeVertexFitWTrack1;
  EvtRecTrack*     m_veeTrack2;
  RecMdcKalTrack*  m_veeKalTrack2;
  WTrackParameter  m_veeInitialWTrack2;
  WTrackParameter  m_veeVertexFitWTrack2;
  WTrackParameter  m_veeVertexFitWTrack;
  double           m_veeLSigma;
  double           m_veeSigma;
  double           m_vee2ndChi2;
  HepPoint3D       m_veeVertex;

  string m_name;
  double m_mass;

  vector<int> m_trackId;
  vector<int> m_showerId;

  HepLorentzVector m_rawFourMomentum;
  HepLorentzVector m_rawFourMomentumA;
  HepLorentzVector m_rawFourMomentumB;
  HepLorentzVector m_intFourMomentum;
  HepLorentzVector m_intFourMomentumA;
  HepLorentzVector m_intFourMomentumB;
  HepLorentzVector m_fitFourMomentum;
  HepLorentzVector m_fitFourMomentumA;
  HepLorentzVector m_fitFourMomentumB;
  HepLorentzVector m_extFourMomentum;
  HepLorentzVector m_extFourMomentumA;
  HepLorentzVector m_extFourMomentumB;
};

//********************************************************************
//********************************************************************
//********************************************************************
//
//   The FSCut class:  defines cuts on intermediate masses
//                     and recoil masses
//
//********************************************************************
//********************************************************************
//********************************************************************

class FSCut
{

public:
  FSCut(const string& initialization);

  string FSName() { return m_FSName; }
  string submodeName() { return m_submodeName; }
  string cutType() { return m_cutType; }
  double lowCut() { return m_lowCut; }
  double highCut() { return m_highCut; }

  bool Raw() { return (m_cutType.find("Raw") != string::npos); }
  bool Int() { return (m_cutType.find("Int") != string::npos); }
  bool Fit() { return (m_cutType.find("Fit") != string::npos); }
  bool Recoil() { return (m_cutType.find("Recoil") != string::npos); }
  bool Mass() { return (m_cutType.find("Mass") != string::npos); }
  bool Squared() { return (m_cutType.find("Squared") != string::npos); }

private:
  string m_FSName;
  string m_submodeName;
  string m_cutType;
  double m_lowCut;
  double m_highCut;
};

//********************************************************************
//********************************************************************
//********************************************************************
//
//   The FSConstraint class:  defines constraints on intermediate
//                            sets of particles
//
//********************************************************************
//********************************************************************
//********************************************************************

class FSConstraint
{

public:
  FSConstraint(const string& initialization);

  string FSName() { return m_FSName; }
  string submodeName() { return m_submodeName; }
  string constraintType() { return m_constraintType; }
  double constraintValue() { return m_constraintValue; }
  string initialization() { return m_initialization; }

  bool Mass() { return (m_constraintType.find("Mass") != string::npos); }

private:
  string m_FSName;
  string m_submodeName;
  string m_constraintType;
  double m_constraintValue;
  string m_initialization;
};

#endif
