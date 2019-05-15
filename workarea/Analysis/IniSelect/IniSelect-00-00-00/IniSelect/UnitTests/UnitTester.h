#ifndef Analysis_IniSelect_UnitTester_H
#define Analysis_IniSelect_UnitTester_H

#include "GaudiKernel/Algorithm.h"
#include "IniSelect/Containers/DstFile.h"
#include "TString.h"

#define TO_STRING(x) #x
#define REQUIRE(VARIABLE) Require(VARIABLE, __LINE__, __FILE__, TO_STRING(VARIABLE))

class UnitTester : public Algorithm
{
public:
  UnitTester(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

protected:
  virtual void TestInitialize() = 0;
  virtual void TestExecute()    = 0;
  virtual void TestEvent()      = 0;
  virtual void TestFinalize()   = 0;

  void   Require(Bool_t passed, Int_t lineNr, const char* file, const char* input);
  Bool_t CompareFloat(Float_t val1, Float_t val2, Float_t procentualDifference = 0.0001f) const;
  Bool_t CompareDouble(Double_t val1, Double_t val2,
                       Double_t procentualDifference = 0.0000001f) const;

protected:
  DstFile fInputFile;

private:
  Int_t fNTests;
  Int_t fNFailed;

  void PrintLine() const;
  void PrintTestResults() const;
  void PrintTestResultHeader() const;
};

#endif