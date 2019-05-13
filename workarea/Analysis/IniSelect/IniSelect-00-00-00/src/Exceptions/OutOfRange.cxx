#include "IniSelect/Exceptions/OutOfRange.h"
#include <iostream>
using namespace std;

const char* OutOfRange::GetMessage() const
{
  return Form("Out of range: $ul/%ul (%s)", fMessage.Data(), fIndex, fSize);
}