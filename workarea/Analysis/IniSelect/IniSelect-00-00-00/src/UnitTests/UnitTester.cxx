#include "IniSelect/UnitTests/UnitTester.h"

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