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

  const std::string& GetPdtName() const { return fName; }

  int   GetPdgCode() const { return fPdgCode; }
  float GetMass() const { return fMass; }
  float GetLifetime() const { return fLifetime; }
  float GetCharge() const { return fCharge; }
  float GetSpin() const { return fSpin; }

private:
  std::string fName;
  int         fPdgCode;
  float       fMass;
  float       fLifetime;
  float       fCharge;
  float       fSpin;

  void Set(PdtEntry* particle);
};
/// @}
#endif