#ifndef Analysis_IniSelect_DstFile_H
#define Analysis_IniSelect_DstFile_H

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "IniSelect/Containers/CutObject.h"
#include "McTruth/McParticle.h"
#include <vector>

/// @addtogroup BOSS_objects
/// @{

/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date   April 21st, 2019
class DstFile
{
public:
  DstFile(IDataProviderSvc* dataProvider);

  void LoadHeaders();
  bool LoadMcCollection();

  int TotalChargedTracks() { return fEvtRecEvent->totalCharged(); }
  int TotalNeutralTracks() { return fEvtRecEvent->totalNeutral(); }
  int TotalTracks() { return fEvtRecEvent->totalTracks(); }

  void IncrementCounters();

  bool IsMonteCarlo() { return fEventHeader->runNumber() < 0; }
  int  RunNumber() { fEventHeader->runNumber(); }
  int  EventNumber() { fEventHeader->eventNumber(); }

  Event::McParticle* FirstMcTrack();
  EvtRecTrack*       FirstChargedTrack();
  EvtRecTrack*       FirstNeutralTrack();
  Event::McParticle* NextMcTrack();
  EvtRecTrack*       NextChargedTrack();
  EvtRecTrack*       NextNeutralTrack();

private:
  IDataProviderSvc*                  fDataProvider;
  SmartDataPtr<Event::EventHeader>   fEventHeader;
  SmartDataPtr<Event::McParticleCol> fMcParticleCol;
  SmartDataPtr<EvtRecEvent>          fEvtRecEvent;
  SmartDataPtr<EvtRecTrackCol>       fEvtRecTrkCol;

  Event::McParticleCol::iterator      fMcIter;
  std::vector<EvtRecTrack*>::iterator fChargedIter;
  std::vector<EvtRecTrack*>::iterator fNeutralIter;

  int fIndex;

  CutObject fEventCounter;
  CutObject fCumulativeNTracks;
  CutObject fCumulativeNCharged;
  CutObject fCumulativeNNeutral;
  CutObject fCumulativeNMdcValid;
};
/// @}
#endif