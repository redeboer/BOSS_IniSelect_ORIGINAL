#ifndef Analysis_IniSelect_UnitTester_H
#define Analysis_IniSelect_UnitTester_H

#include "GaudiKernel/Algorithm.h"

class UnitTester : public Algorithm
{
public:
  UnitTester(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator) {}

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

protected:
  virtual StatusCode TestInitialize() = 0;
  virtual StatusCode TestExecute()    = 0;
  virtual StatusCode TestFinalize()   = 0;
};

#endif