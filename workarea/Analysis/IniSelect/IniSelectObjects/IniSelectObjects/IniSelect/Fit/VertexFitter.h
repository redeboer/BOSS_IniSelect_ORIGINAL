#ifndef Analysis_IniSelect_VertexFitter_H
#define Analysis_IniSelect_VertexFitter_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "VertexFit/VertexFit.h"

/// @addtogroup BOSS_objects
/// @{
class VertexFitter
{
public:
  VertexFitter();

  void Initialize();
  void AddTrack(EvtRecTrack* track, const double& mass);
  void AddCleanVertex();
  void FitAndSwim();

  const bool&     IsSuccessful() const { return fIsSuccessful; };
  WTrackParameter GetTrack(int i) const;

private:
  int  fNTracks;
  bool fIsSuccessful;

  static VertexFit* fVertexFit;

  VertexParameter BuildVertexParameter() const;
};
/// @}
#endif