#ifndef Analysis_IniSelect_KinematicFitFailed_H
#define Analysis_IniSelect_KinematicFitFailed_H

#include "IniSelect/Exceptions/Exception.h"

/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date April 17th, 2019

/// @addtogroup BOSS_globals
/// @{

class KinematicFitFailed : public Exception
{
public:
  KinematicFitFailed(Double_t chi2) : Exception(Form("Kinematic fit failed: chi2 = %g", chi2)) {}
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

/// @}
#endif