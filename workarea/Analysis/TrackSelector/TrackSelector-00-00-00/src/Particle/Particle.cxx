#include "TrackSelector/Particle/Particle.h"

const ParticleDatabase* fDatabase = ParticleDatabase::Instance();

void Particle::Set(const TString& name)
{
  Set(fDatabase->GetParticle(name.Data()));
}

void Particle::Set(const Int_t pdgCode)
{
  Set(fDatabase->GetParticle(pdgCode));
}

void Particle::Set(PdtEntry* particle)
{
  fName     = particle->name();
  fPdgCode  = particle->pdgId();
  fMass     = particle->mass();
  fLifetime = particle->lifetime();
  fCharge   = particle->charge();
  fSpin     = particle->spin();
}