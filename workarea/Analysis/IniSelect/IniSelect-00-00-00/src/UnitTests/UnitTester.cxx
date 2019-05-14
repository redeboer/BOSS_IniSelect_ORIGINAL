#include "IniSelect/UnitTests/UnitTester.h"
#include "IniSelect/Globals.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace IniSelect;
using namespace std;

StatusCode UnitTester::initialize()
{
  TestInitialize();
  return StatusCode::SUCCESS;
}

StatusCode UnitTester::execute()
{
  TestExecute();
  return StatusCode::SUCCESS;
}

StatusCode UnitTester::finalize()
{
  TestFinalize();
  PrintTestResults();
  return StatusCode::SUCCESS;
}

void UnitTester::PrintTestResults() const
{
  if(fNFailed)
  {
    PrintLine();
    TerminalIO::PrintRed(Form("Failed %d of %d tests\n\n\n", fNFailed, fNTests));
  }
  else
  {
    PrintTestResultHeader();
    TerminalIO::PrintGreen(Form("Succesfully passed %d tests\n\n\n", fNTests));
  }
}

void UnitTester::PrintLine() const
{
  cerr << setw(25) << setfill('-') << "" << endl;
}

void UnitTester::PrintTestResultHeader() const
{
  stringstream ss;
  ss << setw(name().size() + 16) << setfill('=') << "";
  cout << endl << endl << endl;
  TerminalIO::PrintBold(ss.str());
  TerminalIO::PrintBold(Form("------- %s -------", name().c_str()));
  TerminalIO::PrintBold(ss.str());
  cout << endl;
}

void UnitTester::Require(Bool_t passed, Int_t lineNr, const char* file, const char* input)
{
  ++fNTests;
  if(passed) return;
  if(fNFailed == 0) PrintTestResultHeader();
  TerminalIO::PrintRed(Form("Failed test %s:%d", file, lineNr));
  TerminalIO::PrintYellow(Form("  %s", input));
  ++fNFailed;
}

Bool_t UnitTester::CompareFloat(Float_t val1, Float_t val2, Float_t procentualDifference) const
{
  return fabs((val1 - val2) / val1) < procentualDifference;
}

Bool_t UnitTester::CompareDouble(Double_t val1, Double_t val2, Double_t procentualDifference) const
{
  return fabs((val1 - val2) / val1) < procentualDifference;
}