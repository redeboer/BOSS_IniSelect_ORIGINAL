#ifndef BOSS_Afterburner_ReconstructedParticle_new_H
#define BOSS_Afterburner_ReconstructedParticle_new_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "Particle.h"
	#include "FitPars.h"
	#include "TString.h"
	#include <utility>



// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_Afterburner_objects
/// @{


	/// Extended data container of a reconstructed particle. In essence an container of `ROOT`'s `TParticlePDG`, but with extended functionality for particle reconstruction. It contains additional parameters that are relevant when reconstructing the particle, like fit range in the invariant mass plot. The idea is that you can use this object to generalise functions like fitting, where the mass range is particle-specific. It is then convenient to only have to feed the function this object.
	/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
	/// @date     October 25th, 2018
	class ReconstructedParticle_new : public Particle
	{
	public:
		/// @name Constructors
			///@{
			ReconstructedParticle_new(const int pdg_code, const TString &decay_chain);
			ReconstructedParticle_new(const int pdg_name, const TString &decay_chain);
			///@}

		/// @name Getters
			///@{
			const TString& DecayChannel() const { return fDecayChannel; }
			const FitPars* FitParameters() const { return fFitParameters; }
			///@}

	protected:
		/// @name Data members
			///@{
			TString fDecayID; ///< **Unique** indication of the decay chain, following names in the PDG table.
			TString fDecayChannel; ///< LaTeX formatted string for the decay chain.
			FitPars* fFitParameters;
			///@}

		/// @name Helper methods
			///@{
			void DetermineReconstructionParameters();
			void Initialize(const TString &decay_chain);
			///@}

	};


/// @}
#endif