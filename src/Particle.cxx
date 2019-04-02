// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "Particle.h"
	#include <iostream>



// * =========================================== * //
// * ------- CONSTRUCTORS AND DESTRUCTORS ------ * //
// * =========================================== * //


	/// Construct particle based on its code in the PDG.
	Particle::Particle(int pdgCode)
	{
		fParticlePDG = gPDG.GetParticle(pdgCode);
		Initialize();
	}


	/// Construct particle based on its code in the PDG.
	Particle::Particle(const char* particleName)
	{
		fParticlePDG = gPDG.GetParticle(particleName);
		Initialize();
	}


	/// Helper function for constructors.
	/// Sets remaining members based on `TParticlePDG` object that was set previously.
	void Particle::Initialize()
	{
		if(!fParticlePDG) {
			std::cout << "Initialize ERROR: Particle was not found in PDG database" << std::endl;
			return;
		}
		fParticleNameLaTeX = DetermineNameLaTeX();
	}



// * ======================= * //
// * ------- GETTERS ------- * //
// * ======================= * //


	/// @return double Mass of the particle according to the PDG. Returns 0 if particle was not loaded.
	double Particle::Mass() const
	{
		if(fParticlePDG) return fParticlePDG->Mass();
		else return 0.;
	}


	/// @return const char* Name of the particle in LaTeX `ROOT` format.
	const char* Particle::NameLaTeX() const
	{
		if(fParticlePDG) return fParticleNameLaTeX;
		else return nullptr;
	}


	/// @return const char* Name of the particle as registered in the `TDatabasePDG`.
	const char* Particle::Name() const
	{
		if(fParticlePDG) return fParticlePDG->GetName();
		else return nullptr;
	}


	/// @return int PDG code, and 0 if particle was not loaded.
	int Particle::PDGCode() const
	{
		if(fParticlePDG) return fParticlePDG->PdgCode();
		else return 0;
	}



// * =========================== * //
// * ------- INFORMATION ------- * //
// * =========================== * //


	bool Particle::IsLoaded() const
	{
		return fParticlePDG;
	}



// * =============================== * //
// * ------- PRIVATE METHODS ------- * //
// * =============================== * //


	/// Determines a LaTeX string in `ROOT` format from the PDG code of the particle that was loaded.
	/// See instructions here: https://root.cern.ch/doc/master/classTLatex.html.
	/// @todo Expand the switch statement here if you miss any particles. See e.g. http://home.fnal.gov/~mrenna/lutp0613man2/node44.html, or use the `TDatabasePDG` in `ROOT`.
	TString& Particle::DetermineNameLaTeX()
	{
		if(!fParticlePDG) fParticleNameLaTeX = "";
		else switch (fParticlePDG->PdgCode()) {
			// * Quarks
				case 1: fParticleNameLaTeX = "d";  break; // down quark
				case 2: fParticleNameLaTeX = "u";  break; // up quark
				case 3: fParticleNameLaTeX = "s";  break; // strange quark
				case 4: fParticleNameLaTeX = "c";  break; // charm quark
				case 5: fParticleNameLaTeX = "b";  break; // bottom quark
				case 6: fParticleNameLaTeX = "t";  break; // top quark
				case 7: fParticleNameLaTeX = "b'"; break; // bottom prime quark
				case 8: fParticleNameLaTeX = "t'"; break; // top prime quark
			// * Leptons
				case 11: fParticleNameLaTeX = "e^{-}";       break; // electron
				case 12: fParticleNameLaTeX = "#nu_{e}";     break; // electron neutrino
				case 13: fParticleNameLaTeX = "#mu^{-}";     break; // muon
				case 14: fParticleNameLaTeX = "#nu_{#mu}";   break; // muon neutrino
				case 15: fParticleNameLaTeX = "#tau^{-}";    break; // tau
				case 16: fParticleNameLaTeX = "#nu_{#tau}";  break; // tau neutrino
				case 17: fParticleNameLaTeX = "#tau'";       break; // tau prime
				case 18: fParticleNameLaTeX = "#nu'_{#tau}"; break; // tau prime neutrino
			// * Gauge boson
				case 21: fParticleNameLaTeX = "g";       break; // graviton
				case 22: fParticleNameLaTeX = "#gamma";  break; // photon
				case 23: fParticleNameLaTeX = "Z^{0}";   break; // Z boson
				case 24: fParticleNameLaTeX = "W^{+}";   break; // W boson
				case -24:fParticleNameLaTeX = "W^{-}";   break; // W boson
				case 25: fParticleNameLaTeX = "h^{0}";   break; // Higgs boson
				case 32: fParticleNameLaTeX = "Z'^{0}";  break; // Z prime boson
				case 33: fParticleNameLaTeX = "Z''^{0}"; break; // Z double prime boson
				case 34: fParticleNameLaTeX = "W'^{+}";  break; // W prime boson
				case -34:fParticleNameLaTeX = "W'^{-}";  break; // W prime boson
				case 35: fParticleNameLaTeX = "H^{0}";   break; // H0 boson
				case 36: fParticleNameLaTeX = "A^{0}";   break; // neutral A boson
				case 37: fParticleNameLaTeX = "H^{+}";   break; // H+ boson
				case -37:fParticleNameLaTeX = "H^{-}";   break; // H+ boson
				case 39: fParticleNameLaTeX = "G";       break; // graviton
				case 41: fParticleNameLaTeX = "R^{0}";   break; // neutral R boson
				case 42: fParticleNameLaTeX = "L_{Q}";   break; // LQ boson
			// * Mesons
				case 211: fParticleNameLaTeX = "#pi^{+}";     break; // positive pion
				case -211:fParticleNameLaTeX = "#pi^{-}";     break; // negative pion
				case 213: fParticleNameLaTeX = "#rho^{+}";    break; // positive rho meson
				case -213:fParticleNameLaTeX = "#rho^{-}";    break; // negative rho meson
				case 311: fParticleNameLaTeX = "K^{0}";       break; // neutral K meson
				case 313: fParticleNameLaTeX = "K^{*0}";      break; // neutral K* meson
				case 321: fParticleNameLaTeX = "K^{+}";       break; // positive kaon
				case -321:fParticleNameLaTeX = "K^{-}";       break; // negative kaon
				case 323: fParticleNameLaTeX = "K^{*+}";      break; // positive K* meson
				case -323:fParticleNameLaTeX = "K^{*-}";      break; // negative K* meson
				case 411: fParticleNameLaTeX = "D^{+}";       break; // positive D meson
				case -411:fParticleNameLaTeX = "D^{-}";       break; // negative D meson
				case 413: fParticleNameLaTeX = "D^{*+}";      break; // positive D* meson
				case -413:fParticleNameLaTeX = "D^{*-}";      break; // negative D* meson
				case 421: fParticleNameLaTeX = "D^{0}";       break; // neutral D meson
				case 423: fParticleNameLaTeX = "D^{*0}";      break; // neutral D* meson
				case 431: fParticleNameLaTeX = "D_{s}^{+}";   break; // positive D strange meson
				case -431:fParticleNameLaTeX = "D_{s}^{-}";   break; // positive D strange meson
				case 433: fParticleNameLaTeX = "D_{s}^{*+}";  break; // positive D* strange meson
				case -433:fParticleNameLaTeX = "D_{s}^{*-}";  break; // negative D* strange meson
				case 511: fParticleNameLaTeX = "B^{0}";       break; // neutral B meson
				case 513: fParticleNameLaTeX = "B^{*0}";      break; // neutral B* meson
				case 521: fParticleNameLaTeX = "B^{+}";       break; // positive B meson
				case -521:fParticleNameLaTeX = "B^{-}";       break; // positive B meson
				case 523: fParticleNameLaTeX = "B^{*+}";      break; // positive B* meson
				case -523:fParticleNameLaTeX = "B^{*-}";      break; // negative B* meson
				case 531: fParticleNameLaTeX = "B_{s}^{0}";   break; // neutral B strange meson
				case 533: fParticleNameLaTeX = "B_{s}^{*0}";  break; // neutral B* strange meson
				case 541: fParticleNameLaTeX = "B_{#c}^{+}";  break; // positive B charmed meson
				case -541:fParticleNameLaTeX = "B_{#c}^{-}";  break; // positive B charmed meson
				case 543: fParticleNameLaTeX = "B_{#c}^{*+}"; break; // positive B* charmed meson
				case -543:fParticleNameLaTeX = "B_{#c}^{*-}"; break; // negative B* charmed meson
				case 111: fParticleNameLaTeX = "#pi^{0}";     break; // neutral pion
				case 113: fParticleNameLaTeX = "#rho^{0}";    break; // neutral rho meson
				case 221: fParticleNameLaTeX = "#eta";        break; // eta meson
				case 223: fParticleNameLaTeX = "#omega";      break; // omega meson
				case 331: fParticleNameLaTeX = "#eta'";       break; // eta' meson
				case 333: fParticleNameLaTeX = "#phi";        break; // phi meson
				case 441: fParticleNameLaTeX = "#eta_{#c}";   break; // eta charm meson
				case 443: fParticleNameLaTeX = "J/#psi ";     break; // J/psi meson
				case 551: fParticleNameLaTeX = "#eta_{#b}";   break; // eta beauty meson
				case 553: fParticleNameLaTeX = "#Upsilon";    break; // upsilon meson
				case 130: fParticleNameLaTeX = "K_{L}^{0}";   break; // neutral K_L meson
				case 310: fParticleNameLaTeX = "K_{S}^{0}";   break; // neutral K_S meson
			// * Baryons
				case 1114: fParticleNameLaTeX = "#Delta^{-}";        break; // Delta-
				case 2112: fParticleNameLaTeX = "n";                 break; // neutron
				case 2114: fParticleNameLaTeX = "#Delta^{0}";        break; // Delta0
				case 2212: fParticleNameLaTeX = "p";                 break; // proton
				case 2214: fParticleNameLaTeX = "#Delta^{+}";        break; // Delta+
				case -2214:fParticleNameLaTeX = "#Delta^{-}";        break; // Delta+
				case 2224: fParticleNameLaTeX = "#Delta^{++}";       break; // Delta++
				case 3112: fParticleNameLaTeX = "#Sigma^{-}";        break; // Sigma-
				case 3114: fParticleNameLaTeX = "#Sigma^{*-}";       break; // Sigma*-
				case -3114:fParticleNameLaTeX = "#Sigma^{*+}";       break; // Sigma*+
				case 3122: fParticleNameLaTeX = "#Lambda^{0}";       break; // Lambda0
				case 3212: fParticleNameLaTeX = "#Sigma^{0}";        break; // Sigma0
				case 3214: fParticleNameLaTeX = "#Sigma^{*0}";       break; // Sigma*0
				case 3222: fParticleNameLaTeX = "#Sigma^{+}";        break; // Sigma+
				case -3222:fParticleNameLaTeX = "#Sigma^{-}";        break; // Sigma+
				case 3224: fParticleNameLaTeX = "#Sigma^{*+}";       break; // Sigma*+
				case -3224:fParticleNameLaTeX = "#Sigma^{*-}";       break; // Sigma*+
				case 3312: fParticleNameLaTeX = "#Xi^{-}";           break; // Xi-
				case 3314: fParticleNameLaTeX = "#Xi^{*-}";          break; // Xi*-
				case 3322: fParticleNameLaTeX = "#Xi^{0}";           break; // Xi0
				case 3324: fParticleNameLaTeX = "#Xi^{*0}";          break; // Xi*0
				case 3334: fParticleNameLaTeX = "#Omega^{-}";        break; // Omega-
				case 4112: fParticleNameLaTeX = "#Sigma_{#c}^{0}";   break; // Sigma_c0
				case 4114: fParticleNameLaTeX = "#Sigma_{#c}^{*0}";  break; // Sigma*_c0
				case 4122: fParticleNameLaTeX = "#Lambda_{#c}^{+}";  break; // Lambda_c+
				case -4122:fParticleNameLaTeX = "#Lambda_{#c}^{-}";  break; // Lambda_c+
				case 4212: fParticleNameLaTeX = "#Sigma_{#c}^{+}";   break; // Sigma_c+
				case -4212:fParticleNameLaTeX = "#Sigma_{#c}^{-}";   break; // Sigma_c+
				case 4214: fParticleNameLaTeX = "#Sigma_{#c}^{*+}";  break; // Sigma*_c+
				case -4214:fParticleNameLaTeX = "#Sigma_{#c}^{*-}";  break; // Sigma*_c+
				case 4222: fParticleNameLaTeX = "#Sigma_{#c}^{++}";  break; // Sigma_c++
				case 4224: fParticleNameLaTeX = "#Sigma_{#c}^{*++}"; break; // Sigma*_c++
				case 4132: fParticleNameLaTeX = "#Xi_{#c}^{0}";      break; // Xi_c0
				case 4312: fParticleNameLaTeX = "#Xi'^{0}_{#c}";     break; // Xi'_c0
				case 4314: fParticleNameLaTeX = "#Xi_{#c}^{*0}";     break; // Xi*_c0
				case 4232: fParticleNameLaTeX = "#Xi_{#c}^{+}";      break; // Xi_c+
				case -4232:fParticleNameLaTeX = "#Xi_{#c}^{-}";      break; // Xi_c+
				case 4322: fParticleNameLaTeX = "#Xi'^{+}_{#c}";     break; // Xi'_c+
				case 4324: fParticleNameLaTeX = "#Xi_{#c}^{*+}";     break; // Xi*_c+
				case -4324:fParticleNameLaTeX = "#Xi_{#c}^{*-}";     break; // Xi*_c+
				case 4332: fParticleNameLaTeX = "#Omega_{#c}^{0}";   break; // Omega_c0
				case 4334: fParticleNameLaTeX = "#Omega_{#c}^{*0}";  break; // Omega*_c0
				case 5112: fParticleNameLaTeX = "#Sigma_{#b}^{-}";   break; // Sigma_b-
				case 5114: fParticleNameLaTeX = "#Sigma_{#b}^{*-}";  break; // Sigma*_b-
				case 5122: fParticleNameLaTeX = "#Lambda_{#b}^{0}";  break; // Lambda_b0
				case 5212: fParticleNameLaTeX = "#Sigma_{#b}^{0}";   break; // Sigma_b0
				case 5214: fParticleNameLaTeX = "#Sigma_{#b}^{*0}";  break; // Sigma*_b0
				case 5222: fParticleNameLaTeX = "#Sigma_{#b}^{+}";   break; // Sigma_b+
				case -5222:fParticleNameLaTeX = "#Sigma_{#b}^{-}";   break; // Sigma_b+
				case 5224: fParticleNameLaTeX = "#Sigma_{#b}^{*+}";  break; // Sigma*_b+
				case -5224:fParticleNameLaTeX = "#Sigma_{#b}^{*-}";  break; // Sigma*_b+
		}
		return fParticleNameLaTeX;
	}