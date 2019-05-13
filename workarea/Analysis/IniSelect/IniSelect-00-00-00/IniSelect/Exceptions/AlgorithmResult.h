#ifndef Analysis_IniSelect_Exceptions_AlgorithmResult_H
#define Analysis_IniSelect_Exceptions_AlgorithmResult_H

#include "GaudiKernel/StatusCode.h"
#include "IniSelect/Exceptions/Exception.h"

/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date May 13th, 2019

/// @addtogroup BOSS_globals
/// @{

class AlgorithmResult : public Exception
{};

class AlgorithmFailure : public AlgorithmResult
{
public:
  AlgorithmFailure() { SetMessage("Algorithm Failure"); }
};

class AlgorithmSuccess : public AlgorithmResult
{
public:
  AlgorithmSuccess() { SetMessage("Algorithm Success"); }
};

class AlgorithmRecoverable : public AlgorithmResult
{
public:
  AlgorithmRecoverable() { SetMessage("Algorithm Recoverable"); }
};

/// @}
#endif