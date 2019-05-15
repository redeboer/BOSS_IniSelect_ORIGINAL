#ifndef Analysis_IniSelect_UnitTests_CombinationShuffler_H
#define Analysis_IniSelect_UnitTests_CombinationShuffler_H

#include "IniSelect/UnitTests/UnitTester.h"
#include <string>
#include <vector>

class Test_CombinationShuffler : public UnitTester
{
public:
  Test_CombinationShuffler(const std::string& name, ISvcLocator* pSvcLocator) :
    UnitTester(name, pSvcLocator)
  {}

  void TestInitialize();
  void TestExecute() {}
  void TestEvent() {}
  void TestFinalize();

private:
  std::vector<int> CreateVector(size_t n) const;
  std::string      VectorToString(vector<int>& v) const;
  std::string      Next(vector<int>& v, size_t nCombinations) const;
  int              NCombinations(size_t nElements, size_t nCombinations) const;
};

#endif