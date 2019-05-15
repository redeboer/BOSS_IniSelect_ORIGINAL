#include "GaudiKernel/DeclareFactoryEntries.h"
#include "IniSelect/Algorithms/D0omega_K4pi.h"
#include "IniSelect/UnitTests/Test_CandidateTracks.h"
#include "IniSelect/UnitTests/Test_CandidateSelection.h"
#include "IniSelect/UnitTests/Test_CombinationShuffler.h"
#include "IniSelect/UnitTests/Test_DstFile.h"
#include "IniSelect/UnitTests/Test_ParticleDatabase.h"

DECLARE_ALGORITHM_FACTORY(D0omega_K4pi)
DECLARE_FACTORY_ENTRIES(D0omega_K4pi)
{
  DECLARE_ALGORITHM(D0omega_K4pi);
}

DECLARE_ALGORITHM_FACTORY(Test_CandidateTracks)
DECLARE_FACTORY_ENTRIES(Test_CandidateTracks)
{
  DECLARE_ALGORITHM(Test_CandidateTracks);
}

DECLARE_ALGORITHM_FACTORY(Test_CandidateSelection)
DECLARE_FACTORY_ENTRIES(Test_CandidateSelection)
{
  DECLARE_ALGORITHM(Test_CandidateSelection);
}

DECLARE_ALGORITHM_FACTORY(Test_CombinationShuffler)
DECLARE_FACTORY_ENTRIES(Test_CombinationShuffler)
{
  DECLARE_ALGORITHM(Test_CombinationShuffler);
}

DECLARE_ALGORITHM_FACTORY(Test_DstFile)
DECLARE_FACTORY_ENTRIES(Test_DstFile)
{
  DECLARE_ALGORITHM(Test_DstFile);
}

DECLARE_ALGORITHM_FACTORY(Test_ParticleDatabase)
DECLARE_FACTORY_ENTRIES(Test_ParticleDatabase)
{
  DECLARE_ALGORITHM(Test_ParticleDatabase);
}