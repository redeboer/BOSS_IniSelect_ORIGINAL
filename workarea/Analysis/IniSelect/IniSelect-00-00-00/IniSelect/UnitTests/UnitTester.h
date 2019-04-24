#ifndef Analysis_IniSelect_UnitTester_H
#define Analysis_IniSelect_UnitTester_H

#include "GaudiKernel/Algorithm.h"

#define TO_STRING(x) #x
#define REQUIRE(VARIABLE) Require(VARIABLE, __LINE__, __FILE__, TO_STRING(VARIABLE))

class UnitTester : public Algorithm
{
public:
  UnitTester(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator), fNTests(0), fNFailed(0) {}

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

protected:
  virtual void TestInitialize() = 0;
  virtual void TestExecute()    = 0;
  virtual void TestFinalize()   = 0;

  void Require(bool passed, int lineNr, const char* file, const char* input);
  bool CompareFloat(float val1, float val2, float procentualDifference = 0.0001f) const;
  bool CompareDouble(double val1, double val2, double procentualDifference = 0.0000001f) const;

private:
  int fNTests;
  int fNFailed;

  void PrintLine() const;
  void PrintTestResults() const;
  void PrintTestResultHeader() const;
};

#endif