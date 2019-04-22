#ifndef Analysis_IniSelect_Exception_H
#define Analysis_IniSelect_Exception_H

#include "TString.h"
#include <vector>

/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date April 17th, 2019

/// @addtogroup BOSS_globals
/// @{

namespace IniSelect
{
  namespace Error
  {
    class Exception
    {
    public:
      Exception(const TString& message) : fMessage(message) {}
      const char* GetMessage() const { return fMessage.Data(); }

    private:
      const TString fMessage;
    };
    class OutOfRange
    {
    public:
      OutOfRange(const TString& message, const size_t i, const size_t size) :
        fMessage(message),
        fIndex(i),
        fSize(size)
      {}
      const char* GetMessage() const
      {
        return Form("Out of range: $ul/%ul (%s)", fMessage.Data(), fIndex, fSize);
      }

    private:
      const TString fMessage;
      const size_t  fIndex;
      const size_t  fSize;
    };
  } // namespace Error
} // namespace IniSelect

/// @}
#endif