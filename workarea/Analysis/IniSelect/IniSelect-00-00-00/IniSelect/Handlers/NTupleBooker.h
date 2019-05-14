#ifndef Analysis_IniSelect_NTupleBooker_H
#define Analysis_IniSelect_NTupleBooker_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ClassID.h"
#include "IniSelect/NTuples/NTupleContainer.h"

/// @addtogroup BOSS_objects
/// @{
class NTupleBooker
{
public:
  NTupleBooker(SmartIF<INTupleSvc>& svc) : fNTupleSvc(svc.get()) {}
  void BookAll(const char* filename = "FILE1");
  void Book(NTupleContainer& tuple, const char* filename = "FILE1");

private:
  INTupleSvc* fNTupleSvc;
};
/// @}
#endif