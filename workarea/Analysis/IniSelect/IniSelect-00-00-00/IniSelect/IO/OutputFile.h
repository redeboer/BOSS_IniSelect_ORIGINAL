#ifndef IniSelect_IO_OutputFile_H
#define IniSelect_IO_OutputFile_H

#include "TFile.h"
#include "GaudiKernel/Algorithm.h"
#include <string>

/// @addtogroup BOSS_objects
/// @{
/// @author Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date   June 11th, 2019
class OutputFile
{
public:
  OutputFile(Algorithm* alg);

private:
  std::string fFileName;
  Algorithm*  fAlgorithm;
};
/// @}
#endif