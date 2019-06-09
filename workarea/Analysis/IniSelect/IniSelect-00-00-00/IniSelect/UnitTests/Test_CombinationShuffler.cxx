#include "IniSelect/UnitTests/Test_CombinationShuffler.h"
#include "IniSelect/Globals/CombinationShuffler.h"
#include <iostream>
#include <sstream>
using namespace std;
using namespace IniSelect::CombinationShuffler;

void Test_CombinationShuffler::TestInitialize()
{
  REQUIRE(NCombinations(3, 2) == 3);
  REQUIRE(NCombinations(5, 2) == 10);
  REQUIRE(NCombinations(10, 3) == 120);

  size_t n = 5, k = 2;
  vector<int> v = CreateVector(n);
  REQUIRE(VectorToString(v).compare("1 2 3 4 5 ") == 0);
  REQUIRE(Next(v, k).compare("1 3 2 4 5 ") == 0);
  REQUIRE(Next(v, k).compare("1 4 2 3 5 ") == 0);
  REQUIRE(Next(v, k).compare("1 5 2 3 4 ") == 0);
  REQUIRE(Next(v, k).compare("2 3 1 4 5 ") == 0);
  REQUIRE(Next(v, k).compare("2 4 1 3 5 ") == 0);
  REQUIRE(Next(v, k).compare("2 5 1 3 4 ") == 0);
  REQUIRE(Next(v, k).compare("3 4 1 2 5 ") == 0);
  REQUIRE(Next(v, k).compare("3 5 1 2 4 ") == 0);
  REQUIRE(Next(v, k).compare("4 5 1 2 3 ") == 0);
}

void Test_CombinationShuffler::TestFinalize()
{
  size_t n = 6, k = 3;
  vector<int> v = CreateVector(n);
  REQUIRE(VectorToString(v).compare("1 2 3 4 5 6 ") == 0);
  REQUIRE(Next(v, k).compare("1 2 4 3 5 6 ") == 0);
  REQUIRE(Next(v, k).compare("1 2 5 3 4 6 ") == 0);
  REQUIRE(Next(v, k).compare("1 2 6 3 4 5 ") == 0);
  REQUIRE(Next(v, k).compare("1 3 4 2 5 6 ") == 0);
  REQUIRE(Next(v, k).compare("1 3 5 2 4 6 ") == 0);
  REQUIRE(Next(v, k).compare("1 3 6 2 4 5 ") == 0);
  REQUIRE(Next(v, k).compare("1 4 5 2 3 6 ") == 0);
  REQUIRE(Next(v, k).compare("1 4 6 2 3 5 ") == 0);
  REQUIRE(Next(v, k).compare("1 5 6 2 3 4 ") == 0);
  REQUIRE(Next(v, k).compare("2 3 4 1 5 6 ") == 0);
  REQUIRE(Next(v, k).compare("2 3 5 1 4 6 ") == 0);
  REQUIRE(Next(v, k).compare("2 3 6 1 4 5 ") == 0);
  REQUIRE(Next(v, k).compare("2 4 5 1 3 6 ") == 0);
  REQUIRE(Next(v, k).compare("2 4 6 1 3 5 ") == 0);
  REQUIRE(Next(v, k).compare("2 5 6 1 3 4 ") == 0);
  REQUIRE(Next(v, k).compare("3 4 5 1 2 6 ") == 0);
  REQUIRE(Next(v, k).compare("3 4 6 1 2 5 ") == 0);
  REQUIRE(Next(v, k).compare("3 5 6 1 2 4 ") == 0);
  REQUIRE(Next(v, k).compare("4 5 6 1 2 3 ") == 0);
}

vector<int> Test_CombinationShuffler::CreateVector(size_t n) const
{
  vector<int> v(n);
  for(size_t i = 0; i < n; ++i) v[i] = i + 1;
  return v;
}

string Test_CombinationShuffler::VectorToString(vector<int>& v) const
{
  stringstream ss;
  for(size_t i = 0; i < v.size(); ++i) ss << v[i] << " ";
  return ss.str();
}

string Test_CombinationShuffler::Next(vector<int>& v, size_t nCombinations) const
{
  NextCombination(v, nCombinations);
  return VectorToString(v);
}

int Test_CombinationShuffler::NCombinations(size_t nElements, size_t nCombinations) const
{
  vector<int> vcount = CreateVector(nElements);
  int count = 0;
  do
  {
    ++count;
  } while(NextCombination(vcount, nCombinations));
  return count;
}