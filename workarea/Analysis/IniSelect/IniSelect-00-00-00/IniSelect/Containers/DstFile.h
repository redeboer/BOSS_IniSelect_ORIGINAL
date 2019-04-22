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
class ChargedTrackIter;
class NeutralTrackIter;
class McTrackIter;

class DstFile
{
  friend class ChargedTrackIter;
  friend class NeutralTrackIter;
  friend class McTrackIter;

public:
  DstFile(IDataProviderSvc* dataProvider);

  void LoadNextEvent();

  int TotalChargedTracks() { return fEvtRecEvent->totalCharged(); }
  int TotalNeutralTracks() { return fEvtRecEvent->totalNeutral(); }
  int TotalTracks() { return fEvtRecEvent->totalTracks(); }

  bool IsMonteCarlo() { return fEventHeader->runNumber() < 0; }
  int  RunNumber() { return fEventHeader->runNumber(); }
  int  EventNumber() { return fEventHeader->eventNumber(); }

private:
  IDataProviderSvc*                  fDataProvider;
  SmartDataPtr<Event::EventHeader>   fEventHeader;
  SmartDataPtr<Event::McParticleCol> fMcParticleCol;
  SmartDataPtr<EvtRecEvent>          fEvtRecEvent;
  SmartDataPtr<EvtRecTrackCol>       fEvtRecTrkCol;

  CutObject fEventCounter;
  CutObject fCumulativeNTracks;
  CutObject fCumulativeNCharged;
  CutObject fCumulativeNNeutral;
  CutObject fCumulativeNMdcValid;

  void IncrementCounters();
};

template <class T>
class DstFileIter_base
{
public:
  DstFileIter_base(DstFile& file) : fFile(&file), fIsIterMode(false) {}
  virtual T* Next() = 0;

protected:
  DstFile* fFile;
  bool     fIsIterMode;
};

class ChargedTrackIter : public DstFileIter_base<EvtRecTrack>
{
public:
  ChargedTrackIter(DstFile& file);
  EvtRecTrack* Next();

private:
  std::vector<EvtRecTrack*>::iterator fIter;
  int                                 fIndex;
};

class NeutralTrackIter : public DstFileIter_base<EvtRecTrack>
{
public:
  NeutralTrackIter(DstFile& file);
  EvtRecTrack* Next();

private:
  std::vector<EvtRecTrack*>::iterator fIter;
};

class McTrackIter : public DstFileIter_base<Event::McParticle>
{
public:
  McTrackIter(DstFile& file);
  Event::McParticle* Next();

private:
  Event::McParticleCol::iterator fIter;
};
/// @}
#endif