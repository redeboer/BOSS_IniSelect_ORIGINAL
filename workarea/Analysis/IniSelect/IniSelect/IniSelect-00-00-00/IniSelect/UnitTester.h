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

  virtual StatusCode TestInitialize() = 0;
  virtual StatusCode TestExecute()    = 0;
  virtual StatusCode TestFinalize()   = 0;

private:
};

StatusCode UnitTester::initialize()
{
  try
  {
    TestInitialize();
  }
  catch(...)
  {}
  return StatusCode::SUCCESS;
}

StatusCode UnitTester::execute()
{
  try
  {
    TestExecute();
  }
  catch(...)
  {}
  return StatusCode::SUCCESS;
}

StatusCode UnitTester::finalize()
{
  try
  {
    TestFinalize();
  }
  catch(...)
  {}
  return StatusCode::SUCCESS;
}

#endif