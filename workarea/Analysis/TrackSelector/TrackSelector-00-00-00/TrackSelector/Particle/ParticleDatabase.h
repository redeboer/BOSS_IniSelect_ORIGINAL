#ifndef Analysis_TrackSelector_ParticleDatabase_H
#define Analysis_TrackSelector_ParticleDatabase_H

#include "MdcRecoUtil/PdtEntry.h"
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

  static PdtEntry* GetParticle(const std::string& name);
  static PdtEntry* GetParticle(const Int_t name);

private:
  static const ParticleDatabase* fUniqueInstance;

  ParticleDatabase();
  ParticleDatabase(const ParticleDatabase&){};
  ParticleDatabase& operator=(const ParticleDatabase&){};

  bool        PdtPathExists() const;
  static void ThrowNoParticleException();
};
/// @}
#endif