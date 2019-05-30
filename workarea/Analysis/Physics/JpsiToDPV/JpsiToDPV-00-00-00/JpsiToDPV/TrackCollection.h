#ifndef Physics_JpsiToDPV_TrackCOllection
#define Physics_JpsiToDPV_TrackCOllection

#include "EvtRecEvent/EvtRecTrack.h"
#include <vector>

struct TrackCollection
{
  std::vector<EvtRecTrack*> charged;
  std::vector<EvtRecTrack*> photon;
  std::vector<EvtRecTrack*> Kp;
  std::vector<EvtRecTrack*> Km;
  std::vector<EvtRecTrack*> pip;
  std::vector<EvtRecTrack*> pim;
  std::vector<EvtRecTrack*> ep;
  std::vector<EvtRecTrack*> em;

  void Reset()
  {
    charged.clear();
    photon.clear();
    Kp.clear();
    Km.clear();
    pip.clear();
    pim.clear();
    ep.clear();
    em.clear();
  }
};

#endif