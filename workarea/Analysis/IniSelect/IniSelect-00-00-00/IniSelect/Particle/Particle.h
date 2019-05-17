#ifndef Analysis_IniSelect_Particle_H
#define Analysis_IniSelect_Particle_H

#include "MdcRecoUtil/PdtEntry.h"
#include <string>

/// @todo Would be better to work with CERN ROOT's `TDatabasePDG`, but this is not included in the BOSS libraries...
/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date April 18th, 2019

/// @addtogroup BOSS_objects
/// @{
class Particle
{
public:
  Particle() {}
  Particle(PdtEntry* entry) { Set(entry); }

  void Print() const;

  std::string& GetPdtName() const { return fName; }
  Int_t        GetPdgCode() const { return fPdgCode; }
  Float_t      GetMass() const { return fMass; }
  Float_t      GetLifetime() const { return fLifetime; }
  Float_t      GetCharge() const { return fCharge; }
  Float_t      GetSpin() const { return fSpin; }

private:
  std::string fName;
  Int_t       fPdgCode;
  Float_t     fMass;
  Float_t     fLifetime;
  Float_t     fCharge;
  Float_t     fSpin;

  void Set(PdtEntry* particle);
};
/// @}
#endif