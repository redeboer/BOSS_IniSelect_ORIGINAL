#include "IniSelect/UnitTests/Test_AlgSvc.h"

std::vector<SmartIF<IAuditorSvc> >      Test_AlgSvc::fAuditorSvc;
std::vector<SmartIF<IChronoStatSvc> >   Test_AlgSvc::fChronoSvc;
std::vector<SmartIF<IDataProviderSvc> > Test_AlgSvc::fDetSvc;
std::vector<SmartIF<IConversionSvc> >   Test_AlgSvc::fDetCnvSvc;
std::vector<SmartIF<IDataProviderSvc> > Test_AlgSvc::fEventSvc;
std::vector<SmartIF<IConversionSvc> >   Test_AlgSvc::fEventCnvSvc;
std::vector<SmartIF<IHistogramSvc> >    Test_AlgSvc::fHistoSvc;
std::vector<SmartIF<INTupleSvc> >       Test_AlgSvc::fNtupleSvc;
std::vector<SmartIF<IRndmGenSvc> >      Test_AlgSvc::fRandSvc;
std::vector<SmartIF<IToolSvc> >         Test_AlgSvc::fToolSvc;
std::vector<SmartIF<IExceptionSvc> >    Test_AlgSvc::fExceptionSvc;
std::vector<SmartIF<IAlgContextSvc> >   Test_AlgSvc::fContextSvc;
std::vector<SmartIF<IMessageSvc> >      Test_AlgSvc::fMsgSvc;

StatusCode Test_AlgSvc1::initialize()
{
  Test_AlgSvc::fAuditorSvc.push_back(auditorSvc());
  Test_AlgSvc::fChronoSvc.push_back(chronoSvc());
  Test_AlgSvc::fDetSvc.push_back(detSvc());
  Test_AlgSvc::fDetCnvSvc.push_back(detCnvSvc());
  Test_AlgSvc::fEventSvc.push_back(eventSvc());
  Test_AlgSvc::fEventCnvSvc.push_back(eventCnvSvc());
  Test_AlgSvc::fHistoSvc.push_back(histoSvc());
  // Test_AlgSvc::fNtupleSvc.push_back(ntupleSvc());
  Test_AlgSvc::fRandSvc.push_back(randSvc());
  Test_AlgSvc::fToolSvc.push_back(toolSvc());
  Test_AlgSvc::fExceptionSvc.push_back(exceptionSvc());
  Test_AlgSvc::fContextSvc.push_back(contextSvc());
  Test_AlgSvc::fMsgSvc.push_back(msgSvc());
}

StatusCode Test_AlgSvc2::initialize()
{
  Test_AlgSvc::fAuditorSvc.push_back(auditorSvc());
  Test_AlgSvc::fChronoSvc.push_back(chronoSvc());
  Test_AlgSvc::fDetSvc.push_back(detSvc());
  Test_AlgSvc::fDetCnvSvc.push_back(detCnvSvc());
  Test_AlgSvc::fEventSvc.push_back(eventSvc());
  Test_AlgSvc::fEventCnvSvc.push_back(eventCnvSvc());
  Test_AlgSvc::fHistoSvc.push_back(histoSvc());
  // Test_AlgSvc::fNtupleSvc.push_back(ntupleSvc());
  Test_AlgSvc::fRandSvc.push_back(randSvc());
  Test_AlgSvc::fToolSvc.push_back(toolSvc());
  Test_AlgSvc::fExceptionSvc.push_back(exceptionSvc());
  Test_AlgSvc::fContextSvc.push_back(contextSvc());
  Test_AlgSvc::fMsgSvc.push_back(msgSvc());
}

StatusCode Test_AlgSvc3::initialize()
{
  Test_AlgSvc::fAuditorSvc.push_back(auditorSvc());
  Test_AlgSvc::fChronoSvc.push_back(chronoSvc());
  Test_AlgSvc::fDetSvc.push_back(detSvc());
  Test_AlgSvc::fDetCnvSvc.push_back(detCnvSvc());
  Test_AlgSvc::fEventSvc.push_back(eventSvc());
  Test_AlgSvc::fEventCnvSvc.push_back(eventCnvSvc());
  Test_AlgSvc::fHistoSvc.push_back(histoSvc());
  // Test_AlgSvc::fNtupleSvc.push_back(ntupleSvc());
  Test_AlgSvc::fRandSvc.push_back(randSvc());
  Test_AlgSvc::fToolSvc.push_back(toolSvc());
  Test_AlgSvc::fExceptionSvc.push_back(exceptionSvc());
  Test_AlgSvc::fContextSvc.push_back(contextSvc());
  Test_AlgSvc::fMsgSvc.push_back(msgSvc());
}

void Test_AlgSvc::TestFinalize()
{
  size_t i;
  for(i = 0; i < fAuditorSvc.size(); ++i)
    REQUIRE(fAuditorSvc[i] == auditorSvc());
  for(i = 0; i < fChronoSvc.size(); ++i)
    REQUIRE(fChronoSvc[i] == chronoSvc());
  for(i = 0; i < fDetSvc.size(); ++i)
    REQUIRE(fDetSvc[i] == detSvc());
  for(i = 0; i < fDetCnvSvc.size(); ++i)
    REQUIRE(fDetCnvSvc[i] == detCnvSvc());
  for(i = 0; i < fEventSvc.size(); ++i)
    REQUIRE(fEventSvc[i] == eventSvc());
  for(i = 0; i < fEventCnvSvc.size(); ++i)
    REQUIRE(fEventCnvSvc[i] == eventCnvSvc());
  for(i = 0; i < fHistoSvc.size(); ++i)
    REQUIRE(fHistoSvc[i] == histoSvc());
  // for(i = 0; i < fNtupleSvc.size(); ++i)
  //   REQUIRE(fNtupleSvc[i] == ntupleSvc());
  for(i = 0; i < fRandSvc.size(); ++i)
    REQUIRE(fRandSvc[i] == randSvc());
  for(i = 0; i < fToolSvc.size(); ++i)
    REQUIRE(fToolSvc[i] == toolSvc());
  for(i = 0; i < fExceptionSvc.size(); ++i)
    REQUIRE(fExceptionSvc[i] == exceptionSvc());
  for(i = 0; i < fContextSvc.size(); ++i)
    REQUIRE(fContextSvc[i] == contextSvc());
  for(i = 0; i < fMsgSvc.size(); ++i)
    REQUIRE(fMsgSvc[i] == msgSvc());
}