#ifndef Analysis_IniSelect_UnitTests_ParticleDatabase_H
#define Analysis_IniSelect_UnitTests_ParticleDatabase_H

#include "IniSelect/UnitTests/UnitTester.h"

class Test_ParticleDatabase : public UnitTester
{
public:
  Test_ParticleDatabase(const std::string& name, ISvcLocator* pSvcLocator) :
    UnitTester(name, pSvcLocator)
  {}

  void TestInitialize(){}
  void TestExecute(){}
  void TestFinalize();
};

#endif