#include "IniSelect/Particle/ParticleDatabase.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Globals/Exception.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcRecoUtil/PdtPdg.h"
#include "TString.h"
using namespace IniSelect;

const ParticleDatabase ParticleDatabase::fUniqueInstance;

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

Particle ParticleDatabase::GetParticle(const std::string& pdtName)
{
  /// See names in the PDT table: `/afs/ihep.ac.cn/bes3/offline/Boss/7.0.4/InstallArea/share/pdt.table`.
  PdtEntry* particle = Pdt::lookup(pdtName);
  if(particle) return Particle(particle);
  ThrowNoParticleException(pdtName);
}

Particle ParticleDatabase::GetParticle(const int pdgCode)
{
  PdtEntry* particle = Pdt::lookup(static_cast<PdtPdg::PdgType>(pdgCode));
  if(particle) return Particle(particle);
  ThrowNoParticleException(pdgCode);
}

template <typename T>
inline void ParticleDatabase::ThrowNoParticleException(const T& info)
{
  std::stringstream ss;
  ss << "Particle \"" << info << "\" does not exist in database" << std::endl;
  IniSelect::Error::Exception message(ss.str());
  std::cout << message.GetMessage() << std::endl;
  throw message;
}