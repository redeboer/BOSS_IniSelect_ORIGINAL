
#include <iostream>
#include <sstream>

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

#include "VertexFit/KalmanKinematicFit.h"
//#include "VertexFit/KinematicFit.h"
#include "VertexFit/IVertexDbSvc.h"
#include "VertexFit/SecondVertexFit.h"
#include "VertexFit/VertexFit.h"
//#include "VertexFit/IBeamEnergySvc.h"
#include "EvtRecEvent/EvtRecEtaToGG.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "McTruth/McParticle.h"
#include "MeasuredEcmsSvc/IMeasuredEcmsSvc.h"
#include "ParticleID/ParticleID.h"
#include "VertexFit/Helix.h"

#include "FSFilter/FSFilter.h"

//********************************************************************
//********************************************************************
//********************************************************************
//
//   CONSTRUCTOR
//
//********************************************************************
//********************************************************************
//********************************************************************

FSFilter::FSFilter(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{

  //********************************************************************
  //
  //   READ PROPERTIES FROM THE JOB OPTIONS FILE
  //
  //********************************************************************

  declareProperty("maxShowers", m_maxShowers = 50);
  declareProperty("maxExtraTracks", m_maxExtraTracks = 2);
  declareProperty("cmEnergy", m_cmEnergyParameter = 3.686093);
  declareProperty("crossingAngle", m_crossingAngle = 0.011);
  declareProperty("energyTolerance", m_energyTolerance = 0.25);
  declareProperty("maxKinematicFitChi2DOF", m_maxChi2DOF = 100.0);
  declareProperty("momentumTolerance", m_momentumTolerance = 0.25);
  declareProperty("trackStudies", m_trackStudies = false);
  declareProperty("extraKaonPID", m_extraKaonPID = false);
  declareProperty("pidStudies", m_pidStudies = false);
  declareProperty("neutralStudies", m_neutralStudies = false);
  declareProperty("pi0Studies", m_pi0Studies = false);
  declareProperty("etaStudies", m_etaStudies = false);
  declareProperty("veeStudies", m_veeStudies = false);
  declareProperty("bypassVertexDB", m_bypassVertexDB = false);
  declareProperty("YUPING", m_yuping = false);
  declareProperty("writeNTGen", m_writeNTGen = true);
  declareProperty("writeNTGenCode", m_writeNTGenCode = true);
  declareProperty("printTruthInformation", m_printTruthInformation = false);
  declareProperty("isolateRunLow", m_isolateRunLow = -1);
  declareProperty("isolateRunHigh", m_isolateRunHigh = -1);
  declareProperty("isolateEventLow", m_isolateEventLow = -1);
  declareProperty("isolateEventHigh", m_isolateEventHigh = -1);
  declareProperty("fastJpsi", m_fastJpsi = false);

  // determine which final states to reconstruct

  for(int i = 0; i < MAXFSNAMES; i++)
  {
    stringstream parname;
    parname << "FS";
    parname << i;
    declareProperty(parname.str(), m_FSNames[i]);
  }

  // parameters for cuts on intermediate states

  for(int i = 0; i < MAXFSCUTS; i++)
  {
    stringstream parname;
    parname << "FSCut";
    parname << i;
    declareProperty(parname.str(), m_FSCuts[i]);
  }

  // parameters for constraints on intermediate states

  for(int i = 0; i < MAXFSCONSTRAINTS; i++)
  {
    stringstream parname;
    parname << "FSConstrain";
    parname << i;
    declareProperty(parname.str(), m_FSConstraints[i]);
  }

  // parameters for missing mass fits

  for(int i = 0; i < MAXFSMMFITS; i++)
  {
    stringstream parname;
    parname << "FSMMFit";
    parname << i;
    declareProperty(parname.str(), m_FSMMFits[i]);
  }

  //********************************************************************
  //
  //   INITIALIZE GLOBAL VARIABLES
  //
  //********************************************************************

  m_runNumberOld = 0;

  // initialize the event counter

  m_eventCounter = 0;
}

//********************************************************************
//********************************************************************
//********************************************************************
//
//   INITIALIZE
//
//********************************************************************
//********************************************************************
//********************************************************************

StatusCode FSFilter::initialize()
{

  //********************************************************************
  //
  //   CREATE TREES AND SET UP FSINFO OBJECTS
  //
  //********************************************************************

  // only reconstruct particles if we need to

  m_findE      = false;
  m_findMu     = false;
  m_findPi     = false;
  m_findK      = false;
  m_findP      = false;
  m_findGamma  = false;
  m_findPi0    = false;
  m_findEta    = false;
  m_findKs     = false;
  m_findLambda = false;

  // create trees for each final state and store them in FSInfo objects

  for(unsigned int i = 0; i < MAXFSNAMES; i++)
  {
    if(m_FSNames[i] != "")
    {

      // a tree for reconstructed information -- nt[EXC/INC][CODE2]_[CODE1]

      string ntName("nt");
      ntName += m_FSNames[i];
      string ntFullName("FILE1/");
      ntFullName += ntName;
      NTupleHelper* nt =
        new NTupleHelper(ntupleSvc()->book(ntFullName, CLID_ColumnWiseTuple, ntName));

      // a tree for generated information -- ntGEN[CODE2]_[CODE1]

      NTupleHelper* ntgen = NULL;
      if(m_writeNTGenCode)
      {
        string ntGenName("ntGEN");
        ntGenName += m_FSNames[i];
        if(ntGenName.find("EXC") != string::npos)
        {
          ntGenName.erase(ntGenName.find("EXC"), 3);
          string ntGenFullName("FILE1/");
          ntGenFullName += ntGenName;
          ntgen =
            new NTupleHelper(ntupleSvc()->book(ntGenFullName, CLID_ColumnWiseTuple, ntGenName));
        }
      }

      // create an FSFinfo object

      FSInfo* fsInfo = new FSInfo(m_FSNames[i], nt, ntgen);
      m_FSInfoVector.push_back(fsInfo);

      // check to see which particles we need to reconstruct

      if(fsInfo->hasParticle("e+") || fsInfo->hasParticle("e-")) m_findE = true;
      if(fsInfo->hasParticle("mu+") || fsInfo->hasParticle("mu-")) m_findMu = true;
      if(fsInfo->hasParticle("pi+") || fsInfo->hasParticle("pi-")) m_findPi = true;
      if(fsInfo->hasParticle("K+") || fsInfo->hasParticle("K-")) m_findK = true;
      if(fsInfo->hasParticle("p+") || fsInfo->hasParticle("p-")) m_findP = true;
      if(fsInfo->hasParticle("gamma")) m_findGamma = true;
      if(fsInfo->hasParticle("pi0")) m_findPi0 = true;
      if(fsInfo->hasParticle("eta")) m_findEta = true;
      if(fsInfo->hasParticle("Ks")) m_findKs = true;
      if(fsInfo->hasParticle("Lambda") || fsInfo->hasParticle("ALambda")) m_findLambda = true;
    }
  }

  // a tree for mode-independent generator information -- ntGEN

  if(m_writeNTGen)
  {
    string ntGenName("ntGEN");
    string ntGenFullName("FILE1/");
    ntGenFullName += ntGenName;
    m_ntGEN = new NTupleHelper(ntupleSvc()->book(ntGenFullName, CLID_ColumnWiseTuple, ntGenName));
  }

  //********************************************************************
  //
  //   CREATE FSCUT OBJECTS
  //
  //********************************************************************

  for(unsigned int i = 0; i < MAXFSCUTS; i++)
  {
    if(m_FSCuts[i] != "")
    {
      FSCut* fscut = new FSCut(m_FSCuts[i]);
      bool   found = false;
      for(unsigned int ifs = 0; ifs < m_FSInfoVector.size(); ifs++)
      {
        if(m_FSInfoVector[ifs]->FSName() == fscut->FSName())
        {
          m_FSInfoVector[ifs]->addFSCut(fscut);
          found = true;
          break;
        }
      }
      if(!found)
      {
        cout << "FSFilter ERROR: could not find a final state for the " << endl;
        cout << "  FSCut with these arguments: " << endl;
        cout << m_FSCuts[i] << endl;
        cout << "  and final state = " << fscut->FSName() << endl;
        exit(0);
      }
    }
  }

  //********************************************************************
  //
  //   CREATE FSCONSTRAINT OBJECTS
  //
  //********************************************************************

  for(unsigned int i = 0; i < MAXFSCONSTRAINTS; i++)
  {
    if(m_FSConstraints[i] != "")
    {
      FSConstraint* fsconstraint = new FSConstraint(m_FSConstraints[i]);
      bool          found        = false;
      for(unsigned int ifs = 0; ifs < m_FSInfoVector.size(); ifs++)
      {
        if(m_FSInfoVector[ifs]->FSName() == fsconstraint->FSName())
        {
          m_FSInfoVector[ifs]->addFSConstraint(fsconstraint);
          found = true;
          break;
        }
      }
      if(!found)
      {
        cout << "FSFilter ERROR: could not find a final state for the " << endl;
        cout << "  FSConstraint with these arguments: " << endl;
        cout << m_FSConstraints[i] << endl;
        cout << "  and final state = " << fsconstraint->FSName() << endl;
        exit(0);
      }
    }
  }

  //********************************************************************
  //
  //   ADD MISSING MASS FITS
  //
  //********************************************************************

  for(unsigned int i = 0; i < MAXFSMMFITS; i++)
  {
    if(m_FSMMFits[i] != "")
    {

      vector<string> pars = FSInfo::parseString(m_FSMMFits[i]);

      if(pars.size() != 2)
      {
        cout << "FSFilter ERROR: bad FSMMFit" << i << " parameter: " << m_FSMMFits[i] << endl;
        exit(0);
      }

      bool found = false;
      for(unsigned int ifs = 0; ifs < m_FSInfoVector.size(); ifs++)
      {
        if(m_FSInfoVector[ifs]->FSName() == pars[0])
        {
          m_FSInfoVector[ifs]->setMissingMass(atof(pars[1].c_str()));
          found = true;
          break;
        }
      }
      if(!found)
      {
        cout << "FSFilter ERROR: could not find a final state for the " << endl;
        cout << "  FSMMFit" << i << " with these arguments: " << endl;
        cout << m_FSMMFits[i] << endl;
        exit(0);
      }
    }
  }

  //********************************************************************
  //
  //   PERFORM CHECKS ON THE INPUT AND EXIT IF THERE ARE PROBLEMS
  //
  //********************************************************************

  // debugging?

  m_debug = false;
  if(m_isolateRunLow >= 0 || m_isolateEventLow >= 0 || m_isolateRunHigh >= 0 ||
     m_isolateEventHigh >= 0)
  {
    m_debug                 = true;
    m_printTruthInformation = true;
  }

  // make sure all final states are currently supported

  //for (unsigned int i = 0; i < m_FSInfoVector.size(); i++){
  //  FSInfo* fs = m_FSInfoVector[i];
  //  vector<string> particleNames = fs->particleNames();
  //  for (unsigned int j = 0; j < particleNames.size(); j++){
  //  }
  //}

  return StatusCode::SUCCESS;
}

//********************************************************************
//********************************************************************
//********************************************************************
//
//   EXECUTE
//
//********************************************************************
//********************************************************************
//********************************************************************

StatusCode FSFilter::execute()
{

  //********************************************************************
  //
  //   EXTRACT THE EVENT HEADER AND EVENT INFO FROM THE FRAMEWORK
  //
  //********************************************************************

  // event header

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(), EventModel::EventHeader);
  int                              runNumber   = eventHeader->runNumber();
  int                              eventNumber = eventHeader->eventNumber();
  m_eventCounter++;
  if(m_eventCounter % 100 == 0)
    cout << "event counter (run, event) = " << m_eventCounter << " (" << runNumber << ","
         << eventNumber << ")" << endl;
  if(m_debug && ((m_isolateRunLow >= 0 && abs(runNumber) < m_isolateRunLow) ||
                 (m_isolateRunHigh >= 0 && abs(runNumber) > m_isolateRunHigh) ||
                 (m_isolateEventLow >= 0 && eventNumber < m_isolateEventLow) ||
                 (m_isolateEventHigh >= 0 && eventNumber > m_isolateEventHigh)))
  { return StatusCode::SUCCESS; } if(runNumber > 0)
    m_yuping = false;

  // beam energy (still waiting to implement BeamEnergySvc)

  //IBeamEnergySvc* beamEnergySvc;
  //Gaudi::svcLocator()->service("BeamEnergySvc",beamEnergySvc);
  //cout << endl << endl << endl << "TESTING BEAM ENERGY" << endl;
  //cout << beamEnergySvc->getbeamE() << endl << endl << endl << endl;

  // BEAM ENERGY

  if(runNumber != m_runNumberOld)
  {
    m_runNumberOld = runNumber;

    // if input Ecm is greater than zero, use this

    if(m_cmEnergyParameter > 0) { m_cmEnergy = m_cmEnergyParameter; }

    // beam energy using MeasuredEcmsSvc (implemented by Dan)
    // use the database for
    //   4010 data or 2013 XYZ data or 2014 XYZ data

    else if(((abs(runNumber) >= 23463) && (abs(runNumber) <= 24141)) ||
            ((abs(runNumber) >= 29677) && (abs(runNumber) <= 33719)) ||
            ((abs(runNumber) >= 35227) && (abs(runNumber) <= 38140)))
    {
      IMeasuredEcmsSvc* ecmsSvc;
      StatusCode        status = serviceLocator()->service("MeasuredEcmsSvc", ecmsSvc, true);
      if(!status.isSuccess()) return status;
      int    sample_name  = ecmsSvc->getSampleName(runNumber);
      double ecms         = ecmsSvc->getEcms(runNumber);
      double ecms_err     = ecmsSvc->getEcmsErr(runNumber);
      double avg_ecms     = ecmsSvc->getAveEcms(runNumber);
      double avg_ecms_err = ecmsSvc->getAveEcmsErr(runNumber);
      m_cmEnergy          = ecms;
    }

    // otherwise use hardcoded energies

    else
    {

      // 4010 running
      if((abs(runNumber) >= 23463) && (abs(runNumber) <= 24141)) { m_cmEnergy = 4.010; }
      else if((abs(runNumber) >= 24142) && (abs(runNumber) <= 24177))
      {
        m_cmEnergy = 3.686093;
      }
      // XYZ before Tsinghua
      else if((abs(runNumber) >= 29677) && (abs(runNumber) <= 30367))
      {
        m_cmEnergy = 4.260;
      }
      else if((abs(runNumber) >= 30372) && (abs(runNumber) <= 30437))
      {
        m_cmEnergy = 4.190;
      }
      else if((abs(runNumber) >= 30438) && (abs(runNumber) <= 30491))
      {
        m_cmEnergy = 4.230;
      }
      else if((abs(runNumber) >= 30492) && (abs(runNumber) <= 30557))
      {
        m_cmEnergy = 4.310;
      }
      else if((abs(runNumber) >= 30616) && (abs(runNumber) <= 31279))
      {
        m_cmEnergy = 4.360;
      }
      else if((abs(runNumber) >= 31281) && (abs(runNumber) <= 31325))
      {
        m_cmEnergy = 4.390;
      }
      else if((abs(runNumber) >= 31327) && (abs(runNumber) <= 31390))
      {
        m_cmEnergy = 4.420;
      }
      // XYZ after Tsinghua
      else if((abs(runNumber) >= 31561) && (abs(runNumber) <= 31981))
      {
        m_cmEnergy = 4.260;
      }
      else if((abs(runNumber) >= 31983) && (abs(runNumber) <= 32045))
      {
        m_cmEnergy = 4.210;
      }
      else if((abs(runNumber) >= 32046) && (abs(runNumber) <= 32140))
      {
        m_cmEnergy = 4.220;
      }
      else if((abs(runNumber) >= 32141) && (abs(runNumber) <= 32226))
      {
        m_cmEnergy = 4.245;
      }
      else if((abs(runNumber) >= 32239) && (abs(runNumber) <= 33484))
      {
        m_cmEnergy = 4.230;
      }
      else if((abs(runNumber) >= 33490) && (abs(runNumber) <= 33556))
      {
        m_cmEnergy = 3.810;
      }
      else if((abs(runNumber) >= 33571) && (abs(runNumber) <= 33657))
      {
        m_cmEnergy = 3.900;
      }
      else if((abs(runNumber) >= 33659) && (abs(runNumber) <= 33719))
      {
        m_cmEnergy = 4.090;
      }
      // 2014 XYZ
      else if((abs(runNumber) >= 35227) && (abs(runNumber) <= 36213))
      {
        m_cmEnergy = 4.600;
      }
      else if((abs(runNumber) >= 36245) && (abs(runNumber) <= 36393))
      {
        m_cmEnergy = 4.470;
      }
      else if((abs(runNumber) >= 36398) && (abs(runNumber) <= 36588))
      {
        m_cmEnergy = 4.530;
      }
      else if((abs(runNumber) >= 36603) && (abs(runNumber) <= 36699))
      {
        m_cmEnergy = 4.575;
      }
      else if((abs(runNumber) >= 36773) && (abs(runNumber) <= 38140))
      {
        m_cmEnergy = 4.420;
      }
      // J/psi
      else if((abs(runNumber) >= 9947) && (abs(runNumber) <= 10878))
      {
        m_cmEnergy = 3.097;
      }
      //else if ((abs(runNumber) >= 27255) && (abs(runNumber) <= 28236)){ m_cmEnergy = 3.097; }
      else if((abs(runNumber) >= 27255) && (abs(runNumber) <= 28293))
      {
        m_cmEnergy = 3.097;
      }
      // 3650
      else if((abs(runNumber) >= 9613) && (abs(runNumber) <= 9779))
      {
        m_cmEnergy = 3.650;
      }
      else if((abs(runNumber) >= 33725) && (abs(runNumber) <= 33772))
      {
        m_cmEnergy = 3.650;
      }
      // psi(2S)
      else if((abs(runNumber) >= 8093) && (abs(runNumber) <= 9025))
      {
        m_cmEnergy = 3.686093;
      }
      else if((abs(runNumber) >= 25338) && (abs(runNumber) <= 27090))
      {
        m_cmEnergy = 3.686093;
      }
      // R SCAN 2014
      else if((abs(runNumber) >= 34011) && (abs(runNumber) <= 35118))
      {
        if((abs(runNumber) >= 34011) && (abs(runNumber) <= 34027)) { m_cmEnergy = 3.850; }
        else if((abs(runNumber) >= 34028) && (abs(runNumber) <= 34036))
        {
          m_cmEnergy = 3.890;
        }
        else if((abs(runNumber) >= 34037) && (abs(runNumber) <= 34045))
        {
          m_cmEnergy = 3.895;
        }
        else if((abs(runNumber) >= 34046) && (abs(runNumber) <= 34057))
        {
          m_cmEnergy = 3.900;
        }
        else if((abs(runNumber) >= 34058) && (abs(runNumber) <= 34068))
        {
          m_cmEnergy = 3.905;
        }
        else if((abs(runNumber) >= 34069) && (abs(runNumber) <= 34076))
        {
          m_cmEnergy = 3.910;
        }
        else if((abs(runNumber) >= 34077) && (abs(runNumber) <= 34083))
        {
          m_cmEnergy = 3.915;
        }
        else if((abs(runNumber) >= 34084) && (abs(runNumber) <= 34090))
        {
          m_cmEnergy = 3.920;
        }
        else if((abs(runNumber) >= 34091) && (abs(runNumber) <= 34096))
        {
          m_cmEnergy = 3.925;
        }
        else if((abs(runNumber) >= 34097) && (abs(runNumber) <= 34104))
        {
          m_cmEnergy = 3.930;
        }
        else if((abs(runNumber) >= 34105) && (abs(runNumber) <= 34117))
        {
          m_cmEnergy = 3.935;
        }
        else if((abs(runNumber) >= 34118) && (abs(runNumber) <= 34127))
        {
          m_cmEnergy = 3.940;
        }
        else if((abs(runNumber) >= 34128) && (abs(runNumber) <= 34134))
        {
          m_cmEnergy = 3.945;
        }
        else if((abs(runNumber) >= 34135) && (abs(runNumber) <= 34141))
        {
          m_cmEnergy = 3.950;
        }
        else if((abs(runNumber) >= 34142) && (abs(runNumber) <= 34150))
        {
          m_cmEnergy = 3.955;
        }
        else if((abs(runNumber) >= 34151) && (abs(runNumber) <= 34160))
        {
          m_cmEnergy = 3.960;
        }
        else if((abs(runNumber) >= 34161) && (abs(runNumber) <= 34167))
        {
          m_cmEnergy = 3.965;
        }
        else if((abs(runNumber) >= 34175) && (abs(runNumber) <= 34183))
        {
          m_cmEnergy = 3.970;
        }
        else if((abs(runNumber) >= 34184) && (abs(runNumber) <= 34190))
        {
          m_cmEnergy = 3.975;
        }
        else if((abs(runNumber) >= 34191) && (abs(runNumber) <= 34196))
        {
          m_cmEnergy = 3.980;
        }
        else if((abs(runNumber) >= 34197) && (abs(runNumber) <= 34202))
        {
          m_cmEnergy = 3.985;
        }
        else if((abs(runNumber) >= 34203) && (abs(runNumber) <= 34210))
        {
          m_cmEnergy = 3.990;
        }
        else if((abs(runNumber) >= 34211) && (abs(runNumber) <= 34220))
        {
          m_cmEnergy = 3.995;
        }
        else if((abs(runNumber) >= 34221) && (abs(runNumber) <= 34230))
        {
          m_cmEnergy = 4.000;
        }
        else if((abs(runNumber) >= 34231) && (abs(runNumber) <= 34239))
        {
          m_cmEnergy = 4.005;
        }
        else if((abs(runNumber) >= 34240) && (abs(runNumber) <= 34245))
        {
          m_cmEnergy = 4.010;
        }
        else if((abs(runNumber) >= 34246) && (abs(runNumber) <= 34252))
        {
          m_cmEnergy = 4.012;
        }
        else if((abs(runNumber) >= 34253) && (abs(runNumber) <= 34257))
        {
          m_cmEnergy = 4.014;
        }
        else if((abs(runNumber) >= 34258) && (abs(runNumber) <= 34265))
        {
          m_cmEnergy = 4.016;
        }
        else if((abs(runNumber) >= 34266) && (abs(runNumber) <= 34271))
        {
          m_cmEnergy = 4.018;
        }
        else if((abs(runNumber) >= 34272) && (abs(runNumber) <= 34276))
        {
          m_cmEnergy = 4.020;
        }
        else if((abs(runNumber) >= 34277) && (abs(runNumber) <= 34281))
        {
          m_cmEnergy = 4.025;
        }
        else if((abs(runNumber) >= 34282) && (abs(runNumber) <= 34297))
        {
          m_cmEnergy = 4.030;
        }
        else if((abs(runNumber) >= 34314) && (abs(runNumber) <= 34320))
        {
          m_cmEnergy = 4.035;
        }
        else if((abs(runNumber) >= 34321) && (abs(runNumber) <= 34327))
        {
          m_cmEnergy = 4.040;
        }
        else if((abs(runNumber) >= 34328) && (abs(runNumber) <= 34335))
        {
          m_cmEnergy = 4.050;
        }
        else if((abs(runNumber) >= 34339) && (abs(runNumber) <= 34345))
        {
          m_cmEnergy = 4.055;
        }
        else if((abs(runNumber) >= 34346) && (abs(runNumber) <= 34350))
        {
          m_cmEnergy = 4.060;
        }
        else if((abs(runNumber) >= 34351) && (abs(runNumber) <= 34358))
        {
          m_cmEnergy = 4.065;
        }
        else if((abs(runNumber) >= 34359) && (abs(runNumber) <= 34367))
        {
          m_cmEnergy = 4.070;
        }
        else if((abs(runNumber) >= 34368) && (abs(runNumber) <= 34373))
        {
          m_cmEnergy = 4.080;
        }
        else if((abs(runNumber) >= 34374) && (abs(runNumber) <= 34381))
        {
          m_cmEnergy = 4.090;
        }
        else if((abs(runNumber) >= 34382) && (abs(runNumber) <= 34389))
        {
          m_cmEnergy = 4.100;
        }
        else if((abs(runNumber) >= 34390) && (abs(runNumber) <= 34396))
        {
          m_cmEnergy = 4.110;
        }
        else if((abs(runNumber) >= 34397) && (abs(runNumber) <= 34403))
        {
          m_cmEnergy = 4.120;
        }
        else if((abs(runNumber) >= 34404) && (abs(runNumber) <= 34411))
        {
          m_cmEnergy = 4.130;
        }
        else if((abs(runNumber) >= 34412) && (abs(runNumber) <= 34417))
        {
          m_cmEnergy = 4.140;
        }
        else if((abs(runNumber) >= 34418) && (abs(runNumber) <= 34427))
        {
          m_cmEnergy = 4.145;
        }
        else if((abs(runNumber) >= 34428) && (abs(runNumber) <= 34436))
        {
          m_cmEnergy = 4.150;
        }
        else if((abs(runNumber) >= 34437) && (abs(runNumber) <= 34446))
        {
          m_cmEnergy = 4.160;
        }
        else if((abs(runNumber) >= 34447) && (abs(runNumber) <= 34460))
        {
          m_cmEnergy = 4.170;
        }
        else if((abs(runNumber) >= 34478) && (abs(runNumber) <= 34485))
        {
          m_cmEnergy = 4.180;
        }
        else if((abs(runNumber) >= 34486) && (abs(runNumber) <= 34493))
        {
          m_cmEnergy = 4.190;
        }
        else if((abs(runNumber) >= 34494) && (abs(runNumber) <= 34502))
        {
          m_cmEnergy = 4.195;
        }
        else if((abs(runNumber) >= 34503) && (abs(runNumber) <= 34511))
        {
          m_cmEnergy = 4.200;
        }
        else if((abs(runNumber) >= 34512) && (abs(runNumber) <= 34526))
        {
          m_cmEnergy = 4.203;
        }
        else if((abs(runNumber) >= 34527) && (abs(runNumber) <= 34540))
        {
          m_cmEnergy = 4.206;
        }
        else if((abs(runNumber) >= 34541) && (abs(runNumber) <= 34554))
        {
          m_cmEnergy = 4.210;
        }
        else if((abs(runNumber) >= 34555) && (abs(runNumber) <= 34563))
        {
          m_cmEnergy = 4.215;
        }
        else if((abs(runNumber) >= 34564) && (abs(runNumber) <= 34573))
        {
          m_cmEnergy = 4.220;
        }
        else if((abs(runNumber) >= 34574) && (abs(runNumber) <= 34584))
        {
          m_cmEnergy = 4.225;
        }
        else if((abs(runNumber) >= 34585) && (abs(runNumber) <= 34592))
        {
          m_cmEnergy = 4.230;
        }
        else if((abs(runNumber) >= 34593) && (abs(runNumber) <= 34602))
        {
          m_cmEnergy = 4.235;
        }
        else if((abs(runNumber) >= 34603) && (abs(runNumber) <= 34612))
        {
          m_cmEnergy = 4.240;
        }
        else if((abs(runNumber) >= 34613) && (abs(runNumber) <= 34622))
        {
          m_cmEnergy = 4.243;
        }
        else if((abs(runNumber) >= 34623) && (abs(runNumber) <= 34633))
        {
          m_cmEnergy = 4.245;
        }
        else if((abs(runNumber) >= 34634) && (abs(runNumber) <= 34641))
        {
          m_cmEnergy = 4.248;
        }
        else if((abs(runNumber) >= 34642) && (abs(runNumber) <= 34651))
        {
          m_cmEnergy = 4.250;
        }
        else if((abs(runNumber) >= 34652) && (abs(runNumber) <= 34660))
        {
          m_cmEnergy = 4.255;
        }
        else if((abs(runNumber) >= 34661) && (abs(runNumber) <= 34673))
        {
          m_cmEnergy = 4.260;
        }
        else if((abs(runNumber) >= 34674) && (abs(runNumber) <= 34684))
        {
          m_cmEnergy = 4.265;
        }
        else if((abs(runNumber) >= 34685) && (abs(runNumber) <= 34694))
        {
          m_cmEnergy = 4.270;
        }
        else if((abs(runNumber) >= 34695) && (abs(runNumber) <= 34704))
        {
          m_cmEnergy = 4.275;
        }
        else if((abs(runNumber) >= 34705) && (abs(runNumber) <= 34718))
        {
          m_cmEnergy = 4.280;
        }
        else if((abs(runNumber) >= 34719) && (abs(runNumber) <= 34728))
        {
          m_cmEnergy = 4.285;
        }
        else if((abs(runNumber) >= 34729) && (abs(runNumber) <= 34739))
        {
          m_cmEnergy = 4.290;
        }
        else if((abs(runNumber) >= 34740) && (abs(runNumber) <= 34753))
        {
          m_cmEnergy = 4.300;
        }
        else if((abs(runNumber) >= 34754) && (abs(runNumber) <= 34762))
        {
          m_cmEnergy = 4.310;
        }
        else if((abs(runNumber) >= 34763) && (abs(runNumber) <= 34776))
        {
          m_cmEnergy = 4.320;
        }
        else if((abs(runNumber) >= 34777) && (abs(runNumber) <= 34784))
        {
          m_cmEnergy = 4.330;
        }
        else if((abs(runNumber) >= 34785) && (abs(runNumber) <= 34793))
        {
          m_cmEnergy = 4.340;
        }
        else if((abs(runNumber) >= 34794) && (abs(runNumber) <= 34803))
        {
          m_cmEnergy = 4.350;
        }
        else if((abs(runNumber) >= 34804) && (abs(runNumber) <= 34811))
        {
          m_cmEnergy = 4.360;
        }
        else if((abs(runNumber) >= 34812) && (abs(runNumber) <= 34824))
        {
          m_cmEnergy = 4.370;
        }
        else if((abs(runNumber) >= 34825) && (abs(runNumber) <= 34836))
        {
          m_cmEnergy = 4.380;
        }
        else if((abs(runNumber) >= 34837) && (abs(runNumber) <= 34847))
        {
          m_cmEnergy = 4.390;
        }
        else if((abs(runNumber) >= 34848) && (abs(runNumber) <= 34860))
        {
          m_cmEnergy = 4.395;
        }
        else if((abs(runNumber) >= 34861) && (abs(runNumber) <= 34868))
        {
          m_cmEnergy = 4.400;
        }
        else if((abs(runNumber) >= 34869) && (abs(runNumber) <= 34881))
        {
          m_cmEnergy = 4.410;
        }
        else if((abs(runNumber) >= 34882) && (abs(runNumber) <= 34890))
        {
          m_cmEnergy = 4.420;
        }
        else if((abs(runNumber) >= 34891) && (abs(runNumber) <= 34899))
        {
          m_cmEnergy = 4.425;
        }
        else if((abs(runNumber) >= 34900) && (abs(runNumber) <= 34912))
        {
          m_cmEnergy = 4.430;
        }
        else if((abs(runNumber) >= 34913) && (abs(runNumber) <= 34925))
        {
          m_cmEnergy = 4.440;
        }
        else if((abs(runNumber) >= 34926) && (abs(runNumber) <= 34935))
        {
          m_cmEnergy = 4.450;
        }
        else if((abs(runNumber) >= 34936) && (abs(runNumber) <= 34946))
        {
          m_cmEnergy = 4.460;
        }
        else if((abs(runNumber) >= 34947) && (abs(runNumber) <= 34957))
        {
          m_cmEnergy = 4.480;
        }
        else if((abs(runNumber) >= 34958) && (abs(runNumber) <= 34967))
        {
          m_cmEnergy = 4.500;
        }
        else if((abs(runNumber) >= 34968) && (abs(runNumber) <= 34981))
        {
          m_cmEnergy = 4.520;
        }
        else if((abs(runNumber) >= 34982) && (abs(runNumber) <= 35009))
        {
          m_cmEnergy = 4.540;
        }
        else if((abs(runNumber) >= 35010) && (abs(runNumber) <= 35026))
        {
          m_cmEnergy = 4.550;
        }
        else if((abs(runNumber) >= 35027) && (abs(runNumber) <= 35040))
        {
          m_cmEnergy = 4.560;
        }
        else if((abs(runNumber) >= 35041) && (abs(runNumber) <= 35059))
        {
          m_cmEnergy = 4.570;
        }
        else if((abs(runNumber) >= 35060) && (abs(runNumber) <= 35085))
        {
          m_cmEnergy = 4.580;
        }
        else if((abs(runNumber) >= 35099) && (abs(runNumber) <= 35118))
        {
          m_cmEnergy = 4.590;
        }
        else
        {
          return StatusCode::SUCCESS;
        }
      }
      // R SCAN 2015
      else if((abs(runNumber) >= 39355) && (abs(runNumber) <= 41958))
      {
        if((abs(runNumber) >= 39355) && (abs(runNumber) <= 39618)) { m_cmEnergy = 3.0800; }
        else if((abs(runNumber) >= 39711) && (abs(runNumber) <= 39738))
        {
          m_cmEnergy = 3.0200;
        }
        else if((abs(runNumber) >= 39680) && (abs(runNumber) <= 39710))
        {
          m_cmEnergy = 3.0000;
        }
        else if((abs(runNumber) >= 39651) && (abs(runNumber) <= 39679))
        {
          m_cmEnergy = 2.9810;
        }
        else if((abs(runNumber) >= 39619) && (abs(runNumber) <= 39650))
        {
          m_cmEnergy = 2.9500;
        }
        else if((abs(runNumber) >= 39775) && (abs(runNumber) <= 40069))
        {
          m_cmEnergy = 2.9000;
        }
        else if((abs(runNumber) >= 40128) && (abs(runNumber) <= 40296))
        {
          m_cmEnergy = 2.6444;
        }
        else if((abs(runNumber) >= 40300) && (abs(runNumber) <= 40435))
        {
          m_cmEnergy = 2.6464;
        }
        else if((abs(runNumber) >= 40436) && (abs(runNumber) <= 40439))
        {
          m_cmEnergy = 2.7000;
        }
        else if((abs(runNumber) >= 40440) && (abs(runNumber) <= 40443))
        {
          m_cmEnergy = 2.8000;
        }
        else if((abs(runNumber) >= 40459) && (abs(runNumber) <= 40769))
        {
          m_cmEnergy = 2.3960;
        }
        else if((abs(runNumber) >= 40771) && (abs(runNumber) <= 40776))
        {
          m_cmEnergy = 2.5000;
        }
        else if((abs(runNumber) >= 40806) && (abs(runNumber) <= 40951))
        {
          m_cmEnergy = 2.3864;
        }
        else if((abs(runNumber) >= 40989) && (abs(runNumber) <= 41121))
        {
          m_cmEnergy = 2.2000;
        }
        else if((abs(runNumber) >= 41122) && (abs(runNumber) <= 41239))
        {
          m_cmEnergy = 2.2324;
        }
        else if((abs(runNumber) >= 41240) && (abs(runNumber) <= 41411))
        {
          m_cmEnergy = 2.3094;
        }
        else if((abs(runNumber) >= 41416) && (abs(runNumber) <= 41532))
        {
          m_cmEnergy = 2.1750;
        }
        else if((abs(runNumber) >= 41533) && (abs(runNumber) <= 41570))
        {
          m_cmEnergy = 2.1500;
        }
        else if((abs(runNumber) >= 41588) && (abs(runNumber) <= 41727))
        {
          m_cmEnergy = 2.1000;
        }
        else if((abs(runNumber) >= 41729) && (abs(runNumber) <= 41909))
        {
          m_cmEnergy = 2.0000;
        }
        else if((abs(runNumber) >= 41911) && (abs(runNumber) <= 41958))
        {
          m_cmEnergy = 2.0500;
        }
        else
        {
          return StatusCode::SUCCESS;
        }
      }
      // Y(2175) 2015
      else if((abs(runNumber) >= 42004) && (abs(runNumber) <= 43253))
      {
        m_cmEnergy = 2.125;
      }
      // 4180 from 2016 (probably should use database eventually)
      else if((abs(runNumber) >= 43716) && (abs(runNumber) <= 47066))
      {
        m_cmEnergy = 4.180;
      }
      // 2017 data
      else if((abs(runNumber) >= 47543) && (abs(runNumber) <= 48170))
      {
        m_cmEnergy = 4.190;
      }
      else if((abs(runNumber) >= 48171) && (abs(runNumber) <= 48713))
      {
        m_cmEnergy = 4.200;
      }
      else if((abs(runNumber) >= 48714) && (abs(runNumber) <= 49239))
      {
        m_cmEnergy = 4.210;
      }
      else if((abs(runNumber) >= 49270) && (abs(runNumber) <= 49787))
      {
        m_cmEnergy = 4.220;
      }
      else if((abs(runNumber) >= 49788) && (abs(runNumber) <= 50254))
      {
        m_cmEnergy = 4.237;
      }
      else if((abs(runNumber) >= 50255) && (abs(runNumber) <= 50793))
      {
        m_cmEnergy = 4.246;
      }
      else if((abs(runNumber) >= 50796) && (abs(runNumber) <= 51302))
      {
        m_cmEnergy = 4.270;
      }
      else if((abs(runNumber) >= 51303) && (abs(runNumber) <= 60000))
      {
        m_cmEnergy = 4.280;
      }

      else
      {
        cout << "FSFILTER ERROR:  could not determine the beam energy for run " << runNumber
             << endl;
        exit(0);
      }
    }
  }

  double mpsi     = m_cmEnergy;
  double me       = 0.000511;
  double costheta = cos(m_crossingAngle);
  double tantheta = tan(m_crossingAngle);
  double epsilon  = 4.0 * me * me / mpsi / mpsi;

  m_pInitial = HepLorentzVector(mpsi * tantheta * sqrt(1.0 - epsilon), 0.0, 0.0,
                                mpsi / costheta * sqrt(1.0 - epsilon * (1 - costheta * costheta)));

  //double beamEnergy = m_pInitial.e()/2.0;

  // event information

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  if(m_debug)
  {
    cout << "\tRun      = " << runNumber << endl;
    cout << "\tEvent    = " << eventNumber << endl;
    cout << "\tNTracks  = " << evtRecEvent->totalCharged() << endl;
    cout << "\tNShowers = " << evtRecEvent->totalNeutral() << endl;
  }

  //********************************************************************
  //
  //   PARSE TRUTH INFORMATION AND WRITE TO THE GENERATOR TREES
  //
  //********************************************************************

  if(runNumber < 0)
  {

    // extract truth information from the framework and set up a MCTruthHelper object

    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), EventModel::MC::McParticleCol);
    m_mcTruthHelper = new MCTruthHelper(mcParticleCol);
    if(m_mcTruthHelper->MCParticleList().size() == 0)
    {
      delete m_mcTruthHelper;
      return StatusCode::SUCCESS;
    }
    if(m_printTruthInformation) { m_mcTruthHelper->printInformation(); }

    // write to the mode-independent generator tree

    if(m_writeNTGen)
    {
      m_ntGEN->fillEvent(eventHeader, evtRecEvent, m_pInitial.e());
      m_ntGEN->fillMCTruth(m_mcTruthHelper);
      m_ntGEN->write();
    }

    // write to the mode-dependent generator trees

    if(m_writeNTGenCode)
    {
      for(unsigned int ifs = 0; ifs < m_FSInfoVector.size(); ifs++)
      {
        FSInfo* fs = m_FSInfoVector[ifs];
        if(fs->exclusive() && (m_mcTruthHelper->MCDecayCode1() == fs->decayCode1()) &&
           (m_mcTruthHelper->MCDecayCode2() == fs->decayCode2()) &&
           (m_mcTruthHelper->MCExtras() == 0))
        {
          NTupleHelper* nt = fs->NTGen();
          nt->fillEvent(eventHeader, evtRecEvent, m_pInitial.e());
          nt->fillMCTruth(m_mcTruthHelper, fs, "");
          nt->fillMCTruth(m_mcTruthHelper, fs);
          nt->write();
        }
      }
    }

    // does this generated event make sense???

    double mcTotalEnergyInitial = m_mcTruthHelper->MCTotalEnergyInitial();
    double mcTotalEnergyFinal   = m_mcTruthHelper->MCTotalEnergyFinal();
    if((mcTotalEnergyFinal > 1.1 * m_pInitial.e() &&
        abs(2.0 * m_pInitial.e() - mcTotalEnergyFinal) > 0.1) ||
       (abs(mcTotalEnergyInitial - mcTotalEnergyFinal) > 0.2))
    {
      cout << "FSFilter WARNING for Truth Parsing" << endl;
      cout << "RUN NUMBER = " << runNumber << endl;
      cout << "EVENT NUMBER = " << eventNumber << endl;
      cout << "FLAG 1 = " << eventHeader->flag1() << endl;
      cout << "FLAG 2 = " << eventHeader->flag2() << endl;
      m_mcTruthHelper->printInformation();
    }
  }

  // track collection

  SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);

  //********************************************************************
  //
  //   MAKE INITIAL REQUIREMENTS TO SPEED PROCESSING
  //
  //********************************************************************

  // check the total number of showers

  if(evtRecEvent->totalNeutral() > m_maxShowers) return StatusCode::SUCCESS;

  // do a quick check to see if there is a J/psi candidate

  if(m_fastJpsi)
  {

    bool okay = false;

    vector<FSParticle*> mupList;
    vector<FSParticle*> mumList;
    vector<FSParticle*> epList;
    vector<FSParticle*> emList;

    int trackIndex = -1;
    for(EvtRecTrackIterator iTrk = evtRecTrackCol->begin(); iTrk != evtRecTrackCol->end(); iTrk++)
    {
      trackIndex++;
      if((*iTrk)->isMdcKalTrackValid())
      {
        RecMdcKalTrack* mdcKalTrack = (*iTrk)->mdcKalTrack();
        if(mdcKalTrack->charge() > 0)
          epList.push_back(new FSParticle(*iTrk, trackIndex, "e+", m_yuping));
        if(mdcKalTrack->charge() < 0)
          emList.push_back(new FSParticle(*iTrk, trackIndex, "e-", m_yuping));
        if(mdcKalTrack->charge() > 0)
          mupList.push_back(new FSParticle(*iTrk, trackIndex, "mu+", m_yuping));
        if(mdcKalTrack->charge() < 0)
          mumList.push_back(new FSParticle(*iTrk, trackIndex, "mu-", m_yuping));
      }
    }

    for(unsigned int ip = 0; ip < epList.size() && !okay; ip++)
    {
      for(unsigned int im = 0; im < emList.size() && !okay; im++)
      {
        if(fabs((epList[ip]->rawFourMomentum() + emList[im]->rawFourMomentum()).m() - 3.1) < 0.6)
          okay = true;
      }
    }

    for(unsigned int ip = 0; ip < mupList.size() && !okay; ip++)
    {
      for(unsigned int im = 0; im < mumList.size() && !okay; im++)
      {
        if(fabs((mupList[ip]->rawFourMomentum() + mumList[im]->rawFourMomentum()).m() - 3.1) < 0.6)
          okay = true;
      }
    }

    for(unsigned int i = 0; i < epList.size(); i++) { delete epList[i]; }
    for(unsigned int i = 0; i < emList.size(); i++) { delete emList[i]; }
    for(unsigned int i = 0; i < mupList.size(); i++) { delete mupList[i]; }
    for(unsigned int i = 0; i < mumList.size(); i++) { delete mumList[i]; }

    if(!okay) return StatusCode::SUCCESS;
  }

  //********************************************************************
  //
  //   EXTRACT MORE INFORMATION FROM THE FRAMEWORK
  //
  //********************************************************************

  // pi0 --> gamma gamma

  SmartDataPtr<EvtRecPi0Col> evtRecPi0Col(eventSvc(), EventModel::EvtRec::EvtRecPi0Col);

  // eta --> gamma gamma

  SmartDataPtr<EvtRecEtaToGGCol> evtRecEtaToGGCol(eventSvc(), EventModel::EvtRec::EvtRecEtaToGGCol);

  // Ks --> pi+ pi- and Lambda --> p+ pi- and ALambda --> p- pi+

  SmartDataPtr<EvtRecVeeVertexCol> evtRecVeeVertexCol(eventSvc(),
                                                      EventModel::EvtRec::EvtRecVeeVertexCol);

  // beam spot

  VertexParameter beamSpot;
  HepPoint3D      vBeamSpot(0., 0., 0.);
  HepSymMatrix    evBeamSpot(3, 0);

  if(!m_bypassVertexDB)
  {
    IVertexDbSvc* vtxsvc;
    Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
    if(vtxsvc->isVertexValid())
    {
      double* dbv = vtxsvc->PrimaryVertex();
      double* vv  = vtxsvc->SigmaPrimaryVertex();
      for(unsigned int ivx = 0; ivx < 3; ivx++)
      {
        vBeamSpot[ivx]       = dbv[ivx];
        evBeamSpot[ivx][ivx] = vv[ivx] * vv[ivx];
      }
    }
    else
    {
      cout << "FSFILTER ERROR:  Could not find a vertex from VertexDbSvc" << endl;
      exit(0);
    }
  }

  beamSpot.setVx(vBeamSpot);
  beamSpot.setEvx(evBeamSpot);

  //********************************************************************
  //
  //   CREATE LISTS OF PARTICLES
  //
  //********************************************************************

  vector<FSParticle*> lambdaList;
  vector<FSParticle*> alambdaList;
  vector<FSParticle*> mupList;
  vector<FSParticle*> mumList;
  vector<FSParticle*> epList;
  vector<FSParticle*> emList;
  vector<FSParticle*> ppList;
  vector<FSParticle*> pmList;
  vector<FSParticle*> etaList;
  vector<FSParticle*> gammaList;
  vector<FSParticle*> kpList;
  vector<FSParticle*> kmList;
  vector<FSParticle*> ksList;
  vector<FSParticle*> pipList;
  vector<FSParticle*> pimList;
  vector<FSParticle*> pi0List;

  // loop over the "track" collection

  int trackIndex = -1;

  for(EvtRecTrackIterator iTrk = evtRecTrackCol->begin(); iTrk != evtRecTrackCol->end(); iTrk++)
  {
    trackIndex++;

    // get photons

    if(m_findGamma)
    {
      if((*iTrk)->isEmcShowerValid())
      {
        RecEmcShower* emcShower = (*iTrk)->emcShower();
        if(((emcShower->energy() > 0.025) && (emcShower->time() > 0) && (emcShower->time() < 14) &&
            (((abs(cos(emcShower->theta())) < 0.80) && (emcShower->energy() > 0.025)) ||
             ((abs(cos(emcShower->theta())) < 0.92) && (abs(cos(emcShower->theta())) > 0.86) &&
              (emcShower->energy() > 0.050)))) ||
           m_neutralStudies)
        { gammaList.push_back(new FSParticle(*iTrk, "gamma")); } }
    }

    // get tracks

    if(((m_findPi) || (m_findK) || (m_findP) || (m_findE) || (m_findMu)) &&
       (*iTrk)->isMdcKalTrackValid())
    {
      RecMdcKalTrack* mdcKalTrack = (*iTrk)->mdcKalTrack();

      // set up pid (but only make very rough pid cuts)

      ParticleID* pid = ParticleID::instance();
      pid->init();
      pid->setMethod(pid->methodProbability());
      pid->setRecTrack(*iTrk);
      pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2());
      pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton() | pid->onlyElectron() |
                    pid->onlyMuon());
      pid->calculate();

      // get pions

      if(m_findPi)
      {
        if((pid->IsPidInfoValid() && pid->probPion() > 1.0e-5) || m_trackStudies)
        {
          if(mdcKalTrack->charge() > 0)
            pipList.push_back(new FSParticle(*iTrk, trackIndex, "pi+", m_yuping));
          if(mdcKalTrack->charge() < 0)
            pimList.push_back(new FSParticle(*iTrk, trackIndex, "pi-", m_yuping));
        }
      }

      // get kaons

      if(m_findK)
      {
        if((pid->IsPidInfoValid() && pid->probKaon() > 1.0e-5) || m_trackStudies)
        {
          if((!m_extraKaonPID) || (pid->IsPidInfoValid() && (pid->probKaon() > 1.0e-5) &&
                                   (pid->probKaon() > pid->probPion())))
          {
            if(mdcKalTrack->charge() > 0)
              kpList.push_back(new FSParticle(*iTrk, trackIndex, "K+", m_yuping));
            if(mdcKalTrack->charge() < 0)
              kmList.push_back(new FSParticle(*iTrk, trackIndex, "K-", m_yuping));
          }
        }
      }

      // get protons

      if(m_findP)
      {
        if((pid->IsPidInfoValid() && pid->probProton() > 1.0e-5) || m_trackStudies)
        {
          if(mdcKalTrack->charge() > 0)
            ppList.push_back(new FSParticle(*iTrk, trackIndex, "p+", m_yuping));
          if(mdcKalTrack->charge() < 0)
            pmList.push_back(new FSParticle(*iTrk, trackIndex, "p-", m_yuping));
        }
      }

      // get electrons

      if(m_findE)
      {
        if((pid->IsPidInfoValid() && pid->probElectron() > 1.0e-5) || m_trackStudies)
        {
          if(mdcKalTrack->charge() > 0)
            epList.push_back(new FSParticle(*iTrk, trackIndex, "e+", m_yuping));
          if(mdcKalTrack->charge() < 0)
            emList.push_back(new FSParticle(*iTrk, trackIndex, "e-", m_yuping));
        }
      }

      // get muons

      if(m_findMu)
      {
        if((pid->IsPidInfoValid() && pid->probMuon() > 1.0e-5) || m_trackStudies)
        {
          if(mdcKalTrack->charge() > 0)
            mupList.push_back(new FSParticle(*iTrk, trackIndex, "mu+", m_yuping));
          if(mdcKalTrack->charge() < 0)
            mumList.push_back(new FSParticle(*iTrk, trackIndex, "mu-", m_yuping));
        }
      }
    }
  }

  // get etas

  if(m_findEta)
  {
    for(EvtRecEtaToGGCol::iterator iEta = evtRecEtaToGGCol->begin();
        iEta != evtRecEtaToGGCol->end(); iEta++)
    {
      if((((*iEta)->chisq() < 2500) && ((*iEta)->unconMass() > 0.40) &&
          ((*iEta)->unconMass() < 0.70)) ||
         m_etaStudies)
      {
        EvtRecTrack*  lo       = const_cast<EvtRecTrack*>((*iEta)->loEnGamma());
        RecEmcShower* loShower = lo->emcShower();
        if(((loShower->energy() > 0.025) && (loShower->time() > 0) && (loShower->time() < 14) &&
            (((abs(cos(loShower->theta())) < 0.80) && (loShower->energy() > 0.025)) ||
             ((abs(cos(loShower->theta())) < 0.92) && (abs(cos(loShower->theta())) > 0.86) &&
              (loShower->energy() > 0.050)))) ||
           m_neutralStudies)
        {
          EvtRecTrack*  hi       = const_cast<EvtRecTrack*>((*iEta)->hiEnGamma());
          RecEmcShower* hiShower = hi->emcShower();
          if(((hiShower->energy() > 0.025) && (hiShower->time() > 0) && (hiShower->time() < 14) &&
              (((abs(cos(hiShower->theta())) < 0.80) && (hiShower->energy() > 0.025)) ||
               ((abs(cos(hiShower->theta())) < 0.92) && (abs(cos(hiShower->theta())) > 0.86) &&
                (hiShower->energy() > 0.050)))) ||
             m_neutralStudies)
          { etaList.push_back(new FSParticle(*iEta, "eta")); } }
      }
    }
  }

  // get pi0s

  //if (m_findPi0){
  for(EvtRecPi0Col::iterator iPi0 = evtRecPi0Col->begin(); iPi0 != evtRecPi0Col->end(); iPi0++)
  {
    if((((*iPi0)->chisq() < 2500) && ((*iPi0)->unconMass() > 0.107) &&
        ((*iPi0)->unconMass() < 0.163)) ||
       m_pi0Studies)
    {
      EvtRecTrack*  lo       = const_cast<EvtRecTrack*>((*iPi0)->loEnGamma());
      RecEmcShower* loShower = lo->emcShower();
      if(((loShower->energy() > 0.025) && (loShower->time() > 0) && (loShower->time() < 14) &&
          (((abs(cos(loShower->theta())) < 0.80) && (loShower->energy() > 0.025)) ||
           ((abs(cos(loShower->theta())) < 0.92) && (abs(cos(loShower->theta())) > 0.86) &&
            (loShower->energy() > 0.050)))) ||
         m_neutralStudies)
      {
        EvtRecTrack*  hi       = const_cast<EvtRecTrack*>((*iPi0)->hiEnGamma());
        RecEmcShower* hiShower = hi->emcShower();
        if(((hiShower->energy() > 0.025) && (hiShower->time() > 0) && (hiShower->time() < 14) &&
            (((abs(cos(hiShower->theta())) < 0.80) && (hiShower->energy() > 0.025)) ||
             ((abs(cos(hiShower->theta())) < 0.92) && (abs(cos(hiShower->theta())) > 0.86) &&
              (hiShower->energy() > 0.050)))) ||
           m_neutralStudies)
        { pi0List.push_back(new FSParticle(*iPi0, "pi0")); } }
    }
  }
  //}

  // get kshorts
  //   [[ vertexId is the pdgID ]]

  if(m_findKs)
  {
    for(EvtRecVeeVertexCol::iterator iKs = evtRecVeeVertexCol->begin();
        iKs != evtRecVeeVertexCol->end(); iKs++)
    {
      if((*iKs)->vertexId() == 310)
      {
        if((((*iKs)->mass() > 0.471) && ((*iKs)->mass() < 0.524) && ((*iKs)->chi2() < 100)) ||
           m_veeStudies)
        { ksList.push_back(new FSParticle(*iKs, "Ks")); } }
    }
  }

  // get Lambdas

  if(m_findLambda)
  {
    for(EvtRecVeeVertexCol::iterator iL = evtRecVeeVertexCol->begin();
        iL != evtRecVeeVertexCol->end(); iL++)
    {
      if((*iL)->vertexId() == 3122)
      {
        if((((*iL)->mass() > 1.100) && ((*iL)->mass() < 1.130) && ((*iL)->chi2() < 100)) ||
           m_veeStudies)
        { lambdaList.push_back(new FSParticle(*iL, "Lambda")); } }
      if((*iL)->vertexId() == -3122)
      {
        if((((*iL)->mass() > 1.100) && ((*iL)->mass() < 1.130) && ((*iL)->chi2() < 100)) ||
           m_veeStudies)
        { alambdaList.push_back(new FSParticle(*iL, "ALambda")); } }
    }
  }

  //********************************************************************
  //
  //   LOOP OVER FINAL STATES
  //
  //********************************************************************

  for(unsigned int ifs = 0; ifs < m_FSInfoVector.size(); ifs++)
  {

    // get information about this final state

    FSInfo*        fsinfo        = m_FSInfoVector[ifs];
    vector<string> particleNames = fsinfo->particleNames();
    NTupleHelper*  NT            = fsinfo->NT();
    if(m_debug) { cout << "\t\tFINAL STATE = " << fsinfo->FSName() << endl; }

    // check number of tracks

    if(fsinfo->exclusive())
    {
      if(evtRecEvent->totalCharged() > fsinfo->nChargedParticles() + m_maxExtraTracks) continue;
      if(evtRecEvent->totalCharged() < fsinfo->nChargedParticles()) continue;
    }

    //********************************************************************
    //
    //   PUT TOGETHER ALL COMBINATIONS OF PARTICLES FOR THIS FINAL STATE
    //
    //********************************************************************

    vector<vector<FSParticle*> > particleCombinations;

    for(unsigned int ifsp = 0; ifsp < particleNames.size(); ifsp++)
    {
      vector<FSParticle*> pList;
      if(particleNames[ifsp] == "Lambda")
        pList = lambdaList;
      else if(particleNames[ifsp] == "ALambda")
        pList = alambdaList;
      else if(particleNames[ifsp] == "mu+")
        pList = mupList;
      else if(particleNames[ifsp] == "mu-")
        pList = mumList;
      else if(particleNames[ifsp] == "e+")
        pList = epList;
      else if(particleNames[ifsp] == "e-")
        pList = emList;
      else if(particleNames[ifsp] == "p+")
        pList = ppList;
      else if(particleNames[ifsp] == "p-")
        pList = pmList;
      else if(particleNames[ifsp] == "eta")
        pList = etaList;
      else if(particleNames[ifsp] == "gamma")
        pList = gammaList;
      else if(particleNames[ifsp] == "K+")
        pList = kpList;
      else if(particleNames[ifsp] == "K-")
        pList = kmList;
      else if(particleNames[ifsp] == "Ks")
        pList = ksList;
      else if(particleNames[ifsp] == "pi+")
        pList = pipList;
      else if(particleNames[ifsp] == "pi-")
        pList = pimList;
      else if(particleNames[ifsp] == "pi0")
        pList = pi0List;
      vector<vector<FSParticle*> > particleCombinationsTemp = particleCombinations;
      particleCombinations.clear();
      if(pList.size() == 0) break;
      for(unsigned int ipl = 0; ipl < pList.size(); ipl++)
      {
        if(ifsp == 0)
        {
          vector<FSParticle*> combo;
          combo.push_back(pList[ipl]);
          if(checkCombination(combo, (combo.size() == particleNames.size()), fsinfo->inclusive()))
            particleCombinations.push_back(combo);
        }
        else
        {
          for(unsigned int itc = 0; itc < particleCombinationsTemp.size(); itc++)
          {
            vector<FSParticle*> combo     = particleCombinationsTemp[itc];
            bool                duplicate = false;
            for(unsigned int ic = 0; ic < combo.size(); ic++)
            {
              if(pList[ipl]->duplicate(combo[ic]))
              {
                duplicate = true;
                continue;
              }
            }
            if(!duplicate)
            {
              combo.push_back(pList[ipl]);
              if(checkCombination(combo, (combo.size() == particleNames.size()),
                                  fsinfo->inclusive()))
                particleCombinations.push_back(combo);
              if(particleCombinations.size() > 1000000) break;
            }
          }
        }
        if(particleCombinations.size() > 1000000) break;
      }
      if(particleCombinations.size() > 1000000) break;
    }

    fsinfo->setParticleCombinations(particleCombinations);

    if(m_debug)
    {
      cout << "\t\t\t\tnumber of combinations = " << fsinfo->particleCombinations().size() << endl;
    }

    // skip events with an unreasonable number of combinations

    if(fsinfo->particleCombinations().size() > 1000000)
    {
      cout << "FSFilter WARNING:  MORE THAN 1 MILLION COMBINATIONS, SKIPPING..." << endl;
      cout << "\tRun      = " << runNumber;
      cout << ", Event    = " << eventNumber;
      cout << ", NTracks  = " << evtRecEvent->totalCharged();
      cout << ", NShowers = " << evtRecEvent->totalNeutral() << endl;
      cout << "\t\tFINAL STATE = " << fsinfo->FSName() << endl;
      continue;
    }

    //********************************************************************
    //
    //   LOOP OVER PARTICLE COMBINATIONS
    //
    //********************************************************************

    for(unsigned int i = 0; i < fsinfo->particleCombinations().size(); i++)
    {

      vector<FSParticle*> combo = fsinfo->particleCombinations()[i];

      //********************************************************************
      //
      //   MAKE CUTS ON INTERMEDIATE PARTICLE COMBINATIONS (RAW 4-VECTORS)
      //
      //********************************************************************

      if(!(fsinfo->evaluateFSCuts(combo, m_pInitial, "Raw"))) continue;

      //********************************************************************
      //
      //   DO THE PRIMARY VERTEX FITTING
      //
      //********************************************************************

      // set up for a primary vertex fit

      VertexFit* vtxfit = VertexFit::instance();
      vtxfit->init();

      VertexParameter primaryVertex(beamSpot);
      double          primaryVertex_chisq = -1.0;

      // set up a helper vertex with large errors

      VertexParameter wideVertex;
      HepPoint3D      vWideVertex(0., 0., 0.);
      HepSymMatrix    evWideVertex(3, 0);

      evWideVertex[0][0] = 1.0e12;
      evWideVertex[1][1] = 1.0e12;
      evWideVertex[2][2] = 1.0e12;

      wideVertex.setVx(vWideVertex);
      wideVertex.setEvx(evWideVertex);

      // only do the vertex fit if there are at least two tracks
      //   coming from the primary; otherwise primaryVertex will
      //   contain the beamSpot

      bool doVertexFit = false;
      int  vtxFitTrack = 0;
      for(unsigned int j = 0; j < combo.size(); j++)
      {
        FSParticle* fsp = combo[j];
        if(fsp->track()) vtxFitTrack++;
      }
      if(vtxFitTrack >= 2) doVertexFit = true;
      //if (m_bypassVertexDB) doVertexFit = false;

      if(doVertexFit)
      {

        // add tracks that come from the primary

        vtxFitTrack = 0;
        vector<int> vtxList;
        for(unsigned int j = 0; j < combo.size(); j++)
        {
          FSParticle* fsp = combo[j];
          if(fsp->track())
          {
            vtxList.push_back(vtxFitTrack);
            vtxfit->AddTrack(vtxFitTrack++, fsp->initialWTrack());
          }
        }

        // add the initial vertex with large errors

        vtxfit->AddVertex(0, wideVertex, vtxList);

        // do the fit

        if(!vtxfit->Fit(0)) continue;
        vtxfit->Swim(0);
        if(vtxfit->chisq(0) != vtxfit->chisq(0)) continue;

        // save track parameters after the vertex fit

        vtxFitTrack = 0;
        for(unsigned int j = 0; j < combo.size(); j++)
        {
          FSParticle* fsp = combo[j];
          if(fsp->track()) fsp->setVertexFitWTrack(vtxfit->wtrk(vtxFitTrack++));
        }

        // save the primary vertex

        primaryVertex.setVx(vtxfit->Vx(0));
        primaryVertex.setEvx(vtxfit->Evx(0));
        primaryVertex_chisq = vtxfit->chisq(0);
      }

      //********************************************************************
      //
      //   DO THE SECONDARY VERTEX FITTING
      //
      //********************************************************************

      // loop over secondary vertices

      for(unsigned int j = 0; j < combo.size(); j++)
      {
        if(combo[j]->vee())
        {
          FSParticle* fsp = combo[j];

          // initialize a vertex fit for Ks --> pi+ pi-
          //  or Lambda --> p+ pi- or ALambda --> p- pi+

          VertexFit* vtxfit2 = VertexFit::instance();
          vtxfit2->init();

          // add the daughters

          vtxfit2->AddTrack(0, fsp->veeInitialWTrack1());
          vtxfit2->AddTrack(1, fsp->veeInitialWTrack2());

          // add an initial vertex with large errors

          vtxfit2->AddVertex(0, wideVertex, 0, 1);

          // do the fit

          vtxfit2->Fit(0);
          vtxfit2->Swim(0);
          vtxfit2->BuildVirtualParticle(0);

          // now initialize a second vertex fit

          SecondVertexFit* svtxfit = SecondVertexFit::instance();
          svtxfit->init();

          // add the primary vertex

          svtxfit->setPrimaryVertex(primaryVertex);

          // add the secondary vertex

          svtxfit->setVpar(vtxfit2->vpar(0));

          // add the Ks or lambda

          svtxfit->AddTrack(0, vtxfit2->wVirtualTrack(0));

          // do the second vertex fit

          svtxfit->Fit();

          // save the new ks parameters

          fsp->setVeeVertexFitWTrack1(vtxfit2->wtrk(0));
          fsp->setVeeVertexFitWTrack2(vtxfit2->wtrk(1));
          fsp->setVeeVertexFitWTrack(svtxfit->wpar());
          fsp->setVeeLSigma(0.0);
          if(svtxfit->decayLengthError() != 0.0)
            fsp->setVeeLSigma(svtxfit->decayLength() / svtxfit->decayLengthError());
          fsp->setVeeSigma(svtxfit->decayLengthError());
          fsp->setVee2ndChi2(svtxfit->chisq());
          fsp->setVeeVertex(svtxfit->vpar().vx());
        }
      }

      //********************************************************************
      //
      //   SAVE INTERMEDIATE 4-VECTORS
      //
      //********************************************************************

      for(unsigned int j = 0; j < combo.size(); j++)
      {
        FSParticle* fsp = combo[j];
        if(fsp->shower()) { fsp->setIntFourMomentum(fsp->rawFourMomentum()); }
        if(fsp->track()) { fsp->setIntFourMomentum(fsp->vertexFitWTrack().p()); }
        if(fsp->pi0())
        {
          fsp->setIntFourMomentum(fsp->pi0()->loPfit() + fsp->pi0()->hiPfit());
          fsp->setIntFourMomentumA(fsp->pi0()->loPfit());
          fsp->setIntFourMomentumB(fsp->pi0()->hiPfit());
        }
        if(fsp->eta())
        {
          fsp->setIntFourMomentum(fsp->eta()->loPfit() + fsp->eta()->hiPfit());
          fsp->setIntFourMomentumA(fsp->eta()->loPfit());
          fsp->setIntFourMomentumB(fsp->eta()->hiPfit());
        }
        if(fsp->vee())
        {
          fsp->setIntFourMomentum(fsp->veeVertexFitWTrack().p());
          fsp->setIntFourMomentumA(fsp->veeVertexFitWTrack1().p());
          fsp->setIntFourMomentumB(fsp->veeVertexFitWTrack2().p());
        }
      }

      //********************************************************************
      //
      //   MAKE CUTS ON INTERMEDIATE PARTICLE COMBINATIONS (INT 4-VECTORS)
      //
      //********************************************************************

      if(!(fsinfo->evaluateFSCuts(combo, m_pInitial, "Int"))) continue;

      //********************************************************************
      //
      //   DO THE KINEMATIC FITTING
      //
      //********************************************************************

      // initialize

      KalmanKinematicFit* kmfit = KalmanKinematicFit::instance();
      //KinematicFit* kmfit = KinematicFit::instance();
      kmfit->init();

      // add the beam spot or the vertex or anything??

      if(!m_bypassVertexDB)
      {
        kmfit->setBeamPosition(beamSpot.vx());
        kmfit->setVBeamPosition(beamSpot.Evx());
      }

      // add final state particles

      int           kinFitTrack = 0;
      map<int, int> combo2kinFitTrack;
      int           kinFitConstraint = 0;
      int           kinFitDOF        = 0;
      for(unsigned int j = 0; j < combo.size(); j++)
      {
        FSParticle* fsp      = combo[j];
        combo2kinFitTrack[j] = kinFitTrack;
        if(fsp->shower()) { kmfit->AddTrack(kinFitTrack++, 0.0, fsp->shower()->emcShower()); }
        if(fsp->track()) { kmfit->AddTrack(kinFitTrack++, fsp->vertexFitWTrack()); }
        if(fsp->pi0())
        {
          kmfit->AddTrack(kinFitTrack++, 0.0, fsp->pi0Lo()->emcShower());
          kmfit->AddTrack(kinFitTrack++, 0.0, fsp->pi0Hi()->emcShower());
          kmfit->AddResonance(kinFitConstraint++, fsp->mass(), kinFitTrack - 2, kinFitTrack - 1);
          kinFitDOF++;
        }
        if(fsp->eta())
        {
          kmfit->AddTrack(kinFitTrack++, 0.0, fsp->etaLo()->emcShower());
          kmfit->AddTrack(kinFitTrack++, 0.0, fsp->etaHi()->emcShower());
          kmfit->AddResonance(kinFitConstraint++, fsp->mass(), kinFitTrack - 2, kinFitTrack - 1);
          kinFitDOF++;
        }
        if(fsp->vee())
        {
          kmfit->AddTrack(kinFitTrack++, fsp->veeVertexFitWTrack1());
          kmfit->AddTrack(kinFitTrack++, fsp->veeVertexFitWTrack2());
          kmfit->AddResonance(kinFitConstraint++, fsp->mass(), kinFitTrack - 2, kinFitTrack - 1);
          //kmfit->AddTrack(kinFitTrack++,fsp->veeVertexFitWTrack());
          kinFitDOF++;
        }
      }

      // add the initial four momentum

      if(fsinfo->exclusive())
      {
        kmfit->AddFourMomentum(kinFitConstraint++, m_pInitial);
        kinFitDOF += 4;
      }

      // add missing mass

      if(fsinfo->inclusive() && fsinfo->missingMassFit())
      {
        kmfit->AddMissTrack(kinFitTrack++, fsinfo->missingMassValue());
        kmfit->AddFourMomentum(kinFitConstraint++, m_pInitial);
        kinFitDOF += 1;
      }

      // do the fit and make a very loose cut on the resulting chi2

      double saveFitChi2 = 0.0;
      double saveFitDOF  = 0.0;
      if(kinFitConstraint > 0)
      {
        kmfit->setChisqCut(m_maxChi2DOF * kinFitDOF);
        if(!kmfit->Fit()) continue;
        if(kmfit->chisq() / kinFitDOF > m_maxChi2DOF || kmfit->chisq() < 0) continue;
        saveFitChi2 = kmfit->chisq();
        saveFitDOF  = kinFitDOF;
      }

      //********************************************************************
      //
      //   SAVE KINEMATIC FIT 4-VECTORS
      //
      //********************************************************************

      if(kinFitConstraint > 0)
      {
        kinFitTrack = 0;
        for(unsigned int j = 0; j < combo.size(); j++)
        {
          FSParticle* fsp = combo[j];
          if(fsp->shower() || fsp->track()) { fsp->setFitFourMomentum(kmfit->pfit(kinFitTrack++)); }
          if(fsp->pi0() || fsp->eta() || fsp->vee())
          {
            fsp->setFitFourMomentum(kmfit->pfit(kinFitTrack++) + kmfit->pfit(kinFitTrack++));
            fsp->setFitFourMomentumA(kmfit->pfit(kinFitTrack - 2));
            fsp->setFitFourMomentumB(kmfit->pfit(kinFitTrack - 1));
          }
        }
      }

      //********************************************************************
      //
      //   MAKE CUTS ON INTERMEDIATE PARTICLE COMBINATIONS (FIT 4-VECTORS)
      //
      //********************************************************************

      if(!(fsinfo->evaluateFSCuts(combo, m_pInitial, "Fit"))) continue;

      //********************************************************************
      //
      //   ADD ONE MORE LAYER OF CONSTRAINTS [EXT for EXTRA]
      //
      //********************************************************************

      // add additional constraints

      vector<FSConstraint*> constraints = fsinfo->getFSConstraints();
      for(unsigned int ic = 0; ic < constraints.size(); ic++)
      {
        FSConstraint*                 constraint = constraints[ic];
        vector<vector<unsigned int> > indicesAll =
          fsinfo->submodeIndices(constraint->submodeName());
        vector<unsigned int> indices = indicesAll[0];
        if(indicesAll.size() > 1)
        {
          double dBest = 1.0e10;
          int    iBest = 0;
          for(unsigned int iA = 0; iA < indicesAll.size(); iA++)
          {
            vector<unsigned int> indicesTest = indicesAll[iA];
            HepLorentzVector     pTest(0.0, 0.0, 0.0, 0.0);
            for(unsigned int iT = 0; iT < indicesTest.size(); iT++)
            { pTest += combo[indicesTest[iT]]->rawFourMomentum(); }
            double dTest = fabs(pTest.m() - constraint->constraintValue());
            if(dTest < dBest)
            {
              dBest = dTest;
              iBest = iA;
            }
          }
          indices = indicesAll[iBest];
        }
        if(indices.size() == 2)
          kmfit->AddResonance(kinFitConstraint++, constraint->constraintValue(),
                              combo2kinFitTrack[indices[0]], combo2kinFitTrack[indices[1]]);
        if(indices.size() == 3)
          kmfit->AddResonance(kinFitConstraint++, constraint->constraintValue(),
                              combo2kinFitTrack[indices[0]], combo2kinFitTrack[indices[1]],
                              combo2kinFitTrack[indices[2]]);
        if(indices.size() == 4)
          kmfit->AddResonance(kinFitConstraint++, constraint->constraintValue(),
                              combo2kinFitTrack[indices[0]], combo2kinFitTrack[indices[1]],
                              combo2kinFitTrack[indices[2]], combo2kinFitTrack[indices[3]]);
        if(indices.size() == 5)
          kmfit->AddResonance(kinFitConstraint++, constraint->constraintValue(),
                              combo2kinFitTrack[indices[0]], combo2kinFitTrack[indices[1]],
                              combo2kinFitTrack[indices[2]], combo2kinFitTrack[indices[3]],
                              combo2kinFitTrack[indices[4]]);
        kinFitDOF++;
      }

      // do the fit

      double saveExtChi2 = 1.0e6;
      double saveExtDOF  = kinFitDOF;
      if(constraints.size() > 0)
      {
        kmfit->setChisqCut(1e6 * kinFitDOF);
        if(kmfit->Fit()) saveExtChi2 = kmfit->chisq();
      }

      // save the four-vectors

      if(constraints.size() > 0)
      {
        kinFitTrack = 0;
        for(unsigned int j = 0; j < combo.size(); j++)
        {
          FSParticle* fsp = combo[j];
          if(fsp->shower() || fsp->track()) { fsp->setExtFourMomentum(kmfit->pfit(kinFitTrack++)); }
          if(fsp->pi0() || fsp->eta() || fsp->vee())
          {
            fsp->setExtFourMomentum(kmfit->pfit(kinFitTrack++) + kmfit->pfit(kinFitTrack++));
            fsp->setExtFourMomentumA(kmfit->pfit(kinFitTrack - 2));
            fsp->setExtFourMomentumB(kmfit->pfit(kinFitTrack - 1));
          }
        }
      }

      //********************************************************************
      //
      //   CALCULATE THE TOTAL ENERGY AND MOMENTUM
      //
      //********************************************************************

      HepLorentzVector pTotal(0.0, 0.0, 0.0, 0.0);
      for(int i = 0; i < combo.size(); i++) { pTotal += combo[i]->rawFourMomentum(); }

      HepLorentzVector pMissing = m_pInitial - pTotal;

      //********************************************************************
      //
      //   RECORD INFORMATION
      //
      //********************************************************************

      // record event level information

      NT->fillEvent(eventHeader, evtRecEvent, m_pInitial.e());
      NT->fillDouble("NPi0s", pi0List.size());
      NT->fillVertices(beamSpot, primaryVertex);
      NT->fillJPsiFinder(evtRecTrackCol);
      if(runNumber < 0) NT->fillMCTruth(m_mcTruthHelper, fsinfo);

      // record odds and ends that have no other place

      NT->fillDouble("TotalEnergy", pTotal.e());
      NT->fillDouble("TotalPx", pTotal.px());
      NT->fillDouble("TotalPy", pTotal.py());
      NT->fillDouble("TotalPz", pTotal.pz());
      NT->fillDouble("TotalP", pTotal.vect().mag());
      NT->fillDouble("MissingMass2", pMissing.m2());

      if(doVertexFit)
        NT->fillDouble("VChi2", primaryVertex_chisq);
      else
        NT->fillDouble("VChi2", -1.0);

      if(kinFitConstraint > 0)
      {
        NT->fillDouble("Chi2", saveFitChi2);
        NT->fillDouble("Chi2DOF", saveFitChi2 / saveFitDOF);
      }
      if(constraints.size() > 0)
      {
        NT->fillDouble("EChi2", saveExtChi2);
        NT->fillDouble("EChi2DOF", saveExtChi2 / saveExtDOF);
      }

      // record particle level information

      string RTAG("R");
      string ITAG("I");
      if(kinFitConstraint == 0) ITAG = "";
      string ETAG("E");
      for(unsigned int j = 0; j < combo.size(); j++)
      {
        FSParticle* fsp = combo[j];

        // record 4-vectors

        NT->fill4Momentum(j + 1, "", RTAG, fsp->rawFourMomentum());
        NT->fill4Momentum(j + 1, "", ITAG, fsp->intFourMomentum());
        if(kinFitConstraint > 0) { NT->fill4Momentum(j + 1, "", "", fsp->fitFourMomentum()); }
        if(constraints.size() > 0) { NT->fill4Momentum(j + 1, "", ETAG, fsp->extFourMomentum()); }
        if(fsp->pi0() || fsp->eta() || fsp->vee())
        {
          NT->fill4Momentum(j + 1, "a", RTAG, fsp->rawFourMomentumA());
          NT->fill4Momentum(j + 1, "b", RTAG, fsp->rawFourMomentumB());
          NT->fill4Momentum(j + 1, "a", ITAG, fsp->intFourMomentumA());
          NT->fill4Momentum(j + 1, "b", ITAG, fsp->intFourMomentumB());
          if(kinFitConstraint > 0)
          {
            NT->fill4Momentum(j + 1, "a", "", fsp->fitFourMomentumA());
            NT->fill4Momentum(j + 1, "b", "", fsp->fitFourMomentumB());
          }
          if(constraints.size() > 0)
          {
            NT->fill4Momentum(j + 1, "a", ETAG, fsp->extFourMomentumA());
            NT->fill4Momentum(j + 1, "b", ETAG, fsp->extFourMomentumB());
          }
        }

        // record particle-specific information

        if(fsp->shower())
        { NT->fillShower(j + 1, "", "Sh", fsp->shower(), pi0List, evtRecTrackCol); }
        if(fsp->track())
        {
          NT->fillTrack(j + 1, "", "Tk", fsp->track(), beamSpot, fsp->trackIndex(), m_pidStudies);
        }
        if(fsp->pi0())
        {
          NT->fillPi0(j + 1, "", "Pi0", fsp->pi0());
          NT->fillShower(j + 1, "a", "Sh", fsp->pi0Lo(), pi0List, evtRecTrackCol);
          NT->fillShower(j + 1, "b", "Sh", fsp->pi0Hi(), pi0List, evtRecTrackCol);
        }
        if(fsp->eta())
        {
          NT->fillEta(j + 1, "", "Eta", fsp->eta());
          NT->fillShower(j + 1, "a", "Sh", fsp->etaLo(), pi0List, evtRecTrackCol);
          NT->fillShower(j + 1, "b", "Sh", fsp->etaHi(), pi0List, evtRecTrackCol);
        }
        if(fsp->vee())
        {
          NT->fillVee(j + 1, "", "Vee", fsp);
          NT->fillTrack(j + 1, "a", "Tk", fsp->veeTrack1(), beamSpot, fsp->trackIndex(),
                        m_pidStudies);
          NT->fillTrack(j + 1, "b", "Tk", fsp->veeTrack2(), beamSpot, fsp->trackIndex(),
                        m_pidStudies);
        }
      }

      // write the tree

      NT->write();
    }

    fsinfo->particleCombinations().clear();
  }

  //********************************************************************
  //
  //   CLEAN UP MEMORY AND RETURN
  //
  //********************************************************************

  for(unsigned int i = 0; i < lambdaList.size(); i++) { delete lambdaList[i]; }
  for(unsigned int i = 0; i < alambdaList.size(); i++) { delete alambdaList[i]; }
  for(unsigned int i = 0; i < epList.size(); i++) { delete epList[i]; }
  for(unsigned int i = 0; i < emList.size(); i++) { delete emList[i]; }
  for(unsigned int i = 0; i < mupList.size(); i++) { delete mupList[i]; }
  for(unsigned int i = 0; i < mumList.size(); i++) { delete mumList[i]; }
  for(unsigned int i = 0; i < ppList.size(); i++) { delete ppList[i]; }
  for(unsigned int i = 0; i < pmList.size(); i++) { delete pmList[i]; }
  for(unsigned int i = 0; i < etaList.size(); i++) { delete etaList[i]; }
  for(unsigned int i = 0; i < gammaList.size(); i++) { delete gammaList[i]; }
  for(unsigned int i = 0; i < kpList.size(); i++) { delete kpList[i]; }
  for(unsigned int i = 0; i < kmList.size(); i++) { delete kmList[i]; }
  for(unsigned int i = 0; i < ksList.size(); i++) { delete ksList[i]; }
  for(unsigned int i = 0; i < pipList.size(); i++) { delete pipList[i]; }
  for(unsigned int i = 0; i < pimList.size(); i++) { delete pimList[i]; }
  for(unsigned int i = 0; i < pi0List.size(); i++) { delete pi0List[i]; }

  if(runNumber < 0) delete m_mcTruthHelper;

  return StatusCode::SUCCESS;
}

//********************************************************************
//********************************************************************
//********************************************************************
//
//   FINALIZE
//
//********************************************************************
//********************************************************************
//********************************************************************

StatusCode FSFilter::finalize()
{

  cout << "FINAL EVENT COUNTER = " << m_eventCounter << endl;

  return StatusCode::SUCCESS;
}

//********************************************************************
//********************************************************************
//********************************************************************
//
//   CHECK THE ENERGY AND MOMENTUM BALANCE OF A GIVEN FINAL STATE
//
//********************************************************************
//********************************************************************
//********************************************************************

bool FSFilter::checkCombination(const vector<FSParticle*>& combo, bool complete, bool inclusive)
{

  if(inclusive) return true;

  // if the combination isn't yet complete, just check to make sure there
  //  is no excess energy outside of the energy tolerance

  if(!complete)
  {

    double totalEnergy = 0.0;
    for(int i = 0; i < combo.size(); i++) { totalEnergy += combo[i]->rawFourMomentum().e(); }

    double excessEnergy = totalEnergy - m_pInitial.e();

    if(excessEnergy > m_energyTolerance) return false;
    return true;
  }

  // if the combination is complete, calculate the total energy and momentum

  HepLorentzVector pTotal(0.0, 0.0, 0.0, 0.0);
  for(int i = 0; i < combo.size(); i++) { pTotal += combo[i]->rawFourMomentum(); }

  HepLorentzVector pMissing = m_pInitial - pTotal;

  // if the combination is complete and exclusive, check the energy
  //   and momentum balance

  if(fabs(pMissing.e()) > m_energyTolerance) return false;
  if(fabs(pMissing.vect().mag()) > m_momentumTolerance) return false;
  return true;
}
