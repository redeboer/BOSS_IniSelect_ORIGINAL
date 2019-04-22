#include "IniSelect/Containers/DstFile.h"
#include "IniSelect/Globals/Exception.h"
using namespace IniSelect::Error;

#include <iostream>

const char* gHeaderName = "/Event/EventHeader";
const char* gMcColName  = "/Event/MC/McParticleCol";

DstFile::DstFile(IDataProviderSvc* dataProvider) :
  fDataProvider(dataProvider),
  fEventHeader(fDataProvider, gHeaderName),
  fMcParticleCol(fDataProvider, gMcColName),
  fEvtRecEvent(fDataProvider, EventModel::EvtRec::EvtRecEvent),
  fEvtRecTrkCol(fDataProvider, EventModel::EvtRec::EvtRecTrackCol),
  fEventCounter("N_events"),
  fCumulativeNTracks("N_tracks"),
  fCumulativeNCharged("N_charged"),
  fCumulativeNNeutral("N_neutral"),
  fCumulativeNMdcValid("N_MDCvalid")
{
  if(!dataProvider)
    throw Exception("FATAL ERROR: could not load data provider service (Algorithm::eventSvc)");
}

void DstFile::LoadNextEvent()
{
  /// Load headers, event, and track collection from the DST input file.
  /// For more info see:
  /// * [Namespace `EventModel`](http://bes3.to.infn.it/Boss/7.0.2/html/namespaceEventModel_1_1EvtRec.html)
  /// * [Class `EvtRecEvent`](http://bes3.to.infn.it/Boss/7.0.2/html/classEvtRecEvent.html)
  /// * [Type definition `EvtRecTrackCol`](http://bes3.to.infn.it/Boss/7.0.2/html/EvtRecTrack_8h.html)
  /// * [Type definition`Event::McParticleCol`](http://bes3.to.infn.it/Boss/7.0.0/html/namespaceEvent.html#b6a28637c54f890ed93d8fd13d5021ed)
  fEventHeader  = SmartDataPtr<Event::EventHeader>(fDataProvider, gHeaderName);
  fEvtRecEvent  = SmartDataPtr<EvtRecEvent>(fDataProvider, EventModel::EvtRec::EvtRecEvent);
  fEvtRecTrkCol = SmartDataPtr<EvtRecTrackCol>(fDataProvider, EventModel::EvtRec::EvtRecTrackCol);
  fMcParticleCol = SmartDataPtr<Event::McParticleCol>(fDataProvider, gMcColName);
  IncrementCounters();
}

void DstFile::IncrementCounters()
{
  ++fEventCounter;
  fCumulativeNTracks += TotalChargedTracks();
  fCumulativeNCharged += TotalNeutralTracks();
  fCumulativeNNeutral += TotalTracks();
}

Event::McParticle* DstFile::FirstMcTrack()
{
  fMcIter = fMcParticleCol->begin();
  if(fMcIter == fMcParticleCol->end()) return nullptr;
  return *fMcIter;
}

EvtRecTrack* DstFile::FirstChargedTrack()
{
  fChargedIter = fEvtRecTrkCol->begin();
  if(fChargedIter == fEvtRecTrkCol->end()) return nullptr;
  return *fChargedIter;
}

EvtRecTrack* DstFile::FirstNeutralTrack()
{
  fNeutralIter = fEvtRecTrkCol->begin() + TotalChargedTracks();
  if(fNeutralIter == fEvtRecTrkCol->end()) return nullptr;
  return *fNeutralIter;
}

Event::McParticle* DstFile::NextMcTrack()
{
  ++fMcIter;
  if(fMcIter == fMcParticleCol->end()) return nullptr;
  return *fMcIter;
}

EvtRecTrack* DstFile::NextChargedTrack()
{
  if(fChargedIter == fEvtRecTrkCol->end()) return nullptr;
  ++fChargedIter;
  ++fIndex;
  if(fIndex >= TotalChargedTracks()) return nullptr;
  if((*fChargedIter)->isMdcTrackValid())
    ++fCumulativeNMdcValid;
  else
    NextChargedTrack();
  return *fChargedIter;
}

EvtRecTrack* DstFile::NextNeutralTrack()
{
std::cout << "NextNeutralTrack" << std::endl;
  if(fNeutralIter == fEvtRecTrkCol->end()) return nullptr;
  ++fNeutralIter;
  if(!(*fNeutralIter)->emcShower()) NextNeutralTrack();
  if(!(*fNeutralIter)->isEmcShowerValid()) NextNeutralTrack();
  return *fNeutralIter;
}