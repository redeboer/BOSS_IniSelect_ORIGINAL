#include "IniSelect/IO/DstFile.h"
#include "IniSelect/Exceptions/Exception.h"

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
  fEventHeader   = SmartDataPtr<Event::EventHeader>(fDataProvider, gHeaderName);
  fEvtRecEvent   = SmartDataPtr<EvtRecEvent>(fDataProvider, EventModel::EvtRec::EvtRecEvent);
  fEvtRecTrkCol  = SmartDataPtr<EvtRecTrackCol>(fDataProvider, EventModel::EvtRec::EvtRecTrackCol);
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

McTrackIter::McTrackIter(DstFile& file) : DstFileIter_base(file)
{
  fIter = fFile->fMcParticleCol->begin();
}

ChargedTrackIter::ChargedTrackIter(DstFile& file) : DstFileIter_base(file)
{
  fIter = fFile->fEvtRecTrkCol->begin();
}

NeutralTrackIter::NeutralTrackIter(DstFile& file) : DstFileIter_base(file)
{
  fIter = fFile->fEvtRecTrkCol->begin() + fFile->TotalChargedTracks();
}

Event::McParticle* McTrackIter::Next()
{
  if(fIndex >= fFile->TotalMcTracks()) return nullptr;
  if(fIter == fFile->fMcParticleCol->end()) return nullptr;
  if(fIndex) ++fIter;
  ++fIndex;
  if(!(*fIter)) Next();
  return *fIter;
}

EvtRecTrack* ChargedTrackIter::Next()
{
  if(fIndex >= fFile->TotalChargedTracks()) return nullptr;
  if(fIter == fFile->fEvtRecTrkCol->end()) return nullptr;
  if(fIndex) ++fIter;
  ++fIndex;
  if(!(*fIter)) Next();
  if((*fIter)->isMdcTrackValid()) ++fFile->fCumulativeNMdcValid;
  return *fIter;
}

EvtRecTrack* NeutralTrackIter::Next()
{
  if(fIndex >= fFile->TotalNeutralTracks()) return nullptr;
  if(fIter == fFile->fEvtRecTrkCol->end()) return nullptr;
  if(fIndex) ++fIter;
  ++fIndex;
  if(!(*fIter)) Next();
  return *fIter;
}