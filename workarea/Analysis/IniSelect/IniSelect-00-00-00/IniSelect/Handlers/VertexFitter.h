#ifndef Analysis_IniSelect_VertexFitter_H
#define Analysis_IniSelect_VertexFitter_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/TrackCollections/ParticleSelection.h"
#include "VertexFit/VertexFit.h"

/// @todo This class functions more as a `namespce`, but has to be a class as it wraps the `VertexFit` singleton.
/// @addtogroup BOSS_objects
/// @{
class VertexFitter
{
public:
  static void Initialize();
  static void AddTrack(EvtRecTrack* track, const double& mass);
  static void AddTracks(ParticleSelection& selection);
  static void AddCleanVertex();
  static void FitAndSwim();

  static const bool&     IsSuccessful() { return fIsSuccessful; };
  static WTrackParameter GetTrack(int i);

private:
  VertexFitter() {}
  VertexFitter(const VertexFitter&) {}
  VertexFitter& operator=(const VertexFitter&) {}

  static int        fNTracks;
  static bool       fIsSuccessful;
  static VertexFit* fVertexFit;

  static VertexParameter BuildVertexParameter();
};
/// @}
#endif