#ifndef Analysis_IniSelect_TrackCollectionNew_H
#define Analysis_IniSelect_TrackCollectionNew_H

#include "EvtRecEvent/EvtRecTrack.h"
#include <string>
#include <vector>

/// Static access to the [`ParticleID`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html) class in BOSS.
/// @addtogroup BOSS_objects
/// @{
struct ParticlePair
{
  std::vector<EvtRecTrack*> neg;
  std::vector<EvtRecTrack*> pos;
  void Clear()
  {
    neg.clear();
    pos.clear();
  }
};

class TrackCollectionNew
{
public:
  std::vector<EvtRecTrack*> charged;
  std::vector<EvtRecTrack*> photon;

  ParticlePair K;
  ParticlePair pi;
  ParticlePair e;
  ParticlePair mu;
  ParticlePair p;

  void Reset()
  {
    charged.clear();
    photon.clear();
    K.Clear();
    pi.Clear();
    mu.Clear();
    e.Clear();
    p.Clear();
  }

  void PushBack(EvtRecTrack* trk);
};

/// @}
#endif
