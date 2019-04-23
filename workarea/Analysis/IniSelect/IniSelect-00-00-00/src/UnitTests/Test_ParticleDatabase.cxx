#include "IniSelect/UnitTests/Test_ParticleDatabase.h"
#include "IniSelect/Particle/Particle.h"
#include "IniSelect/Particle/ParticleDatabase.h"
#include "TString.h"

void Test_ParticleDatabase::TestFinalize()
{
  Particle pi0 = ParticleDatabase::GetParticle(111);
  Particle pip = ParticleDatabase::GetParticle("pi+");

  TString pi0_name("pi0");
  TString pip_name("pi+");

  TEST_CASE(pi0_name.EqualTo(pi0.GetPdtName()));
  TEST_CASE(pi0.GetPdgCode() == 111);
  TEST_CASE(CompareFloat(pi0.GetMass(), 0.134977));
  TEST_CASE(CompareFloat(pi0.GetLifetime(), 3.40282e38));
  TEST_CASE(pi0.GetCharge() == 0);
  TEST_CASE(pi0.GetSpin() == 0);

  TEST_CASE(pip_name.EqualTo(pip.GetPdtName()));
  TEST_CASE(pip.GetPdgCode() == 211);
  TEST_CASE(CompareFloat(pip.GetMass(), 0.13957));
  TEST_CASE(CompareFloat(pip.GetLifetime(), 780.45));
  TEST_CASE(pip.GetCharge() == 1);
  TEST_CASE(pip.GetSpin() == 0);

  TString Kp("K+");
  TEST_CASE(Kp.EqualTo(ParticleDatabase::CodeToName(321)));
  TEST_CASE(ParticleDatabase::NameToCode("D0") == 421);
}