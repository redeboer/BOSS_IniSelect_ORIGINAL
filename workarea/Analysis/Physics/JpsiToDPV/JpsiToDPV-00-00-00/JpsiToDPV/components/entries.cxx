#include "GaudiKernel/DeclareFactoryEntries.h"
#include "JpsiToDPV/JpsiToDPV.h"
#include "JpsiToDPV/JpsiToDPV_new.h"

DECLARE_ALGORITHM_FACTORY(JpsiToDPV)
DECLARE_ALGORITHM_FACTORY(JpsiToDPV_new)

DECLARE_FACTORY_ENTRIES(JpsiToDPV)
{
  DECLARE_ALGORITHM(JpsiToDPV);
}
DECLARE_FACTORY_ENTRIES(JpsiToDPV_new)
{
  DECLARE_ALGORITHM(JpsiToDPV_new);
}
