#ifndef Analysis_IniSelect_Containers_FinalState_H
#define Analysis_IniSelect_Containers_FinalState_H

#include "TString.h"

/// @addtogroup BOSS_objects
/// @{
class FinalState
{
public:
  FinalState() {}
  FinalState(const TString& finalState) { Set(finalState); }
  void  Set(const TString& finalState) { fFinalState = finalState; }
  void  AddParticle(const TString& pdtName) { fFinalState += " " + pdtName; }
  Int_t CountOccurences(const TString& pdtName);

private:
  TString fFinalState;
};
/// @}
#endif