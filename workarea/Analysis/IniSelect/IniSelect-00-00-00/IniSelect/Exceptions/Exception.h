#ifndef Analysis_IniSelect_Exception_H
#define Analysis_IniSelect_Exception_H

#include "TString.h"

/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date April 17th, 2019

/// @addtogroup BOSS_globals
/// @{

class Exception
{
public:
  Exception() {}
  Exception(const TString& message) : fMessage(message) {}
  const char* GetMessage() const { return fMessage.Data(); }
  void        SetMessage(const TString& message) { fMessage = message; }
  void        Print() const;

protected:
  TString fMessage;
};
/// @}
#endif