#include "IniSelect/UnitTests/UnitTester.h"
#include "IniSelect/Globals.h"
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace IniSelect;

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
    TerminalIO::PrintRed(Form("Failed %d of %d tests", fNFailed, fNTests));
  }
  else
  {
    PrintTestResultHeader();
    TerminalIO::PrintGreen(Form("Succesfully passed %d tests", fNTests));
  }
  std::cout << std::endl << std::endl << std::endl << std::endl;
}

void UnitTester::PrintLine() const
{
  std::cout << std::setw(25) << std::setfill('-') << "" << std::endl;
}

void UnitTester::PrintTestResultHeader() const
{
  TerminalIO::PrintBold("\n\n\n\n==========================");
  TerminalIO::PrintBold("------ TEST RESULTS ------");
  TerminalIO::PrintBold("==========================\n");
}

void UnitTester::Test(bool passed, int lineNr, const char* input)
{
  ++fNTests;
  if(passed) return;
  if(fNFailed == 0) PrintTestResultHeader();
  TerminalIO::PrintRed(Form("Failed test line %d: %s", lineNr, input));
  ++fNFailed;
}

bool UnitTester::CompareFloat(float val1, float val2, float procentualDifference) const
{
  return fabs((val1 - val2) / val1) < procentualDifference;
}

bool UnitTester::CompareDouble(double val1, double val2, double procentualDifference) const
{
  return fabs((val1 - val2) / val1) < procentualDifference;
}