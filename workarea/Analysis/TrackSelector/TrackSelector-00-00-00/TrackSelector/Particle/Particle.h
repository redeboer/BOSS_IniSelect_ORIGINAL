#ifndef Analysis_TrackSelector_Particle_H
#define Analysis_TrackSelector_Particle_H

#include "TString.h"
#include "TrackSelector/Particle/ParticleDatabase.h"

/// @todo Would be better to work with CERN ROOT's `TDatabasePDG`, but this is not included in the BOSS libraries...
/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date April 18th, 2019

/// @addtogroup BOSS_objects
/// @{
class Particle
{
public:
  Particle(const TString& pdtName) { Set(pdtName); }
  Particle(const Int_t pdgCode) { Set(pdgCode); }

  void Set(const TString& pdtName);
  void Set(const Int_t pdgCode);

  const TString& GetPdtName() const { return fName; }
  const Int_t&   GetPdgCode() const { return fPdgCode; }
  const Float_t& GetMass() const { return fMass; }
  const Float_t& GetLifetime() const { return fLifetime; }
  const Float_t& GetCharge() const { return fCharge; }
  const Float_t& GetSpin() const { return fSpin; }

  void Print() const;

private:
  TString fName;
  Int_t   fPdgCode;
  Float_t fMass;
  Float_t fLifetime;
  Float_t fCharge;
  Float_t fSpin;

  void Set(PdtEntry* particle);

  static const ParticleDatabase* fDatabase;
};
/// @}
#endif