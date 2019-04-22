#include "IniSelect/UnitTests/Test_ParticleDatabase.h"
#include "IniSelect/Particle/Particle.h"
#include "IniSelect/Particle/ParticleDatabase.h"
#include <stdio.h>

Test_ParticleDatabase::Test_ParticleDatabase(const std::string& name, ISvcLocator* pSvcLocator) :
  UnitTester(name, pSvcLocator)
{}

void Test_ParticleDatabase::TestInitialize()
{}

void Test_ParticleDatabase::TestExecute()
{}

void Test_ParticleDatabase::TestFinalize()
{
  Particle pip = ParticleDatabase::GetParticle("pi+");
  Particle pi0 = ParticleDatabase::GetParticle("pi0");
  pip.Print();
  pi0.Print();
}