#ifndef Analysis_IniSelect_ParticleTrack_H
#define Analysis_IniSelect_ParticleTrack_H

#include "EmcRecEventModel/RecEmcShower.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/Particle/Particle.h"
#include "IniSelect/Particle/ParticleDatabase.h"
#include "McTruth/McParticle.h"
#include "VertexFit/WTrackParameter.h"
#include <map>
#include <string>

/// @addtogroup BOSS_objects
/// @{
class ParticleTrack : public Particle
{
public:
  ParticleTrack() {}
  ParticleTrack(const std::string& pdtName) : Particle(ParticleDatabase::GetParticle(pdtName)) {}

  EvtRecTrack*       operator=(EvtRecTrack* trk) { fEvtRecTrack = trk; }
  Event::McParticle* operator=(Event::McParticle* trk) { fMcParticle = trk; }
  RecEmcShower*      operator=(RecEmcShower* trk) { fRecEmcShower = trk; }
  WTrackParameter&   operator=(const WTrackParameter& trk) { fWTrackParameter = trk; }

private:
  EvtRecTrack*       fEvtRecTrack;
  Event::McParticle* fMcParticle;
  RecEmcShower*      fRecEmcShower;
  WTrackParameter    fWTrackParameter;
};
/// @}
#endif