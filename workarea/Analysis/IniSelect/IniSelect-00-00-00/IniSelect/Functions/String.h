#ifndef Analysis_IniSelect_Functions_String_H
#define Analysis_IniSelect_Functions_String_H

#include "TString.h"

/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date May 12th, 2019

/// @addtogroup BOSS_functions
/// @{

namespace IniSelect
{
  namespace String
  {
    TString BaseName(TString path);
    TString NoMainDir(TString path);
    TString NoExtension(TString path);
  } // namespace String
} // namespace IniSelect

/// @}
#endif