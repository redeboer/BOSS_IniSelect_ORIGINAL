#ifndef NTUPLEHELPER_H
#define NTUPLEHELPER_H

using namespace std;

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "MucRecEvent/RecMucTrack.h"
#include "ParticleID/ParticleID.h"
#include <map>
#include <string>

#include "FSFilter/FSHelper.h"
#include "FSFilter/MCTruthHelper.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "GaudiKernel/NTuple.h"

class FSParticle;
class FSInfo;
class MCTruthHelper;

// ***************************************************************
//
//  CLASS NTupleHelper
//
//    a utility to keep track of and fill trees
//
// ***************************************************************

class NTupleHelper
{

public:
  // constructor takes a Tuple object created by BOSS

  NTupleHelper(NTuple::Tuple* Tree);

  // generic routine to fill a tree variable;
  //   if the variable doesn't yet exist, it is created

  void fillDouble(string name, double value);

  // fill specific information

  void fill4Momentum(int index, TString subindex, string tag, const HepLorentzVector& p);
  void fill4Momentum(int index, TString subindex, string tag, const RecEmcShower& p);
  void fill4Momentum(int index, TString subindex, string tag);
  void fillShower(int index, TString subindex, string tag, EvtRecTrack* tk,
                  vector<FSParticle*> pi0s, SmartDataPtr<EvtRecTrackCol> evtRecTrackCol);
  void fillTrack(int index, TString subindex, string tag, EvtRecTrack* p, VertexParameter beamSpot,
                 int trackIndex, bool pidStudies = false);
  void fillPi0(int index, TString subindex, string tag, EvtRecPi0* p);
  void fillEta(int index, TString subindex, string tag, EvtRecEtaToGG* p);
  void fillVee(int index, TString subindex, string tag, FSParticle* p);
  void fillEvent(SmartDataPtr<Event::EventHeader> eventHeader,
                 SmartDataPtr<EvtRecEvent> evtRecEvent, double cmEnergy);
  void fillVertices(const VertexParameter& beamSpot, const VertexParameter& primaryVertex);
  void fillMCTruth(MCTruthHelper* mc, FSInfo* fs = NULL, string tag = "MC");
  void fillJPsiFinder(SmartDataPtr<EvtRecTrackCol> evtRecTrackCol);

  // write the tree

  void write();

private:
  bool containsEntry(string name);

  NTuple::Tuple* m_Tree;

  map<string, NTuple::Item<double> > m_ntupleDoubleMap;

  map<string, double> m_doubleMap;

  bool m_bookingStage;

  string concatName(string prefix, string base, int index, TString subindex);
};

#endif
