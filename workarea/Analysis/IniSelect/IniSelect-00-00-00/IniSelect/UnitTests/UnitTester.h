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
  virtual void TestInitialize() = 0;
  virtual void TestExecute()    = 0;
  virtual void TestFinalize()   = 0;
};

#endif