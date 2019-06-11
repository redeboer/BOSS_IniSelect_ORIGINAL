#include "IniSelect/IO/OutputFile.h"

OutputFile::OutputFile(Algorithm* alg) : fAlgorithm(alg)
{
  fAlgorithm->declareProperty("OutFile", fFileName);
}