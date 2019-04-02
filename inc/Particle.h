#ifndef BOSS_Afterburner_Particle_H
#define BOSS_Afterburner_Particle_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "TDatabasePDG.h"
	#include "TParticlePDG.h"
	#include "TString.h"



// * ======================= * //
// * ------- GLOBALS ------- * //
// * ======================= * //
/// @addtogroup BOSS_Afterburner_settings
/// @{


	inline const TDatabasePDG gPDG; ///< A `ROOT` `TDatabasePDG` object that contains all info of particles. Has to be constructed once, which is why it is a global.



/// @}
// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Container for particle data. In essence an extended container of `ROOT`'s `TParticlePDG`.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     November 16th, 2018
	/// This object describes fundamental properties of a particle. Most of this information is accessed through a reference to its equivalent `TParticlePDG` in the `TDatabasePDG`. The rest is additional information, like a LaTeX formated string that gives the sign for the particle.
	/// @remark   @b DEVELOPMENTAL
	class Particle
	{
	public:
		/// @name Constructor and destructors
			///@{
			Particle(int pdgCode);
			Particle(const char* pdgCode);
			///@}

		/// @name Information functions
			///@{
			bool IsLoaded() const;
			///@}

		/// @name Getters
			///@{
			TParticlePDG* ParticlePDG() const { return fParticlePDG; }
			int PDGCode() const;
			double Mass() const;
			const char* NameLaTeX() const;
			const char* Name() const;
			///@}

	protected:
		/// @name Data members
			///@{
			TParticlePDG* fParticlePDG; ///< A pointer to its corresponding particle in the PDG.
			TString fParticleNameLaTeX; ///< Particle name in LaTeX format.
			///@}

		/// @name Helper methods
			///@{
			void Initialize();
			TString& DetermineNameLaTeX();
			///@}

	};


/// @}
#endif