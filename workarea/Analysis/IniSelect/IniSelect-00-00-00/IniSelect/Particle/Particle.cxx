#include "IniSelect/Particle/Particle.h"
#include <iostream>
using namespace std;

void Particle::Set(PdtEntry* particle)
{
  fName     = particle->name();
  fPdgCode  = particle->pdgId();
  fMass     = particle->mass();
  fLifetime = particle->lifetime();
  fCharge   = particle->charge();
  fSpin     = particle->spin();
}

void Particle::Print() const
{
  if(fName.compare("") == 0) cout << "Particle is empty" << endl;
  cout << "Particle \"" << fName << "\"" << endl;
  cout << "  PDG code = " << fPdgCode << endl;
  cout << "  mass     = " << fMass << endl;
  cout << "  lifetime = " << fLifetime << endl;
  cout << "  charge   = " << fCharge << endl;
  cout << "  spin     = " << fSpin << endl;
}