#include "IniSelect/Particle/ParticleDatabase.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Globals/Exception.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcRecoUtil/PdtPdg.h"
#include "TString.h"
using namespace IniSelect;

ParticleDatabase::ParticleDatabase()
{
  Pdt::readMCppTable(Paths::pdtTable);
  if(PdtPathExists()) return;
  Error::Exception message(Form("PDT table path\n  \"%s\"\ndoes not exist", Paths::pdtTable));
  std::cout << message.GetMessage() << std::endl;
  throw message;
}

bool ParticleDatabase::PdtPathExists() const
{
  return Pdt::lookup(PdtPdg::pi0);
}

const ParticleDatabase* ParticleDatabase::fUniqueInstance = nullptr;

const ParticleDatabase* ParticleDatabase::Instance()
{
  if(!fUniqueInstance) fUniqueInstance = new ParticleDatabase();
  return fUniqueInstance;
}

Particle ParticleDatabase::GetParticle(const std::string& pdtName)
{
  /// See names in the PDT table: `/afs/ihep.ac.cn/bes3/offline/Boss/7.0.4/InstallArea/share/pdt.table`.
  PdtEntry* particle = Pdt::lookup(pdtName);
  if(!particle) ThrowNoParticleException(pdtName);
  return Particle(particle);
}

Particle ParticleDatabase::GetParticle(const int pdgCode)
{
  PdtEntry* particle = Pdt::lookup(static_cast<PdtPdg::PdgType>(pdgCode));
  if(!particle) ThrowNoParticleException(pdgCode);
  return Particle(particle);
}