#include "TrackSelector/Particle/Particle.h"
#include <iostream>
using namespace std;

const ParticleDatabase* fDatabase = ParticleDatabase::Instance();

void Particle::Set(const TString& pdtName)
{
  /// See names in the PDT table: `/afs/ihep.ac.cn/bes3/offline/Boss/7.0.4/InstallArea/share/pdt.table`.
  Set(fDatabase->GetParticle(pdtName.Data()));
}

void Particle::Set(const Int_t pdgCode)
{
  Set(fDatabase->GetParticle(pdgCode));
}

void Particle::Set(PdtEntry* particle)
{
  fName      = particle->name();
  fPdgCode   = particle->pdgId();
  fMass      = particle->mass();
  fLifetime  = particle->lifetime();
  fCharge    = particle->charge();
  fSpin      = particle->spin();
}

void Particle::Print() const
{
  if(fName.EqualTo("")) cout << "Particle is empty" << endl;
  cout << "Particle \"" << fName << "\"" << endl;
  cout << "  PDG code = " << fPdgCode << endl;
  cout << "  mass     = " << fMass << endl;
  cout << "  lifetime = " << fLifetime << endl;
  cout << "  charge   = " << fCharge << endl;
  cout << "  spin     = " << fSpin << endl;
}