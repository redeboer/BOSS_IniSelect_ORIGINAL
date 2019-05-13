#include "IniSelect/Particle/ParticleDatabase.h"
#include "IniSelect/Globals.h"
#include "IniSelect/Exceptions/Exception.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcRecoUtil/PdtPdg.h"
#include <iostream>
using namespace IniSelect;
using namespace std;

const ParticleDatabase ParticleDatabase::fUniqueInstance;

ParticleDatabase::ParticleDatabase()
{
  Pdt::readMCppTable(Paths::pdtTable);
  if(PdtPathExists()) return;
  NoPdtTable e;
  e.Print();
  throw e;
}

bool ParticleDatabase::PdtPathExists() const
{
  return Pdt::lookup(PdtPdg::pi0);
}

Particle ParticleDatabase::GetParticle(const string& pdtName)
{
  /// See names in the PDT table: `/afs/ihep.ac.cn/bes3/offline/Boss/7.0.4/InstallArea/share/pdt.table`.
  PdtEntry* particle = Pdt::lookup(pdtName);
  if(particle) return Particle(particle);
  ThrowNoParticleException(pdtName);
}

Particle ParticleDatabase::GetParticle(const Int_t pdgCode)
{
  PdtEntry* particle = Pdt::lookup(static_cast<PdtPdg::PdgType>(pdgCode));
  if(particle) return Particle(particle);
  ThrowNoParticleException(pdgCode);
}

Int_t ParticleDatabase::NameToCode(const string& pdtName)
{
  PdtEntry* particle = Pdt::lookup(pdtName);
  if(particle) return particle->pdgId();
  return 0;
}

const char* ParticleDatabase::CodeToName(const Int_t pdgCode)
{
  PdtEntry* particle = Pdt::lookup(static_cast<PdtPdg::PdgType>(pdgCode));
  if(particle) return particle->name();
  return "";
}

template <typename T>
inline void ParticleDatabase::ThrowNoParticleException(const T& info)
{
  ParticleDoesNotExist e(info);
  e.Print();
  throw e;
}

NoPdtTable::NoPdtTable()
{
  SetMessage(Form("PDT table path\n  \"%s\"\ndoes not exist", Paths::pdtTable));
}

template <typename T>
ParticleDoesNotExist::ParticleDoesNotExist(const T& info)
{
  stringstream ss;
  ss << "Particle \"" << info << "\" does not exist in database" << endl;
  SetMessage(ss.str());
}