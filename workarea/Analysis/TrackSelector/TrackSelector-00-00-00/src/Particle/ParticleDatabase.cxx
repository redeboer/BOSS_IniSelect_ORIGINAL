#include "TrackSelector/Particle/ParticleDatabase.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcRecoUtil/PdtPdg.h"
#include "TrackSelector/TSGlobals.h"
#include "TrackSelector/TSGlobals/TSException.h"
using namespace TSGlobals;

ParticleDatabase::ParticleDatabase()
{
  Pdt::readMCppTable(Paths::pdtTable);
}

const ParticleDatabase* ParticleDatabase::fUniqueInstance = nullptr;

const ParticleDatabase* ParticleDatabase::Instance()
{
  if(!fUniqueInstance) fUniqueInstance = new ParticleDatabase();
}

PdtEntry* ParticleDatabase::GetParticle(const std::string& name)
{
  PdtEntry* particle = Pdt::lookup(name);
  if(particle) return particle;
  ThrowNoParticleException();
}

PdtEntry* ParticleDatabase::GetParticle(const Int_t pdgCode)
{
  PdtEntry* particle = Pdt::lookup(static_cast<PdtPdg::PdgType>(pdgCode));
  if(particle) return particle;
  ThrowNoParticleException();
}

void ParticleDatabase::ThrowNoParticleException()
{
  throw Error::Exception("Particle \"%s\" does not exist in database");
}
