#ifndef FSFILTER_H
#define FSFILTER_H

using namespace std;

#include <map>
#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "FSFilter/FSHelper.h"
#include "FSFilter/MCTruthHelper.h"
#include "FSFilter/NTupleHelper.h"

class MCTruthHelper;
class NTupleHelper;

//********************************************************************
//********************************************************************
//********************************************************************
//
//   The FSFilter class:  the main algorithm
//
//********************************************************************
//********************************************************************
//********************************************************************

class FSFilter : public Algorithm
{

public:
  FSFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  // check the energy and momentum balance for a combination
  //  of final state particles

  bool checkCombination(const vector<FSParticle*>& combo, bool complete, bool inclusive);

private:
  // keep track of the previous run number

  int m_runNumberOld;

  // input parameters from the job options file

  int              m_maxShowers;
  int              m_maxExtraTracks;
  double           m_cmEnergyParameter;
  double           m_cmEnergy;
  double           m_crossingAngle;
  double           m_energyTolerance;
  double           m_momentumTolerance;
  double           m_maxChi2DOF;
  bool             m_trackStudies;
  bool             m_extraKaonPID;
  bool             m_pidStudies;
  bool             m_neutralStudies;
  bool             m_pi0Studies;
  bool             m_etaStudies;
  bool             m_veeStudies;
  bool             m_bypassVertexDB;
  bool             m_yuping;
  bool             m_writeNTGen;
  bool             m_writeNTGenCode;
  bool             m_printTruthInformation;
  int              m_isolateRunLow;
  int              m_isolateRunHigh;
  int              m_isolateEventLow;
  int              m_isolateEventHigh;
  bool             m_fastJpsi;
  bool             m_debug;
  static const int MAXFSNAMES       = 10000;
  static const int MAXFSCUTS        = 10000;
  static const int MAXFSCONSTRAINTS = 10000;
  static const int MAXFSMMFITS      = 10000;
  string           m_FSNames[MAXFSNAMES];
  string           m_FSCuts[MAXFSCUTS];
  string           m_FSConstraints[MAXFSCONSTRAINTS];
  string           m_FSMMFits[MAXFSMMFITS];

  // vector of all final states to be analyzed

  vector<FSInfo*> m_FSInfoVector;

  // save time by only reconstructing particles
  //  if they appear in m_FSInfoVector

  bool m_findE;
  bool m_findMu;
  bool m_findPi;
  bool m_findK;
  bool m_findP;
  bool m_findGamma;
  bool m_findPi0;
  bool m_findEta;
  bool m_findKs;
  bool m_findLambda;

  // MCTruthHelper object

  MCTruthHelper* m_mcTruthHelper;

  // tree for generator level information

  NTupleHelper* m_ntGEN;

  // event counter

  int m_eventCounter;

  // initial four-momentum

  HepLorentzVector m_pInitial;
};

#endif
