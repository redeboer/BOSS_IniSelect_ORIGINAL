#ifndef Analysis_IniSelect_Exceptions_OutOfRange_H
#define Analysis_IniSelect_Exceptions_OutOfRange_H

#include "IniSelect/Exceptions/Exception.h"

/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date April 17th, 2019

/// @addtogroup BOSS_globals
/// @{

class OutOfRange : public Exception
{
public:
  OutOfRange(const TString& message, size_t i, size_t size) :
    Exception(message),
    fIndex(i),
    fSize(size)
  {}
  const char* GetMessage() const;

private:
  const size_t fIndex;
  const size_t fSize;
};

/// @}
#endif