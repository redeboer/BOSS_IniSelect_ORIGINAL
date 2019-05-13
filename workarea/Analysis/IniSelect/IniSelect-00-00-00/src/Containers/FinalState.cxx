#include "IniSelect/Containers/FinalState.h"

Int_t FinalState::CountOccurences(const TString& pdtName)
{
  TString tok;
  Ssiz_t  from = 0;
  Int_t   n    = 0;
  while(fFinalState.Tokenize(tok, from))
    if(tok.EqualTo(pdtName)) ++n;
  return n;
}