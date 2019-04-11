#ifndef Analysis_TrackSelector_H
#define Analysis_TrackSelector_H

// * ========================================= * //
// * ------- DOXYGEN GROUP DEFINITIONS ------- * //
// * ========================================= * //

/// @defgroup BOSS_packages Event selection
/// Event selection __packages__ for the BOSS framework. The core of these packages is the
/// `TrackSelector` algorithm, which is the base class for all subalgorithms, such as `D0phi_KpiKK`.

/// @defgroup BOSS_objects Helper objects
/// Helper __classes__ for the event selection packages. An example would be the `CutObject`, which
/// is can be used in all packages for applying cuts and storing their paramters in the final ROOT
/// output file.

/// @defgroup BOSS_globals Globals
/// Global __parameters__ encapsulated in namespaces.

// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

#include "CLHEP/Geometry/Point3D.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRefVector.h"
#include "McTruth/McEvent.h"
#include "McTruth/McParticle.h"
#include "ParticleID/ParticleID.h"
#include "TH1D.h"
#include "THStack.h"
#include "TofRecEvent/RecTofTrack.h"
#include "TrackSelector/Containers/CutObject.h"
#include "TrackSelector/Containers/JobSwitch.h"
#include "TrackSelector/Containers/NTupleContainer.h"
#include "TrackSelector/Containers/ParticleTracks.h"
#include "TrackSelector/KKFitResult.h"
#include "VertexFit/KalmanKinematicFit.h"
#include <map> /// @todo It would be more efficient to use `unordered_map`, but this is a `c++11` feature...
#include <string>
#include <vector>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

// * ==================================== * //
// * ------- GLOBALS AND TYPEDEFS ------- * //
// * ==================================== * //
/// @addtogroup BOSS_globals
/// @{

/// Namespace that contains some parameters (such as particle masses) that are useful for derived
/// classes of the `TrackSelector` base algorithm.
namespace TSGlobals
{
  const double gM_rho = 0.77526;
  ///< Mass of \f$\rho^{0\pm}\f$, see
  ///< [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-rho-770.pdf).
  const double gM_pi0 = 0.1349770;
  ///< Mass of \f$\pi^0\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-pi-zero.pdf).
  const double gM_pi = 0.13957061;
  ///< Mass of \f$\pi^\pm\f$, see
  ///< [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-pi-plus-minus.pdf).
  const double gM_K = 0.493677;
  ///< Mass of \f$K^\pm\f$.
  const double gM_D0 = 1.86483;
  ///< Mass of \f$D^0\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-D-zero.pdf).
  const double gM_phi = 1.019461;
  ///< Mass of \f$\phi\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-phi-1020.pdf).
  const double gM_omega = 0.78265;
  ///< Mass of \f$\omega\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-omega-782.pdf).
  const double gM_Jpsi = 3.0969;
  ///< Mass of \f$J/\psi\f$, see [PDG](http://pdg.lbl.gov/2018/listings/rpp2018-list-J-psi-1S.pdf).
  const double gEcms = 3.097;
  ///< Center-of-mass energy.
  const HepLorentzVector gEcmsVec(0.034, 0, 0, gEcms);
}; // namespace TSGlobals

/// @}
// * ================================ * //
// * ------- CLASS DEFINITION ------- * //
// * ================================ * //
/// @addtogroup BOSS_packages
/// @{

/// **Base algorithm** that creates a selection of charged and/or neutral tracks. Your analysis
/// has to be defined in *derived* algorithms, making use of the vectors that are filled in
/// this algorithm. See the [tutorial
/// pages](https://besiii.gitbook.io/boss/the-boss-afterburner/initial) for more information.
/// @todo The design of using `map`s of `NTuple::Items`s is not ideal, as it does not (yet) allow
/// enough flexibility.
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     December 14th, 2018
class TrackSelector : public Algorithm
{
public:
  /// @name Constructors
  ///@{
  TrackSelector(const std::string& name, ISvcLocator* pSvcLocator);
  void PostConstructor();
  ///@}

  /// @name Gaudi Algorithm steps
  ///@{
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  ///@}

protected:
  /// @name Derived Algorithm steps
  ///@{
  virtual StatusCode initialize_rest() = 0;
  ///< This function is executed at the end of `initialize`. It should be

  ///< further defined in derived subalgorithms.
  virtual StatusCode execute_rest() = 0;
  ///< This function is executed at the end of `execute`. It

  ///< should be further defined in derived subalgorithms.
  virtual StatusCode finalize_rest() = 0;
  ///< This function is executed at the end of `finalize`. It should be further

  ///< defined in derived subalgorithms.
  void      PrintFunctionName(const char* class_name, const char* function_name);
  MsgStream fLog;
  ///< Stream object for logging. It needs to be declared as a data member so that

  ///< it is accessible to all methods of this class.
  ///@}

  /// @name NTuple handlers
  ///@{
  void         AddNTupleItems_MCTruth(NTupleContainer& tuple);
  void         AddNTupleItems_Dedx(NTupleContainer& tuple);
  void         AddNTupleItems_Tof(NTupleContainer& tuple);
  bool         CreateMCTruthCollection();
  virtual void CreateMCTruthSelection() = 0;
  ///< Function that should be defined in the derived calss and called after
  ///< `CreateMCtruthCollection`. See [here](http://home.fnal.gov/~mrenna/lutp0613man2/node44.html)
  ///< for a list of PDG codes.
  ///@}

  /// @name Write methods
  ///@{
  void         WriteDedxInfo(EvtRecTrack* evtRecTrack, NTupleContainer& tuple);
  void         WriteDedxInfoForVector(std::vector<EvtRecTrack*>& vector, NTupleContainer& tuple);
  void         WritePIDInformation();
  void         WriteTofInformation(SmartRefVector<RecTofTrack>::iterator iter_tof, double ptrk,
                                   NTupleContainer& tuple);
  void         WriteFitResults(KKFitResult* fitresult, NTupleContainer& tuple);
  bool         WriteMCTruthForTopoAna(NTupleContainer& tuple);
  virtual void SetFitNTuple(KKFitResult* fitresult, NTupleContainer& tuple) = 0;
  ///< Virtual method that is executed in `WriteFitResults` and should be further specified in the
  ///< derived classes. @param fitresult This parameter is a pointer to allow for `dynamic_cast` in
  ///< the derived specification of this `virtual` function. @param tuple The `NTuple` to which you
  ///< eventually want to write the results.
  ///@}

  /// @name Access to the DST file
  ///@{
  SmartDataPtr<Event::EventHeader> fEventHeader;
  ///< Data pointer for `"Event/EventHeader"` data. It is set in `execute()` step in each event.
  SmartDataPtr<Event::McParticleCol> fMcParticleCol;
  ///< Data pointer for `"Event/MC/McParticleCol"` data. It is set in `execute()` step in each
  ///< event.
  SmartDataPtr<EvtRecEvent> fEvtRecEvent; ///< Data pointer for `EventModel::EvtRec::EvtRecEvent`
                                          ///< which is set in `execute()` in each event.
  SmartDataPtr<EvtRecTrackCol>
    fEvtRecTrkCol; ///< Data pointer for `EventModel::EvtRec::EvtRecTrackCol` which is set in
                   ///< `execute()` in each event.
                   ///@}

  /// @name Track collections and iterators
  ///@{
  std::vector<Event::McParticle*> fMcParticles;
  ///< Vector that, in each event, will be filled by a selection of pointers to MC particles that
  ///< are of interest. Only generated particles that are detectable are included. The cluster and
  ///< everything that comes before it is also not included. See
  ///< [here](https://besiii.gitbook.io/boss/besiii-software-system/packages/analysis/topoana#structure-of-the-event-mcparticlecol-collection)
  ///< for a better explanation. @remark Note that this vector has to be used in the derived
  ///< algorithm, e.g. by filling the data members of the `fNTuple_mctruth` member and calling its
  ///< `NTupleTopoAna::Write` method, otherwise it is useless.
  std::vector<EvtRecTrack*> fGoodChargedTracks;
  ///< Vector that, in each event, will be filled by a selection of pointers to 'good' charged
  ///< tracks.
  std::vector<EvtRecTrack*> fGoodNeutralTracks;
  ///< Vector that, in each event, will be filled by a selection of pointers to 'good' neutral
  ///< tracks (photons).
  std::vector<EvtRecTrack*>::iterator fTrackIterator;
  ///< Iterator for looping over the collection of charged and neutral tracks (`EvtRecTrackCol`).
  bool fCreateChargedCollection;
  ///< A switch that allows the derived algorithms to decide whether to create a collection of @b
  ///< charged tracks in the `TrackSelector::execute` step. This switch is hard-coded, and
  ///< inaccessible to the job options, because it depends on the nature of the derived analysis
  ///< whether or not to create this collection.
  bool fCreateNeutralCollection;
  ///< A switch that allows the derived algorithms to decide whether to create a collection of @b
  ///< neutral tracks in the `TrackSelector::execute` step. This switch is hard-coded, and
  ///< inaccessible to the job options, because it depends on the nature of the derived analysis
  ///< whether or not to create this collection.
  ///@}

  /// @name Reconstructed track data
  ///@{
  RecEmcShower* fTrackEMC;
  ///< Pointer to reconstructed data from the EMC. See more info in
  ///< [`RecEmcShower`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecEmcShower.html).
  RecExtTrack* fTrackExt;
  ///< Pointer to reconstructed data from the extension through all detectors. See more info in
  ///< [`RecExtTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecExtTrack.html).
  RecMdcDedx* fTrackDedx;
  ///< Pointer to reconstructed \f$dE/dx\f$ data from the MDC. See more info in
  ///< [`RecMdcDedx`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecMdcDedx.html).
  RecMdcKalTrack* fTrackKal;
  ///< Pointer to reconstructed Kalman fit data from the MDC. See more info in
  ///< [`RecMdcKalTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecMdcKalTrack.html).
  RecMdcTrack* fTrackMDC;
  ///< Pointer to reconstructed track data from the MDC. See more info in
  ///< [`RecMdcTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecMdcTrack.html).
  RecMucTrack* fTrackMUC;
  ///< Pointer to reconstructed data from the MUC. See more info in
  ///< [`RecMucTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecMucTrack.html).
  RecTofTrack* fTrackTOF;
  ///< Pointer to reconstructed data from the TOF. See more info in
  ///< [`RecTofTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecTofTrack.html).
  ///@}

  /// @name NTuples (eventual TTrees)
  ///@{
  NTupleContainer fNTuple_cuts; ///< `NTuple::Tuple` container for the cut parameters.
  NTupleContainer
    fNTuple_PID; ///< `NTuple::Tuple` container for the general PID information (TOF, \f$dE/dx\f$,
                 ///< etc) branch. **Needs to be filled in the derived class!**
  NTupleContainer fNTuple_TofEC; ///< `NTuple::Tuple` container for the data from the Time-of-Flight
                                 ///< end cap detector branch.
  NTupleContainer fNTuple_TofIB; ///< `NTuple::Tuple` container for the data from the Time-of-Flight
                                 ///< inner barrel detector branch.
  NTupleContainer fNTuple_TofOB; ///< `NTuple::Tuple` container for the data from the Time-of-Flight
                                 ///< outer barrel detector branch.
  NTupleContainer
    fNTuple_charged; ///< `NTuple::Tuple` container for the charged track vertex branch.
  NTupleContainer
                  fNTuple_dedx; ///< `NTuple::Tuple` container for the energy loss (\f$dE/dx\f$) branch.
  NTupleContainer fNTuple_mult;     ///< `NTuple::Tuple` container for the multiplicities branch.
  NTupleContainer fNTuple_mult_sel; ///< `NTuple::Tuple` container for the `"mult_select"` branch.
  NTupleContainer fNTuple_neutral; ///< `NTuple::Tuple` container for the neutral track info neutral
                                   ///< track info branch.
  NTupleContainer
    fNTuple_vertex; ///< `NTuple::Tuple` container for the primary vertex info vertex branch.
  NTupleContainer
    fNTuple_topology; ///< `NTuple::Tuple` container for the decay topology according to Monte Carlo
                      ///< truth. This `NTuple` contains indexed items (`NTuple::Array`) and
                      ///< therefore had to be further specified in a `NTupleTopoAna` object, a
                      ///< derived class of `NTupleContainer`.
  ///@}

  /// @name Counters and cut objects
  ///@{
  NTuple::Tuple* fCutTuples;

  CutObject fCounter_Ntracks;  ///< Cummulative total number of tracks.
  CutObject fCounter_Ncharged; ///< Cummulative total number of charged tracks.
  CutObject fCounter_Nmdcvalid;
  ///< Cummulative total number of charged tracks that are 'MDC valid'.
  CutObject fCounter_Nneutral;
  ///< Cummulative total number of neutral tracks.
  CutObject fCutFlow_Nevents;
  ///< **Cut flow counter**: total number of events.
  CutObject fCut_Vxy;
  ///< Cut on the radius \f$r\f$ of the primary vertex.
  CutObject fCut_Vz;
  ///< Cut on the \f$z\f$ coordinate of the primary vertex.
  CutObject fCut_Rxy;
  ///< Cut on the distance in the \f$z\f$ direction between the primary vertex and the vertex of the
  ///< charged track.
  CutObject fCut_Rz;
  ///< Cut on the distance in the \f$xy\f$ plane between the primary vertex and the vertex of the
  ///< charged track.
  CutObject fCut_CosTheta;     ///< Cut on \f$\cos(\theta)\f$.
  CutObject fCut_PhotonEnergy; ///< Cut on the photon energy.
  CutObject fCut_PIDChiSq;
  ///< Cut on the \f$\chi_\mathrm{red}^2\f$ of the kinematic Kalman fits.
  CutObject fCut_PIDProb;
  ///< Cut on the probability that a particle is either a kaon, pion, electron, muon, or proton
  ///< according to the probability method. See for instance
  ///< [`ParticleID::probPion`](http://bes3.to.infn.it/Boss/7.0.2/html/classParticleID.html#147bb7be5fa47f275ca3b32e6ae8fbc6).
  ///@}

  /// @name Particle Identification
  ///@{
  ParticleID* fPIDInstance;
  ///< Pointer to instance of particle identification (PID). Only used in *derived subalgorithms*.
  ParticleID* InitializePID(const int method, const int pidsys, const int pidcase,
                            const double chimin = 4.);
  bool        IsDecay(Event::McParticle* particle, const int mother) const;
  bool        IsDecay(Event::McParticle* particle, const int mother, const int pdg) const;
  ///@}

  /// @name Other stored values
  ///@{
  HepPoint3D fVertexPoint;
  ///< Coordinates of the interaction point (primary vertex). Set in each event in
  ///< `TrackSelector::execute`.
  Int_t fNetChargeMDC;
  ///< Net charge detected in the MDC. Should be zero, so use can use this value as a cut. This for
  ///< instance happens in the `RhopiAlg` example.
  ///@}

private:
  /// @name NTuple handlers
  ///@{
  void BookNTuple(NTupleContainer& tuple);
  void BookNTuples();
  void DeclareSwitches();
  void CreateChargedCollection();
  void CreateNeutralCollection();
  ///@}

  /// @name Cut handlers
  ///@{
  void AddAndWriteCuts();
  void DeclareCuts();
  ///@}

  /// @name Computational
  ///@{
  HepLorentzVector ComputeMomentum(EvtRecTrack* track);
  ///@}

  /// @name Constructors
  ///@{
  bool fPostConstructed;
  ///< Boolean that keeps track of whether the `PostConstructor has actually been called. Yes, it's
  ///< a crappy solution... see `PostConstructor`.
  ///@}
};

/// @}
#endif