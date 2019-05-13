#ifndef Analysis_IniSelect_ParticleDatabase_H
#define Analysis_IniSelect_ParticleDatabase_H

#include "IniSelect/Exceptions/Exception.h"
#include "IniSelect/Particle/Particle.h"
#include "MdcRecoUtil/PdtEntry.h"
#include "TString.h"
#include <iostream>
#include <sstream>
#include <string>

/// Singleton access class to external database of particles.
/// Currently makes used of the [`Pdt`](http://bes3.to.infn.it/Boss/7.0.2/html/classPdt.html) BOSS class that is a searchable particle list from BaBar.
/// @todo Would be better to work with CERN ROOT's `TDatabasePDG`, but this is not included in the BOSS libraries...
/// @author Remco de Boer 雷穆克 (remco.de.boer@ihep.ac.cn or r.e.deboer@students.uu.nl)
/// @date April 18th, 2019

/// @addtogroup BOSS_objects
/// @{
class ParticleDatabase
{
public:
  static Particle GetParticle(const std::string& pdtName);
  static Particle GetParticle(const Int_t pdgCode);

  static Int_t         NameToCode(const std::string& pdtName);
  static const char* CodeToName(const Int_t pdgCode);

private:
  static const ParticleDatabase fUniqueInstance;

  ParticleDatabase();
  ParticleDatabase(const ParticleDatabase&){};
  ParticleDatabase& operator=(const ParticleDatabase&){};

  bool PdtPathExists() const;

  template <typename T>
  inline static void ThrowNoParticleException(const T& info);
};

class NoPdtTable : public Exception
{
public:
  NoPdtTable();
};

class ParticleDoesNotExist : public Exception
{
public:
  template <typename T>
  ParticleDoesNotExist(const T& info);
};
/// @}
#endif