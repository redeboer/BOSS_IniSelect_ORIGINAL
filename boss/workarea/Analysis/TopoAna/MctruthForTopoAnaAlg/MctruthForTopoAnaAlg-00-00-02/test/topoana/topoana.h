#ifndef TOPOANA_H
#define TOPOANA_H

#include <vector>
#include <string>
#include <list>
#include <map>
using namespace std;

class topoana
{
  private:
          vector<string> m_inputRootFileNames;
	  string         m_inputTreeName;
          string m_outputMainFileName;
          vector<int>           m_vSigPid;
          vector< vector<int> > m_vVSigPid;
          vector< vector<int> > m_vVSigMidx;

          vector< vector< list<int> > > m_vEvtDcyTr;
          vector<int> m_vIEvtDcyTr;
          vector<int> m_vNEvtDcyTr;
          vector< list<int> > m_vEvtDcyIFChn;
          vector<int> m_vIEvtDcyIFChn;
          vector<int> m_vNEvtDcyIFChn;
          vector<int> m_vISigPid;              
          vector<int> m_vNSigPid;
          vector< vector< list<int> > > m_vSigEvtDcyTr;
          vector<int> m_vISigEvtDcyTr;
          vector<int> m_vNSigEvtDcyTr;
          vector< list<int> > m_vSigEvtDcyIFChn;
          vector<int> m_vISigEvtDcyIFChn;
          vector<int> m_vNSigEvtDcyIFChn;

          map<int,int> m_pidThreeMPchargeMap;
          map<int,string> m_pidTxtpnameMap;
          map<int,string> m_pidTexpnameMap;
  public:
          ~topoana(){}
          void           readCard(string topoAnaCardName);
          void           makePidThreeMChargeMap();
	  void           sortByPidAndPcharge(vector<int> &,vector<int> &,vector<int> &);
	  void           evtReorder(vector<int> & vPid,vector<int> & vMidx);
          void           getEvtDcyTr(vector<int> & vPid,vector<int> & vMidx,vector< list<int> > & evtDcyTr);
          void           sortByPidAndPcharge(list<int> &);
	  void           getEvtDcyIFChn(vector<int> & vPid,vector<int> & vMidx,list<int> & evtDcyIFChn);
          void           sortByFirstFromLargeToSmall(vector<int> &,vector<int> &,vector<int> &);
          void           sortByFirstFromLargeToSmall(vector<int> &,vector< vector< list<int> > > &,vector<int> &);
          void           sortByFirstFromLargeToSmall(vector<int> &,vector< list<int> > &,vector<int> &);
          void           getResult();

          void           makePidTxtpnameMap();
          void           outputResultIntoTxtFile();
          void           makePidTexpnameMap();
          void           outputResultIntoTexFile();
          void           convertTexFileIntoPdfFile();
};

#endif
