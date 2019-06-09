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

  REQUIRE(pi0_name.EqualTo(pi0.GetPdtName().c_str()));
  REQUIRE(pi0.GetPdgCode() == 111);
  REQUIRE(CompareFloat(pi0.GetMass(), 0.134977));
  REQUIRE(CompareFloat(pi0.GetLifetime(), 3.40282e38));
  REQUIRE(pi0.GetCharge() == 0);
  REQUIRE(pi0.GetSpin() == 0);

  REQUIRE(pip_name.EqualTo(pip.GetPdtName().c_str()));
  REQUIRE(pip.GetPdgCode() == 211);
  REQUIRE(CompareFloat(pip.GetMass(), 0.13957));
  REQUIRE(CompareFloat(pip.GetLifetime(), 780.45));
  REQUIRE(pip.GetCharge() == 1);
  REQUIRE(pip.GetSpin() == 0);

  TString Kp("K+");
  REQUIRE(Kp.EqualTo(ParticleDatabase::CodeToName(321)));
  REQUIRE(ParticleDatabase::NameToCode("D0") == 421);
}