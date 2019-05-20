#ifndef Physics_Analysis_D0omega_K4pi_H
#define Physics_Analysis_D0omega_K4pi_H

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_packages
/// @{

/// Commented version of the `D0omega_K4pi` example BOSS package (version `00-00-23`). The `D0omega_K4pi`
/// example package particularly teaches the use of the Kalman kinematic fit procedure.
/// @remark   In addition to the more extensive comments, some additional parameters, such as ToF
/// without particle hypothesis, have been added.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 18th, 2018
class D0omega_K4pi : public Algorithm
{
public:
  /// @name Constructors
  ///@{
  D0omega_K4pi(const std::string& name, ISvcLocator* pSvcLocator);
  ///@}

  /// @name Gaudi Algorithm steps
  ///@{
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  ///@}

private:
  // ! ================================= ! //
  // ! ------- DECLARE CUTS HERE ------- ! //
  // ! ================================= ! //
  /// @name Cut parameters. Here, you can define data members that you use to define cuts. The values for these cuts should be set in the D0omega_K4pi::D0omega_K4pi constructor (see cxx file).
  ///@{
  // * Declare r0, z0 cut for charged tracks
  double fVr0cut;
  double fVz0cut;
  double fRvz0cut;
  double fRvxy0cut;

  // * Declare energy, dphi, dthe cuts for fake gamma's
  double fEnergyThreshold;
  double fGammaPhiCut;
  double fGammaThetaCut;
  double fGammaAngleCut;

  // * Declare whether to test the success of the 4- and 5-constraint fits
  bool   fDo_fit4c;
  bool   fDo_fit5c;
  double fMaxChiSq;
  double fMinPID;

  // * Declare whether or not to check success of Particle Identification
  bool fCheckMC;
  bool fCheckVertex;
  bool fCheckPhoton;
  bool fCheckDedx;
  bool fCheckTof;
  bool fCheckPID;
  bool fCheckEtot;

  ///@}

  // ! ==================================== ! //
  // ! ------- DECLARE NTUPLES HERE ------- ! //
  // ! ==================================== ! //
  /// @name NTuples (eventual TTrees and their branches). NTuples are like vectors, but its members do not necessarily have to be of the same type. In this package, the NTuples are used to store event-by-event information. Its values are then written to the output ROOT file, creating a ROOT TTree. In that sense, each NTuple here represents one TTree within that output ROOT file, and each NTuple::Item represents its leaves. The name of the leaves is determined when calling NTuple::addItem. Note that the NTuple::Items are added to the NTuple during the D0omega_K4pi::initialize() step. This is also the place where you name these variables, so make sure that the structure here is reflected there!
  ///@{
  NTuple::Tuple*       fTupleVxyz; ///< Vertex information of the charged tracks
  NTuple::Item<double> fVx0;       ///< Primary \f$x\f$-vertex as determined by MDC
  NTuple::Item<double> fVy0;       ///< Primary \f$y\f$-vertex as determined by MDC
  NTuple::Item<double> fVz0;       ///< Primary \f$z\f$-vertex as determined by MDC
  NTuple::Item<double> fVr0;       ///< Distance from origin in \f$xy\f$-plane
  NTuple::Item<double> fRvxy0;     ///< Nearest distance to IP in \f$xy\f$ plane
  NTuple::Item<double> fRvz0;      ///< Nearest distance to IP in \f$z\f$ direction
  NTuple::Item<double> fRvphi0;    ///< Angle in the \f$xy\f$-plane (?)

  NTuple::Tuple*       fTupleAngles; ///< Photon kinematics
  NTuple::Item<double> fDeltaTheta;
  ///< \f$\theta\f$ angle difference with nearest charged track (degrees)
  NTuple::Item<double> fDeltaPhi;
  ///< \f$\phi\f$ angle difference with nearest charged track (degrees)
  NTuple::Item<double> fDeltaAngle; ///< Angle difference with nearest charged track
  NTuple::Item<double> fEraw;       ///< Energy of the photon

  NTuple::Tuple*       fTupleMgg; ///< Raw invariant mass of the two gammas and their total energy
  NTuple::Item<double> fMtoGG;    ///< Inv. mass of the two gammas
  NTuple::Item<double> fEtot;     ///< Total energy of \f$\pi^+\f$, \f$\pi^-\f$ and the two gammas

  NTuple::Tuple*       fTupleFit4C; ///< 4-constraint (4C) fit information
  NTuple::Item<double> fChi_4C;     ///< \f$\chi^2\f$ of the Kalman kinematic fit
  NTuple::Item<double> fMpi0_4C;    ///< Inv. mass for \f$\pi^0 \rightarrow \gamma\gamma\f$
  NTuple::Item<double> fMD0_4C;     ///< Inv. mass for \f$D^0 \rightarrow K^-\pi^+\f$
  NTuple::Item<double> fMomega_4C;  ///< Inv. mass for \f$\omega \rightarrow \pi^0\pi^-\pi^+\f$
  NTuple::Item<double> fPpi0_4C;    ///< 3-mom. for \f$\pi^0 \rightarrow \gamma\gamma\f$
  NTuple::Item<double> fPD0_4C;     ///< 3-mom. for \f$D^0 \rightarrow K^-\pi^+\f$
  NTuple::Item<double> fPomega_4C;  ///< 3-mom. for \f$\omega \rightarrow \pi^0\pi^-\pi^+\f$

  NTuple::Tuple*       fTupleFit5C; ///< 5-constraint (5C) fit information
  NTuple::Item<double> fChi_5C;     ///< \f$\chi^2\f$ of the Kalman kinematic fit
  NTuple::Item<double> fMpi0_5C;    ///< Inv. mass for \f$\pi^0 \rightarrow \gamma\gamma\f$
  NTuple::Item<double> fMD0_5C;     ///< Inv. mass for \f$D^0 \rightarrow K^-\pi^+\f$
  NTuple::Item<double> fMomega_5C;  ///< Inv. mass for \f$\omega \rightarrow \pi^0\pi^-\pi^+\f$
  NTuple::Item<double> fPpi0_5C;    ///< 3-mom. for \f$\pi^0 \rightarrow \gamma\gamma\f$
  NTuple::Item<double> fPD0_5C;     ///< 3-mom. for \f$D^0 \rightarrow K^-\pi^+\f$
  NTuple::Item<double> fPomega_5C;  ///< 3-mom. for \f$\omega \rightarrow \pi^0\pi^-\pi^+\f$
  NTuple::Item<double> fFcos;       ///< \f$E/|\vec{p}|\f$ ratio for \f$\pi^0\f$ candidate
  NTuple::Item<double> fElow;       ///< Lowest energy of the two photons

  NTuple::Tuple*       fTupleDedx; ///< Energy loss dE/dx
  NTuple::Item<double> fPtrack;    ///< Momentum of the track
  NTuple::Item<double> fChi2e;     ///< \f$\chi^2\f$ in case of electron
  NTuple::Item<double> fChi2mu;    ///< \f$\chi^2\f$ in case of muon
  NTuple::Item<double> fChi2pi;    ///< \f$\chi^2\f$ in case of pion
  NTuple::Item<double> fChi2k;     ///< \f$\chi^2\f$ in case of kaon
  NTuple::Item<double> fChi2p;     ///< \f$\chi^2\f$ in case of proton
  NTuple::Item<double> fProbPH;    ///< Most probable pulse height from truncated mean
  NTuple::Item<double> fNormPH;    ///< Normalized pulse height
  NTuple::Item<double> fGhit;      ///< Number of good hits
  NTuple::Item<double> fThit;      ///< Total number of hits

  NTuple::Tuple*       fTupleTofEC;    ///< End cap ToF decector
  NTuple::Item<double> fPtotTofEC;     ///< Momentum of the track as reconstructed by MDC
  NTuple::Item<double> fPathTofEC;     ///< Path length
  NTuple::Item<double> fTofEC;         ///< Time of flight
  NTuple::Item<double> fCntrTofEC;     ///< ToF counter ID
  NTuple::Item<double> fPhTofEC;       ///< ToF pulse height
  NTuple::Item<double> fRhitTofEC;     ///< Track extrapolate Z or R Hit position
  NTuple::Item<double> fQualTofEC;     ///< Data quality of reconstruction
  NTuple::Item<double> fElectronTofEC; ///< Difference with ToF in electron hypothesis
  NTuple::Item<double> fMuonTofEC;     ///< Difference with ToF in muon hypothesis
  NTuple::Item<double> fProtoniTofEC;  ///< Difference with ToF in charged pion hypothesis
  NTuple::Item<double> fKaonTofEC;     ///< Difference with ToF in charged kaon hypothesis
  NTuple::Item<double> fProtonTofEC;   ///< Difference with ToF in proton hypothesis

  NTuple::Tuple*       fTupleTofIB;    ///< Inner barrel ToF decector
  NTuple::Item<double> fPtotTofIB;     ///< Momentum of the track as reconstructed by MDC
  NTuple::Item<double> fPathTofIB;     ///< Path length
  NTuple::Item<double> fTofIB;         ///< Time of flight
  NTuple::Item<double> fCntrTofIB;     ///< ToF counter ID
  NTuple::Item<double> fPhTofIB;       ///< ToF pulse height
  NTuple::Item<double> fZhitTofIB;     ///< Track extrapolate Z or R Hit position
  NTuple::Item<double> fQualTofIB;     ///< Data quality of reconstruction
  NTuple::Item<double> fElectronTofIB; ///< Difference with ToF in electron hypothesis
  NTuple::Item<double> fMuonTofIB;     ///< Difference with ToF in muon hypothesis
  NTuple::Item<double> fProtoniTofIB;  ///< Difference with ToF in charged pion hypothesis
  NTuple::Item<double> fKaonTofIB;     ///< Difference with ToF in charged kaon hypothesis
  NTuple::Item<double> fProtonTofIB;   ///< Difference with ToF in proton hypothesis

  NTuple::Tuple*       fTupleTofOB;    ///< Outer barrel ToF decector
  NTuple::Item<double> fPtotTofOB;     ///< Momentum of the track as reconstructed by MDC
  NTuple::Item<double> fPathTofOB;     ///< Path length
  NTuple::Item<double> fTofOB;         ///< Time of flight
  NTuple::Item<double> fCntrTofOB;     ///< ToF counter ID
  NTuple::Item<double> fPhTofOB;       ///< ToF pulse height
  NTuple::Item<double> fZhitTofOB;     ///< Track extrapolate Z or R Hit position
  NTuple::Item<double> fQualTofOB;     ///< Data quality of reconstruction
  NTuple::Item<double> fElectronTofOB; ///< Difference with ToF in electron hypothesis
  NTuple::Item<double> fMuonTofOB;     ///< Difference with ToF in muon hypothesis
  NTuple::Item<double> fProtoniTofOB;  ///< Difference with ToF in charged pion hypothesis
  NTuple::Item<double> fKaonTofOB;     ///< Difference with ToF in charged kaon hypothesis
  NTuple::Item<double> fProtonTofOB;   ///< Difference with ToF in proton hypothesis

  NTuple::Tuple*       fTuplePID;  ///< Particle ID info
  NTuple::Item<double> fPtrackPID; ///< Momentum of the track
  NTuple::Item<double> fCostPID;   ///< Theta angle of the track
  NTuple::Item<double> fDedxPID;   ///< \f$\chi^2\f$ of the \f$dE/dx\f$ of the track
  NTuple::Item<double> fTof1PID;   ///< \f$\chi^2\f$ of the inner barrel ToF of the track
  NTuple::Item<double> fTof2PID;   ///< \f$\chi^2\f$ of the outer barrel ToF of the track
  NTuple::Item<double> fProbPi;    ///< Probability that it is a pion
  NTuple::Item<double> fProbK;     ///< Probability that it is a kaon

  NTuple::Tuple*     fTupleMC; /// `NTuple` that will be the eventual `"MctruthForTopoAna"` tree.
  NTuple::Item<double> fMC_4C_mD0;
  NTuple::Item<double> fMC_4C_momega;
  NTuple::Item<double> fMC_5C_mD0;
  NTuple::Item<double> fMC_5C_momega;
  NTuple::Item<int>    fRunid; ///< Run number ID.
  NTuple::Item<int>    fEvtid; ///< Rvent number ID.
  NTuple::Item<int>    fNparticles;
  ///< Number of MC particles stored for this event. This one is necessary for loading following two items, because they are arrays.
  NTuple::Array<int> fPDG;    ///< PDG code for the particle in this array.
  NTuple::Array<int> fMother; ///< Track index of the mother particle.

  NTuple::Tuple*     fTupleCutFlow;
  NTuple::Item<int>  fNCut0;
  NTuple::Item<int>  fNCut1;
  NTuple::Item<int>  fNCut2;
  NTuple::Item<int>  fNCut3;
  NTuple::Item<int>  fNCut4;
  NTuple::Item<int>  fNCut5;
  NTuple::Item<int>  fNCut6;

  ///@}
};

/// @}
#endif