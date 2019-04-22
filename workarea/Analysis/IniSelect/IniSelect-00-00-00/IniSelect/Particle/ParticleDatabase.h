#ifndef Analysis_IniSelect_ParticleDatabase_H
#define Analysis_IniSelect_ParticleDatabase_H

#include "IniSelect/Globals/Exception.h"
#include "MdcRecoUtil/PdtEntry.h"
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
  static const ParticleDatabase* Instance();

  static PdtEntry* GetParticle(const std::string& pdtName);
  static PdtEntry* GetParticle(const int pdgCode);

private:
  static const ParticleDatabase* fUniqueInstance;

  ParticleDatabase();
  ParticleDatabase(const ParticleDatabase&){};
  ParticleDatabase& operator=(const ParticleDatabase&){};

  bool PdtPathExists() const;

  template <typename T>
  inline static void ThrowNoParticleException(const T& info);
};
/// @}

template <typename T>
inline void ParticleDatabase::ThrowNoParticleException(const T& info)
{
  std::stringstream ss;
  ss << "Particle \"" << info << "\" does not exist in database" << std::endl;
  IniSelect::Error::Exception message(ss.str());
  std::cout << message.GetMessage() << std::endl;
  throw message;
}

#endif