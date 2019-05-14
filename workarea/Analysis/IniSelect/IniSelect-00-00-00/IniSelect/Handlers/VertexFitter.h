#ifndef Analysis_IniSelect_VertexFitter_H
#define Analysis_IniSelect_VertexFitter_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/TrackCollections/CandidateSelection.h"
#include "VertexFit/VertexFit.h"

/// @todo This class functions more as a `namespce`, but has to be a class as it wraps the `VertexFit` singleton.
/// @addtogroup BOSS_objects
/// @{
class VertexFitter
{
public:
  static void Initialize();
  static void AddTrack(EvtRecTrack* track, const Double_t mass);
  static void AddTracks(CandidateSelection& selection);
  static void AddCleanVertex();
  static void FitAndSwim();

  static const Bool_t    IsSuccessful() { return fIsSuccessful; };
  static WTrackParameter GetTrack(Int_t i);

private:
  VertexFitter() {}
  VertexFitter(const VertexFitter&) {}
  VertexFitter& operator=(const VertexFitter&) {}

  static Int_t      fNTracks;
  static Bool_t     fIsSuccessful;
  static VertexFit* fVertexFit;

  static VertexParameter BuildVertexParameter();
};
/// @}
#endif