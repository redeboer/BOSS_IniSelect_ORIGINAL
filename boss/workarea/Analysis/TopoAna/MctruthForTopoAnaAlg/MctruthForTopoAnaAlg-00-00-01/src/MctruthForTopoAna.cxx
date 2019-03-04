#include "GaudiKernel/SmartDataPtr.h"
#include "EventModel/EventHeader.h"
#include "McTruth/McParticle.h"

#include "MctruthForTopoAnaAlg/MctruthForTopoAna.h"    

MctruthForTopoAna::MctruthForTopoAna(const std::string & name,ISvcLocator * pSvcLocator): Algorithm(name,pSvcLocator){}

StatusCode MctruthForTopoAna::initialize()
{
  MsgStream log(msgSvc(),name());
  log<<MSG::INFO<<"in initialize()"<<endreq;
  StatusCode status;

  NTuplePtr nt(ntupleSvc(),"FILE1/MctruthForTopoAna");
  if(nt)
     m_tuple=nt;
  else
    {
      m_tuple=ntupleSvc()->book("FILE1/MctruthForTopoAna",CLID_ColumnWiseTuple,"Mctruth For Topology Analysis");
      if(m_tuple)
        {
          status=m_tuple->addItem("Ievt",m_ievt);
          status=m_tuple->addItem("Runid",m_runid);
          status=m_tuple->addItem("Evtid",m_evtid);
          status=m_tuple->addItem("Nmcps",m_nmcps,0,100);
          status=m_tuple->addIndexedItem("Pid",m_nmcps,m_pid);
          status=m_tuple->addIndexedItem("Midx",m_nmcps,m_midx);
        }
      else
        {
          log<<MSG::ERROR<<"Cannot book N-tuple:"<<long(m_tuple)<<endmsg;
          return StatusCode::FAILURE;
        }
    }
  return StatusCode::SUCCESS;
}

int ievt=0;
StatusCode MctruthForTopoAna::execute() 
{
  ievt++;
  m_ievt=ievt;
  //std::cout<<std::endl<<std::endl;
  //std::cout<<"ievt="<<ievt<<std::endl;
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  //std::cout<<"RunNo="<<eventHeader->runNumber()<<std::endl;
  //std::cout<<"EvtNo="<<eventHeader->eventNumber()<<std::endl;
  m_runid=eventHeader->runNumber();
  m_evtid=eventHeader->eventNumber();

  SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
  if(!mcParticleCol)
    {
      std::cout<<"Could not retrieve McParticelCol"<<std::endl;
      return(StatusCode::FAILURE);  
    }
  Event::McParticleCol::iterator iter_mc=mcParticleCol->begin();
  m_nmcps=0;
  for (;iter_mc!=mcParticleCol->end();iter_mc++)
    {
      //std::cout<<"idx="<<m_nmcps<<"\t"<<"pid="<<(*iter_mc)->particleProperty()<<"\t"<<"midx="<<((*iter_mc)->mother()).trackIndex()<<std::endl;
      m_pid[m_nmcps]=(*iter_mc)->particleProperty();
      m_midx[m_nmcps]=((*iter_mc)->mother()).trackIndex();
      m_nmcps++;
    }
  m_tuple->write();
  return StatusCode::SUCCESS;
}

StatusCode MctruthForTopoAna::finalize()
{
  MsgStream log(msgSvc(),name());
  std::cout<<"ievt="<<ievt<<std::endl;
  log<<MSG::INFO<<"in finalize()"<<endreq;
}
