#ifndef Analysis_IniSelect_VertexFitter_H
#define Analysis_IniSelect_VertexFitter_H

#include "EvtRecEvent/EvtRecTrack.h"
#include "IniSelect/Handler/FinalStateHandler.h"
#include "VertexFit/VertexFit.h"
#include <map>
#include <string>
#include <vector>

/// @todo This class functions more as a `namespce`, but has to be a class as it wraps the `VertexFit` singleton.
/// @addtogroup BOSS_objects
/// @{
class VertexFitter
{
public:
  static void Initialize();
  static void AddTrack(EvtRecTrack* track, const Double_t mass);
  static void AddTracks(FinalStateHandler& selection);
  static void AddCleanVertex();
  static void FitAndSwim();

  static const Bool_t    IsSuccessful() { return fIsSuccessful; };
  static WTrackParameter GetTrack(const std::string& pdtName, size_t i = 0);

  static void ThrowIfEmpty();

private:
  VertexFitter() {}
  VertexFitter(const VertexFitter&) {}
  VertexFitter& operator=(const VertexFitter&) {}

  static Int_t      fNTracks;
  static Bool_t     fIsSuccessful;
  static VertexFit* fVertexFit;

  static std::map<std::string, std::vector<Int_t> > fNameToIndex;

  static VertexParameter BuildVertexParameter();
};
/// @}

/// @addtogroup BOSS_globals
/// @{
/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date May 17th, 2019
class NoVertexFit : public Exception
{
public:
  NoVertexFit() : Exception("Empty pointer for vertex fit") {}
};
/// @}
#endif