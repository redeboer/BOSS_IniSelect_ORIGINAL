#include "GaudiKernel/DeclareFactoryEntries.h"
#include "IniSelect/Algorithms/D0omega_K4pi.h"
#include "IniSelect/UnitTests/Test_DstFile.h"

DECLARE_ALGORITHM_FACTORY(D0omega_K4pi)
DECLARE_FACTORY_ENTRIES(D0omega_K4pi)
{
  DECLARE_ALGORITHM(D0omega_K4pi);
}
DECLARE_ALGORITHM_FACTORY(Test_DstFile)
DECLARE_FACTORY_ENTRIES(Test_DstFile)
{
  DECLARE_ALGORITHM(Test_DstFile);
}