#ifndef Analysis_IniSelect_UnitTests_AlgSvc_H
#define Analysis_IniSelect_UnitTests_AlgSvc_H

#include "IniSelect/UnitTests/UnitTester.h"
#include <vector>

/// This is to test whether the services in the Gaudi `Algorithm` are `static`.
class Test_AlgSvc : public UnitTester
{
public:
  Test_AlgSvc(const std::string& name, ISvcLocator* pSvcLocator) :
    UnitTester(name, pSvcLocator)
  {}

  void TestInitialize() {}
  void TestExecute() {}
  void TestEvent() {}
  void TestFinalize();

  static std::vector<SmartIF<IAuditorSvc> >      fAuditorSvc;
  static std::vector<SmartIF<IChronoStatSvc> >   fChronoSvc;
  static std::vector<SmartIF<IDataProviderSvc> > fDetSvc;
  static std::vector<SmartIF<IConversionSvc> >   fDetCnvSvc;
  static std::vector<SmartIF<IDataProviderSvc> > fEventSvc;
  static std::vector<SmartIF<IConversionSvc> >   fEventCnvSvc;
  static std::vector<SmartIF<IHistogramSvc> >    fHistoSvc;
  static std::vector<SmartIF<INTupleSvc> >       fNtupleSvc;
  static std::vector<SmartIF<IRndmGenSvc> >      fRandSvc;
  static std::vector<SmartIF<IToolSvc> >         fToolSvc;
  static std::vector<SmartIF<IExceptionSvc> >    fExceptionSvc;
  static std::vector<SmartIF<IAlgContextSvc> >   fContextSvc;
  static std::vector<SmartIF<IMessageSvc> >      fMsgSvc;
};

class Test_AlgSvc1 : public Algorithm
{
public:
  Test_AlgSvc1(const std::string& name, ISvcLocator* pSvcLocator) :
    Algorithm(name, pSvcLocator)
  {}

  StatusCode initialize();
  StatusCode execute() {}
  StatusCode event() {}
  StatusCode finalize() {}
};

class Test_AlgSvc2 : public Algorithm
{
public:
  Test_AlgSvc2(const std::string& name, ISvcLocator* pSvcLocator) :
    Algorithm(name, pSvcLocator)
  {}

  StatusCode initialize();
  StatusCode execute() {}
  StatusCode finalize() {}
};

class Test_AlgSvc3 : public Algorithm
{
public:
  Test_AlgSvc3(const std::string& name, ISvcLocator* pSvcLocator) :
    Algorithm(name, pSvcLocator)
  {}

  StatusCode initialize();
  StatusCode execute() {}
  StatusCode finalize() {}
};

#endif