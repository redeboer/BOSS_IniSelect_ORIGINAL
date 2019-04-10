#include "../include/topoana.h"
#include "TBranch.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

void topoana::getRslt()
{
  unsigned int Nasterisks = 89;
  for(unsigned int i = 0; i < Nasterisks; i++) cout << "*";
  cout << endl << endl;

  TChain* chn = new TChain(m_trNm.c_str());
  for(unsigned int i = 0; i < m_nmsOfIptRootFls.size(); i++)
  { chn->Add(m_nmsOfIptRootFls[i].c_str()); }

  // int Nps,Pid[200],Midx[200]; // The statement instead of the following two ought to be used
  // along with the Ntuple Tool MCGenKinematics with the option [200].
  const unsigned int NpsMax = chn->GetMaximum(m_brNmOfNps.c_str());
  int                Nps, Pid[NpsMax], Midx[NpsMax];
  chn->SetBranchAddress(m_brNmOfNps.c_str(), &Nps);
  chn->SetBranchAddress(m_brNmOfPid.c_str(), &Pid);
  chn->SetBranchAddress(m_brNmOfMidx.c_str(), &Midx);

  unsigned long nEtrs = chn->GetEntries();
  cout << "There are " << nEtrs << " entries in total in the input root files." << endl << endl;
  unsigned long nEtrsToBePrcsd = nEtrs < m_nEtrsMax ? nEtrs : m_nEtrsMax;

  bool                openANewOptRootFl = true;
  unsigned int        iOptRootFls       = 0;
  string              NmOfOptRootFl;
  TFile*              fl;
  TTree*              tr;
  bool                closeTheOptRootFl1;
  bool                closeTheOptRootFl2;
  bool                closeTheOptRootFl3;
  const unsigned long nTotBytesMaxOfASngOptRootFl =
    (unsigned long)3 * 1024 * 1024 * 1024; // The size of a single output root file is 3G.

  int                iDcyTr, iCcDcyTr, iDcyFSt, iCcDcyFSt;
  const unsigned int sPttaPid = m_vPttaPid.size() > 0 ? m_vPttaPid.size() : 1;
  char               nameP[sPttaPid][64];
  int                nP[sPttaPid], iCcP[sPttaPid], nCcP[sPttaPid], nTotP[sPttaPid];
  int                iDcyBrP[sPttaPid][200], iCcDcyBrP[sPttaPid][200], iDcyBrCcP[sPttaPid][200];
  const unsigned int sVPttaPid1 = m_vPttaIncDcyBr.size() > 0 ? m_vPttaIncDcyBr.size() : 1;
  char               nameIncDcyBr[sVPttaPid1][64];
  int                nIncDcyBr[sVPttaPid1], iCcIncDcyBr[sVPttaPid1], nCcIncDcyBr[sVPttaPid1],
    nTotIncDcyBr[sVPttaPid1];
  int iDcyBrIncDcyBr[sVPttaPid1][200], iCcDcyBrIncDcyBr[sVPttaPid1][200],
    iDcyBrCcIncDcyBr[sVPttaPid1][200];
  const unsigned int sSigP = m_vSigPid.size() > 0 ? m_vSigPid.size() : 1;
  int                nSigP[sSigP], nCcSigP[sSigP], nTotSigP[sSigP], iCcSigP[sSigP];
  const unsigned int sSigIncDcyBr = m_vSigIncDcyBr.size() > 0 ? m_vSigIncDcyBr.size() : 1;
  int nSigIncDcyBr[sSigIncDcyBr], nCcSigIncDcyBr[sSigIncDcyBr], nTotSigIncDcyBr[sSigIncDcyBr],
    iCcSigIncDcyBr[sSigIncDcyBr];
  const unsigned int sSigSeqDcyBrs = m_vSigSeqDcyBrs.size() > 0 ? m_vSigSeqDcyBrs.size() : 1;
  int nSigSeqDcyBrs[sSigSeqDcyBrs], nCcSigSeqDcyBrs[sSigSeqDcyBrs], nTotSigSeqDcyBrs[sSigSeqDcyBrs],
    iCcSigSeqDcyBrs[sSigSeqDcyBrs];
  const unsigned int sSigIncSeqDcyBrs =
    m_vSigIncSeqDcyBrs.size() > 0 ? m_vSigIncSeqDcyBrs.size() : 1;
  int nSigIncSeqDcyBrs[sSigIncSeqDcyBrs], nCcSigIncSeqDcyBrs[sSigIncSeqDcyBrs],
    nTotSigIncSeqDcyBrs[sSigIncSeqDcyBrs], iCcSigIncSeqDcyBrs[sSigIncSeqDcyBrs];
  const unsigned int sSigIRADcyBr = m_vSigIRADcyBr.size() > 0 ? m_vSigIRADcyBr.size() : 1;
  int nSigIRADcyBr[sSigIRADcyBr], nCcSigIRADcyBr[sSigIRADcyBr], nTotSigIRADcyBr[sSigIRADcyBr],
    iCcSigIRADcyBr[sSigIRADcyBr];
  const unsigned int sSigIncOrIRASeqDcyBrs =
    m_vSigIncOrIRASeqDcyBrs.size() > 0 ? m_vSigIncOrIRASeqDcyBrs.size() : 1;
  int nSigIncOrIRASeqDcyBrs[sSigIncOrIRASeqDcyBrs], nCcSigIncOrIRASeqDcyBrs[sSigIncOrIRASeqDcyBrs],
    nTotSigIncOrIRASeqDcyBrs[sSigIncOrIRASeqDcyBrs], iCcSigIncOrIRASeqDcyBrs[sSigIncOrIRASeqDcyBrs];
  int iSigDcyTr, iCcSigDcyTr, iSigDcyFSt, iCcSigDcyFSt;
  int iSigDcyFSt2, iCcSigDcyFSt2;

  clock_t       starttime;
  unsigned long nEtrsForTiming = 50000;

  TTreeFormula* trfml;
  unsigned int  nInsts = 1;
  if(!m_cut.empty())
  {
    trfml = new TTreeFormula("trfml", m_cut.c_str(), chn);
    chn->SetNotify(trfml); // This statement is indispensible if more than one root file is added to
                           // the object of the TChain class.
  }
  unsigned long nEtrsThroughTheCut = 0;

  vector<int>       vPid, vCcPid;
  vector<int>       vMidx, vCcMidx;
  vector<list<int>> dcyTr, ccDcyTr;
  vector<int>       vIdxOfHead, vCcIdxOfHead;
  vector<int>       vMidxOfHead, vCcMidxOfHead;
  string            strDcyTr, strCcDcyTr;
  list<int>         dcyFSt, ccDcyFSt;
  string            strDcyFSt, strCcDcyFSt;
  ostringstream     oss;
  bool              havesmpd = false;

  if(m_sttaDcyTrsAndDcyFSts == false)
  {
    m_vDcyTr.clear();
    m_vCcDcyTr.clear();
    m_vIDcyTr.clear();
    m_vICcDcyTr.clear();
    m_vNDcyTr.clear();
    m_vNCcDcyTr.clear();
    m_uomDcyTr.clear();
    m_uomCcDcyTr.clear();
    m_vDcyFSt.clear();
    m_vCcDcyFSt.clear();
    m_vIDcyFSt.clear();
    m_vICcDcyFSt.clear();
    m_vNDcyFSt.clear();
    m_vNCcDcyFSt.clear();
    m_uomDcyFSt.clear();
    m_uomCcDcyFSt.clear();
    m_iDcyTrIDcyFStMap.clear();
    m_iDcyTrICcDcyFStMap.clear();
  }
  if(m_vSigDcyTr.size() > 0)
  {
    m_iSigDcyTrIDcyTrMap.clear();
    m_iSigDcyFStIDcyFStMap.clear();
  }
  if(m_vSigDcyFSt2.size() > 0) { m_iSigDcyFSt2IDcyFStMap.clear(); }

  for(unsigned int i = 0; i < nEtrsToBePrcsd; i++)
  {
    if(openANewOptRootFl == true)
    {
      openANewOptRootFl = false;
      if(iOptRootFls == 0) { NmOfOptRootFl = m_mainNmOfOptFls + ".root"; }
      else
      {
        ostringstream oss;
        oss << iOptRootFls;
        string strIOptRootFls = oss.str();
        NmOfOptRootFl         = m_mainNmOfOptFls + "_" + strIOptRootFls + ".root";
      }
      fl = new TFile(NmOfOptRootFl.c_str(), "recreate");
      if(!fl)
      {
        cerr << "Error: Can't create the output root file \"" << NmOfOptRootFl << "\"!" << endl;
        cerr << "Infor: Please check it." << endl;
        exit(-1);
      }

      tr = chn->CloneTree(0);

      if(m_sttaDcyTrsAndDcyFSts == false)
      {
        tr->Branch("iDcyTr", &iDcyTr, "iDcyTr/I");
        tr->Branch("iDcyFSt", &iDcyFSt, "iDcyFSt/I");
      }
      if(m_ccSwitch == true)
      {
        tr->Branch("iCcDcyTr", &iCcDcyTr, "iCcDcyTr/I");
        tr->Branch("iCcDcyFSt", &iCcDcyFSt, "iCcDcyFSt/I");
      }
      if(m_vPttaPid.size() > 0)
      {
        char specifier1[30], specifier2[30];
        for(unsigned int i = 0; i < m_vPttaPid.size(); i++)
        {
          sprintf(specifier1, "nameP%d", i + 1);
          sprintf(specifier2, "nameP%d/C", i + 1);
          tr->Branch(specifier1, &nameP[i], specifier2);
          strcpy(nameP[i], m_pidTxtPnmMap[m_vPttaPid[i]].c_str());
          if(m_ccSwitch == true)
          {
            sprintf(specifier1, "iCcP%d", i + 1);
            sprintf(specifier2, "iCcP%d/I", i + 1);
            tr->Branch(specifier1, &iCcP[i], specifier2);
            iCcP[i] = m_vPttaICcPid[i];
            if(iCcP[i] == 0)
            {
              sprintf(specifier1, "nP%d", i + 1);
              sprintf(specifier2, "nP%d/I", i + 1);
              tr->Branch(specifier1, &nP[i], specifier2);
              sprintf(specifier1, "iDcyBrP%d", i + 1);
              sprintf(specifier2, "iDcyBrP%d[nP%d]/I", i + 1, i + 1);
              tr->Branch(specifier1, &iDcyBrP[i], specifier2);
              sprintf(specifier1, "iCcDcyBrP%d", i + 1);
              sprintf(specifier2, "iCcDcyBrP%d[nP%d]/I", i + 1, i + 1);
              tr->Branch(specifier1, &iCcDcyBrP[i], specifier2);
            }
            else
            {
              sprintf(specifier1, "nP%d", i + 1);
              sprintf(specifier2, "nP%d/I", i + 1);
              tr->Branch(specifier1, &nP[i], specifier2);
              sprintf(specifier1, "iDcyBrP%d", i + 1);
              sprintf(specifier2, "iDcyBrP%d[nP%d]/I", i + 1, i + 1);
              tr->Branch(specifier1, &iDcyBrP[i], specifier2);
              sprintf(specifier1, "nCcP%d", i + 1);
              sprintf(specifier2, "nCcP%d/I", i + 1);
              tr->Branch(specifier1, &nCcP[i], specifier2);
              sprintf(specifier1, "iDcyBrCcP%d", i + 1);
              sprintf(specifier2, "iDcyBrCcP%d[nCcP%d]/I", i + 1, i + 1);
              tr->Branch(specifier1, &iDcyBrCcP[i], specifier2);
              sprintf(specifier1, "nTotP%d", i + 1);
              sprintf(specifier2, "nTotP%d/I", i + 1);
              tr->Branch(specifier1, &nTotP[i], specifier2);
            }
          }
          else
          {
            sprintf(specifier1, "nP%d", i + 1);
            sprintf(specifier2, "nP%d/I", i + 1);
            tr->Branch(specifier1, &nP[i], specifier2);
            sprintf(specifier1, "iDcyBrP%d", i + 1);
            sprintf(specifier2, "iDcyBrP%d[nP%d]/I", i + 1, i + 1);
            tr->Branch(specifier1, &iDcyBrP[i], specifier2);
          }
        }
      }
      if(m_vPttaIncDcyBr.size() > 0)
      {
        char                specifier1[100], specifier2[100];
        list<int>::iterator liit;
        for(unsigned int i = 0; i < m_vPttaIncDcyBr.size(); i++)
        {
          sprintf(specifier1, "nameIncDcyBr%d", i + 1);
          sprintf(specifier2, "nameIncDcyBr%d/C", i + 1);
          tr->Branch(specifier1, &nameIncDcyBr[i], specifier2);
          // The following statement is used to clear the character array nameIncDcyBr[i].
          strcpy(nameIncDcyBr[i], "");
          liit = m_vPttaIncDcyBr[i].begin();
          strcat(nameIncDcyBr[i], m_pidTxtPnmMap[(*liit)].c_str());
          strcat(nameIncDcyBr[i], " ");
          if((*liit) == -11)
          {
            liit++;
            strcat(nameIncDcyBr[i], m_pidTxtPnmMap[(*liit)].c_str());
            strcat(nameIncDcyBr[i], " ");
          }
          strcat(nameIncDcyBr[i], "-->");
          for(liit++; liit != m_vPttaIncDcyBr[i].end(); liit++)
          {
            strcat(nameIncDcyBr[i], " ");
            strcat(nameIncDcyBr[i], m_pidTxtPnmMap[(*liit)].c_str());
          }
          strcat(nameIncDcyBr[i], " + anything");
          if(m_ccSwitch == true)
          {
            sprintf(specifier1, "iCcIncDcyBr%d", i + 1);
            sprintf(specifier2, "iCcIncDcyBr%d/I", i + 1);
            tr->Branch(specifier1, &iCcIncDcyBr[i], specifier2);
            iCcIncDcyBr[i] = m_vPttaICcPid[i];
            if(iCcIncDcyBr[i] == 0)
            {
              sprintf(specifier1, "nIncDcyBr%d", i + 1);
              sprintf(specifier2, "nIncDcyBr%d/I", i + 1);
              tr->Branch(specifier1, &nIncDcyBr[i], specifier2);
              sprintf(specifier1, "iDcyBrIncDcyBr%d", i + 1);
              sprintf(specifier2, "iDcyBrIncDcyBr%d[nIncDcyBr%d]/I", i + 1, i + 1);
              tr->Branch(specifier1, &iDcyBrIncDcyBr[i], specifier2);
              sprintf(specifier1, "iCcDcyBrIncDcyBr%d", i + 1);
              sprintf(specifier2, "iCcDcyBrIncDcyBr%d[nIncDcyBr%d]/I", i + 1, i + 1);
              tr->Branch(specifier1, &iCcDcyBrIncDcyBr[i], specifier2);
            }
            else
            {
              sprintf(specifier1, "nIncDcyBr%d", i + 1);
              sprintf(specifier2, "nIncDcyBr%d/I", i + 1);
              tr->Branch(specifier1, &nIncDcyBr[i], specifier2);
              sprintf(specifier1, "iDcyBrIncDcyBr%d", i + 1);
              sprintf(specifier2, "iDcyBrIncDcyBr%d[nIncDcyBr%d]/I", i + 1, i + 1);
              tr->Branch(specifier1, &iDcyBrIncDcyBr[i], specifier2);
              sprintf(specifier1, "nCcIncDcyBr%d", i + 1);
              sprintf(specifier2, "nCcIncDcyBr%d/I", i + 1);
              tr->Branch(specifier1, &nCcIncDcyBr[i], specifier2);
              sprintf(specifier1, "iDcyBrCcIncDcyBr%d", i + 1);
              sprintf(specifier2, "iDcyBrCcIncDcyBr%d[nCcIncDcyBr%d]/I", i + 1, i + 1);
              tr->Branch(specifier1, &iDcyBrCcIncDcyBr[i], specifier2);
              sprintf(specifier1, "nTotIncDcyBr%d", i + 1);
              sprintf(specifier2, "nTotIncDcyBr%d/I", i + 1);
              tr->Branch(specifier1, &nTotIncDcyBr[i], specifier2);
            }
          }
          else
          {
            sprintf(specifier1, "nIncDcyBr%d", i + 1);
            sprintf(specifier2, "nIncDcyBr%d/I", i + 1);
            tr->Branch(specifier1, &nIncDcyBr[i], specifier2);
            sprintf(specifier1, "iDcyBrIncDcyBr%d", i + 1);
            sprintf(specifier2, "iDcyBrIncDcyBr%d[nIncDcyBr%d]/I", i + 1, i + 1);
            tr->Branch(specifier1, &iDcyBrIncDcyBr[i], specifier2);
          }
        }
      }
      if(m_vSigPid.size() > 0)
      {
        char specifier[30];
        if(m_ccSwitch == true)
        {
          sprintf(specifier, "iCcSigP[%d]/I", int(sSigP));
          tr->Branch("iCcSigP", &iCcSigP, specifier);
          for(unsigned int i = 0; i < sSigP; i++) iCcSigP[i] = m_vICcSigP[i];
          sprintf(specifier, "nSigP[%d]/I", int(sSigP));
          tr->Branch("nSigP", &nSigP, specifier);
          sprintf(specifier, "nCcSigP[%d]/I", int(sSigP));
          tr->Branch("nCcSigP", &nCcSigP, specifier);
          sprintf(specifier, "nTotSigP[%d]/I", int(sSigP));
          tr->Branch("nTotSigP", &nTotSigP, specifier);
        }
        else
        {
          sprintf(specifier, "nSigP[%d]/I", int(sSigP));
          tr->Branch("nSigP", &nSigP, specifier);
        }
      }
      if(m_vSigIncDcyBr.size() > 0)
      {
        char specifier[30];
        if(m_ccSwitch == true)
        {
          sprintf(specifier, "iCcSigIncDcyBr[%d]/I", int(sSigIncDcyBr));
          tr->Branch("iCcSigIncDcyBr", &iCcSigIncDcyBr, specifier);
          for(unsigned int i = 0; i < sSigIncDcyBr; i++) iCcSigIncDcyBr[i] = m_vICcSigIncDcyBr[i];
          sprintf(specifier, "nSigIncDcyBr[%d]/I", int(sSigIncDcyBr));
          tr->Branch("nSigIncDcyBr", &nSigIncDcyBr, specifier);
          sprintf(specifier, "nCcSigIncDcyBr[%d]/I", int(sSigIncDcyBr));
          tr->Branch("nCcSigIncDcyBr", &nCcSigIncDcyBr, specifier);
          sprintf(specifier, "nTotSigIncDcyBr[%d]/I", int(sSigIncDcyBr));
          tr->Branch("nTotSigIncDcyBr", &nTotSigIncDcyBr, specifier);
        }
        else
        {
          sprintf(specifier, "nSigIncDcyBr[%d]/I", int(sSigIncDcyBr));
          tr->Branch("nSigIncDcyBr", &nSigIncDcyBr, specifier);
        }
      }
      if(m_vSigSeqDcyBrs.size() > 0)
      {
        char specifier[30];
        if(m_ccSwitch == true)
        {
          sprintf(specifier, "iCcSigSeqDcyBrs[%d]/I", int(sSigSeqDcyBrs));
          tr->Branch("iCcSigSeqDcyBrs", &iCcSigSeqDcyBrs, specifier);
          for(unsigned int i = 0; i < sSigSeqDcyBrs; i++)
            iCcSigSeqDcyBrs[i] = m_vICcSigSeqDcyBrs[i];
          sprintf(specifier, "nSigSeqDcyBrs[%d]/I", int(sSigSeqDcyBrs));
          tr->Branch("nSigSeqDcyBrs", &nSigSeqDcyBrs, specifier);
          sprintf(specifier, "nCcSigSeqDcyBrs[%d]/I", int(sSigSeqDcyBrs));
          tr->Branch("nCcSigSeqDcyBrs", &nCcSigSeqDcyBrs, specifier);
          sprintf(specifier, "nTotSigSeqDcyBrs[%d]/I", int(sSigSeqDcyBrs));
          tr->Branch("nTotSigSeqDcyBrs", &nTotSigSeqDcyBrs, specifier);
        }
        else
        {
          sprintf(specifier, "nSigSeqDcyBrs[%d]/I", int(sSigSeqDcyBrs));
          tr->Branch("nSigSeqDcyBrs", &nSigSeqDcyBrs, specifier);
        }
      }
      if(m_vSigIncSeqDcyBrs.size() > 0)
      {
        char specifier[30];
        if(m_ccSwitch == true)
        {
          sprintf(specifier, "iCcSigIncSeqDcyBrs[%d]/I", int(sSigIncSeqDcyBrs));
          tr->Branch("iCcSigIncSeqDcyBrs", &iCcSigIncSeqDcyBrs, specifier);
          for(unsigned int i = 0; i < sSigIncSeqDcyBrs; i++)
            iCcSigIncSeqDcyBrs[i] = m_vICcSigIncSeqDcyBrs[i];
          sprintf(specifier, "nSigIncSeqDcyBrs[%d]/I", int(sSigIncSeqDcyBrs));
          tr->Branch("nSigIncSeqDcyBrs", &nSigIncSeqDcyBrs, specifier);
          sprintf(specifier, "nCcSigIncSeqDcyBrs[%d]/I", int(sSigIncSeqDcyBrs));
          tr->Branch("nCcSigIncSeqDcyBrs", &nCcSigIncSeqDcyBrs, specifier);
          sprintf(specifier, "nTotSigIncSeqDcyBrs[%d]/I", int(sSigIncSeqDcyBrs));
          tr->Branch("nTotSigIncSeqDcyBrs", &nTotSigIncSeqDcyBrs, specifier);
        }
        else
        {
          sprintf(specifier, "nSigIncSeqDcyBrs[%d]/I", int(sSigIncSeqDcyBrs));
          tr->Branch("nSigIncSeqDcyBrs", &nSigIncSeqDcyBrs, specifier);
        }
      }
      if(m_vSigIRADcyBr.size() > 0)
      {
        char specifier[30];
        if(m_ccSwitch == true)
        {
          sprintf(specifier, "iCcSigIRADcyBr[%d]/I", int(sSigIRADcyBr));
          tr->Branch("iCcSigIRADcyBr", &iCcSigIRADcyBr, specifier);
          for(unsigned int i = 0; i < sSigIRADcyBr; i++) iCcSigIRADcyBr[i] = m_vICcSigIRADcyBr[i];
          sprintf(specifier, "nSigIRADcyBr[%d]/I", int(sSigIRADcyBr));
          tr->Branch("nSigIRADcyBr", &nSigIRADcyBr, specifier);
          sprintf(specifier, "nCcSigIRADcyBr[%d]/I", int(sSigIRADcyBr));
          tr->Branch("nCcSigIRADcyBr", &nCcSigIRADcyBr, specifier);
          sprintf(specifier, "nTotSigIRADcyBr[%d]/I", int(sSigIRADcyBr));
          tr->Branch("nTotSigIRADcyBr", &nTotSigIRADcyBr, specifier);
        }
        else
        {
          sprintf(specifier, "nSigIRADcyBr[%d]/I", int(sSigIRADcyBr));
          tr->Branch("nSigIRADcyBr", &nSigIRADcyBr, specifier);
        }
      }
      if(m_vSigIncOrIRASeqDcyBrs.size() > 0)
      {
        char specifier[50];
        if(m_ccSwitch == true)
        {
          sprintf(specifier, "iCcSigIncOrIRASeqDcyBrs[%d]/I", int(sSigIncOrIRASeqDcyBrs));
          tr->Branch("iCcSigIncOrIRASeqDcyBrs", &iCcSigIncOrIRASeqDcyBrs, specifier);
          for(unsigned int i = 0; i < sSigIncOrIRASeqDcyBrs; i++)
            iCcSigIncOrIRASeqDcyBrs[i] = m_vICcSigIncOrIRASeqDcyBrs[i];
          sprintf(specifier, "nSigIncOrIRASeqDcyBrs[%d]/I", int(sSigIncOrIRASeqDcyBrs));
          tr->Branch("nSigIncOrIRASeqDcyBrs", &nSigIncOrIRASeqDcyBrs, specifier);
          sprintf(specifier, "nCcSigIncOrIRASeqDcyBrs[%d]/I", int(sSigIncOrIRASeqDcyBrs));
          tr->Branch("nCcSigIncOrIRASeqDcyBrs", &nCcSigIncOrIRASeqDcyBrs, specifier);
          sprintf(specifier, "nTotSigIncOrIRASeqDcyBrs[%d]/I", int(sSigIncOrIRASeqDcyBrs));
          tr->Branch("nTotSigIncOrIRASeqDcyBrs", &nTotSigIncOrIRASeqDcyBrs, specifier);
        }
        else
        {
          sprintf(specifier, "nSigIncOrIRASeqDcyBrs[%d]/I", int(sSigIncOrIRASeqDcyBrs));
          tr->Branch("nSigIncOrIRASeqDcyBrs", &nSigIncOrIRASeqDcyBrs, specifier);
        }
      }
      if(m_vSigDcyTr.size() > 0)
      {
        tr->Branch("iSigDcyTr", &iSigDcyTr, "iSigDcyTr/I");
        tr->Branch("iSigDcyFSt", &iSigDcyFSt, "iSigDcyFSt/I");
        if(m_ccSwitch == true)
        {
          tr->Branch("iCcSigDcyTr", &iCcSigDcyTr, "iCcSigDcyTr/I");
          tr->Branch("iCcSigDcyFSt", &iCcSigDcyFSt, "iCcSigDcyFSt/I");
        }
      }
      if(m_vSigDcyFSt2.size() > 0)
      {
        tr->Branch("iSigDcyFSt2", &iSigDcyFSt2, "iSigDcyFSt2/I");
        if(m_ccSwitch == true) { tr->Branch("iCcSigDcyFSt2", &iCcSigDcyFSt2, "iCcSigDcyFSt2/I"); }
      }
    }

    if(i == 0) starttime = clock();

    chn->GetEntry(i);

    if(!m_cut.empty())
    {
      // The following four statements are used to handle the cases where array variables are used
      // in the cut.
      nInsts = trfml->GetNdata(); // This statement is indispensable if multiple instances have to
                                  // be evaluated by the object of the TTreeFormula class.
      bool passTheCut;
      if(m_cutMethod == true)
      {
        passTheCut = false;
        for(unsigned int j = 0; j < nInsts; j++)
          if(trfml->EvalInstance(j))
          {
            passTheCut = true;
            break;
          }
      }
      else
      {
        passTheCut = true;
        for(unsigned int j = 0; j < nInsts; j++)
          if(!(trfml->EvalInstance(j)))
          {
            passTheCut = false;
            break;
          }
      }
      if(!passTheCut)
      {
        if((i + 1 >= nEtrsForTiming) &&
           (((i + 1) % nEtrsForTiming == 0) || ((i + 1) == nEtrsToBePrcsd)))
        {
          if((i + 1) == nEtrsForTiming)
            cout << "Number of entries processed\tNumber of seconds elapsed" << endl << endl;
          cout << setiosflags(ios::right) << setw(14) << i + 1 << "\t\t\t"
               << setiosflags(ios::fixed) << setprecision(2) << setw(14)
               << (clock() - starttime) / ((double)CLOCKS_PER_SEC)
               << resetiosflags(ios::adjustfield) << endl;
          if((i + 1) == nEtrsToBePrcsd)
          {
            cout << endl
                 << "Note that only " << nEtrsThroughTheCut << " entries passed the cut." << endl
                 << endl;
          }
        }
        if((i + 1) == nEtrsToBePrcsd)
        {
          fl->Write();
          delete tr; // Pay attention to that replacing the "delete tr" by "tr->Delete()" will
                     // result in a problem of "*** Break *** segmentation violation".
          fl->Close();
          delete fl;
          for(unsigned int j = 0; j < Nasterisks; j++) cout << "*";
          cout << endl << endl;
        }
        continue;
      }
      else
      {
        nEtrsThroughTheCut++;
      }
    }

    vPid.clear();
    vMidx.clear();
    for(int j = 0; j < Nps; j++)
    {
      vPid.push_back(Pid[j]);
      vMidx.push_back(Midx[j]);
      // cout<<j<<"\t"<<Pid[j]<<"\t"<<Midx[j]<<endl;
    }
    // cout<<endl;
    /*for(unsigned int j=0;j<vPid.size();j++)
      {
        cout<<j<<"\t"<<vPid[j]<<"\t"<<vMidx[j]<<endl;
      }
    cout<<endl;*/
    sortPs(vPid, vMidx);
    dcyTr.clear();
    vIdxOfHead.clear();
    vMidxOfHead.clear();
    getDcyTr(vPid, vMidx, dcyTr, vIdxOfHead, vMidxOfHead);
    oss.str("");
    list<int> dcyBr;
    for(unsigned int j = 0; j < dcyTr.size(); j++)
    {
      if(j == 0 && m_spr1stBrsOfDcyTrs == true && dcyTr[0].size() == 3) continue;
      dcyBr.clear();
      dcyBr                    = dcyTr[j];
      list<int>::iterator liit = dcyBr.begin();
      oss << (*liit) << " ";
      // The condition "j==0" is set for the initial state particle pair e+e-;the condition
      // "(*liit)==1, 2, 3, 4, 5 or 6" is set for the intermediate state quark pair ddbar, uubar,
      // ssbar, ccbar, bbbar or ttbar;
      if(j == 0 || (*liit) == 1 || (*liit) == 2 || (*liit) == 3 || (*liit) == 4 || (*liit) == 5 ||
         (*liit) == 6)
      {
        liit++;
        oss << (*liit) << " ";
      }
      oss << "> ";
      list<int>::iterator liitTmp = dcyBr.end();
      liitTmp--;
      for(liit++; liit != liitTmp; liit++) oss << (*liit) << " ";
      oss << (*liit);
      if(j != (dcyTr.size() - 1)) oss << ", ";
    }
    strDcyTr = oss.str();
    // cout<<i<<"\t"<<strDcyTr<<endl<<endl;
    dcyFSt.clear();
    getDcyFSt(vPid, vMidx, dcyFSt);
    oss.str("");
    list<int>::iterator liit;
    if(m_spr1stBrsOfDcyTrs == true && dcyTr[0].size() == 3)
    {
      liit = dcyTr[0].begin();
      liit++;
      liit++;
      oss << (*liit) << " ";
      liit = dcyFSt.begin();
      liit++;
    }
    else
    {
      liit = dcyFSt.begin();
      oss << (*liit) << " ";
      liit++;
      oss << (*liit) << " ";
    }
    oss << "> ";
    list<int>::iterator liitTmp = dcyFSt.end();
    liitTmp--;
    for(liit++; liit != liitTmp; liit++) oss << (*liit) << " ";
    oss << (*liit);
    strDcyFSt = oss.str();
    // cout<<i<<"\t"<<strDcyFSt<<endl<<endl;
    if(m_ccSwitch == true)
    {
      vCcPid.clear();
      vCcMidx.clear();
      for(unsigned int j = 0; j < vPid.size(); j++) vCcPid.push_back(getCcPid(vPid[j]));
      vCcMidx = vMidx;
      sortPs(vCcPid, vCcMidx);
      ccDcyTr.clear();
      vCcIdxOfHead.clear();
      vCcMidxOfHead.clear();
      getDcyTr(vCcPid, vCcMidx, ccDcyTr, vCcIdxOfHead, vCcMidxOfHead);
      oss.str("");
      list<int> ccDcyBr;
      for(unsigned int j = 0; j < ccDcyTr.size(); j++)
      {
        if(j == 0 && m_spr1stBrsOfDcyTrs == true && ccDcyTr[0].size() == 3) continue;
        ccDcyBr.clear();
        ccDcyBr                  = ccDcyTr[j];
        list<int>::iterator liit = ccDcyBr.begin();
        oss << (*liit) << " ";
        // The condition "j==0" is set for the initial state particle pair e+e-;the condition
        // "(*liit)==1, 2, 3, 4, 5 or 6" is set for the intermediate state quark pair ddbar, uubar,
        // ssbar, ccbar, bbbar or ttbar;
        if(j == 0 || (*liit) == 1 || (*liit) == 2 || (*liit) == 3 || (*liit) == 4 || (*liit) == 5 ||
           (*liit) == 6)
        {
          liit++;
          oss << (*liit) << " ";
        }
        oss << "> ";
        list<int>::iterator liitTmp = ccDcyBr.end();
        liitTmp--;
        for(liit++; liit != liitTmp; liit++) oss << (*liit) << " ";
        oss << (*liit);
        if(j != (ccDcyTr.size() - 1)) oss << ", ";
      }
      strCcDcyTr = oss.str();
      // cout<<i<<"\t"<<strCcDcyTr<<endl<<endl;
      ccDcyFSt.clear();
      getDcyFSt(vCcPid, vCcMidx, ccDcyFSt);
      oss.str("");
      list<int>::iterator liit;
      if(m_spr1stBrsOfDcyTrs == true && ccDcyTr[0].size() == 3)
      {
        liit = ccDcyTr[0].begin();
        liit++;
        liit++;
        oss << (*liit) << " ";
        liit = ccDcyFSt.begin();
        liit++;
      }
      else
      {
        liit = ccDcyFSt.begin();
        oss << (*liit) << " ";
        liit++;
        oss << (*liit) << " ";
      }
      oss << "> ";
      list<int>::iterator liitTmp = ccDcyFSt.end();
      liitTmp--;
      for(liit++; liit != liitTmp; liit++) oss << (*liit) << " ";
      oss << (*liit);
      strCcDcyFSt = oss.str();
      // cout<<i<<"\t"<<strCcDcyFSt<<endl<<endl;
    }

    if(m_sttaDcyTrsAndDcyFSts == false)
    {
      if((m_nDcyTrsToBeAnlzdMax == ULONG_MAX) ||
         ((m_nDcyTrsToBeAnlzdMax != ULONG_MAX) && (havesmpd == false) && (i < m_nEtrsOfCtrSmpMax)))
      {
        if((m_nDcyTrsToBeAnlzdMax != ULONG_MAX) && (havesmpd == false) && (i == 0))
          cout << "Begin sampling for top " << m_nDcyTrsToBeAnlzdMax << " decay trees ..." << endl
               << endl;
        iDcyTr = -1; // If the variable is still equal to -1 after the following loop, then the
                     // decay tree is a new decay tree.
        // To get a higher search efficiency, the following code segment is replaced by the next
        // one, which is featured by the use of "unordered_map".
        /*for(unsigned int j=0;j<m_vDcyTr.size();j++)
          {
            if(dcyTr==m_vDcyTr[j])
              {
                iDcyTr=j;
                iDcyFSt=m_iDcyTrIDcyFStMap[iDcyTr];
                if(m_ccSwitch==true)
                  {
                    iCcDcyTr=m_vICcDcyTr[j];
                    iCcDcyFSt=m_iDcyTrICcDcyFStMap[iDcyTr];
                  }
                break;
              }
            else if(m_ccSwitch==true&&m_vICcDcyTr[j]!=0&&dcyTr==m_vCcDcyTr[j])
              {
                iDcyTr=j;
                iDcyFSt=m_iDcyTrIDcyFStMap[iDcyTr];
                iCcDcyTr=-1;
                iCcDcyFSt=-1*m_iDcyTrICcDcyFStMap[iDcyTr];
                break;
              }
          }*/
        if(m_uomDcyTr.find(strDcyTr) != m_uomDcyTr.end())
        {
          iDcyTr  = m_uomDcyTr[strDcyTr];
          iDcyFSt = m_iDcyTrIDcyFStMap[iDcyTr];
          if(m_ccSwitch == true)
          {
            iCcDcyTr  = m_vICcDcyTr[iDcyTr];
            iCcDcyFSt = m_iDcyTrICcDcyFStMap[iDcyTr];
          }
        }
        else if(m_ccSwitch == true && m_uomCcDcyTr.find(strDcyTr) != m_uomCcDcyTr.end())
        {
          iDcyTr    = m_uomCcDcyTr[strDcyTr];
          iDcyFSt   = m_iDcyTrIDcyFStMap[iDcyTr];
          iCcDcyTr  = -1;
          iCcDcyFSt = -1 * m_iDcyTrICcDcyFStMap[iDcyTr];
        }
        if(iDcyTr == -1)
        {
          iDcyTr = m_vDcyTr.size();
          m_vDcyTr.push_back(dcyTr);
          m_vIDcyTr.push_back(iDcyTr);
          m_vNDcyTr.push_back(1);
          m_uomDcyTr[strDcyTr] = iDcyTr;
          if(m_ccSwitch == true)
          {
            if(ccDcyTr == dcyTr)
              iCcDcyTr = 0;
            else
              iCcDcyTr = 1;
            m_vCcDcyTr.push_back(ccDcyTr);
            m_vICcDcyTr.push_back(iCcDcyTr);
            m_vNCcDcyTr.push_back(0);
            m_uomCcDcyTr[strCcDcyTr] = iDcyTr;
          }

          iDcyFSt = -1; // If the variable is still equal to -1 after the following loop, then the
                        // decay final states is a new decay final states.
          // To get a higher search efficiency, the following code segment is replaced by the next
          // one, which is featured by the use of "unordered_map".
          /*for(unsigned int j=0;j<m_vDcyFSt.size();j++)
            {
              if(dcyFSt==m_vDcyFSt[j])
                {
                  iDcyFSt=j;
                  if(m_ccSwitch==true) iCcDcyFSt=m_vICcDcyFSt[j];
                  break;
                }
              else if(m_ccSwitch==true&&m_vICcDcyFSt[j]!=0&&dcyFSt==m_vCcDcyFSt[j])
                {
                  iDcyFSt=j;
                  iCcDcyFSt=-1;
                  break;
                }
            }*/
          if(m_uomDcyFSt.find(strDcyFSt) != m_uomDcyFSt.end())
          {
            iDcyFSt = m_uomDcyFSt[strDcyFSt];
            if(m_ccSwitch == true) iCcDcyFSt = m_vICcDcyFSt[iDcyFSt];
          }
          else if(m_ccSwitch == true && m_uomCcDcyFSt.find(strDcyFSt) != m_uomCcDcyFSt.end())
          {
            iDcyFSt   = m_uomCcDcyFSt[strDcyFSt];
            iCcDcyFSt = -1;
          }
          if(iDcyFSt == -1)
          {
            iDcyFSt = m_vDcyFSt.size();
            m_vDcyFSt.push_back(dcyFSt);
            m_vIDcyFSt.push_back(iDcyFSt);
            m_vNDcyFSt.push_back(1);
            m_uomDcyFSt[strDcyFSt] = iDcyFSt;
            if(m_ccSwitch == true)
            {
              if(ccDcyFSt == dcyFSt)
                iCcDcyFSt = 0;
              else
                iCcDcyFSt = 1;
              m_vCcDcyFSt.push_back(ccDcyFSt);
              m_vICcDcyFSt.push_back(iCcDcyFSt);
              m_vNCcDcyFSt.push_back(0);
              m_uomCcDcyFSt[strCcDcyFSt] = iDcyFSt;
            }
          }
          else
          {
            if(m_ccSwitch == true)
            {
              if(iCcDcyFSt == -1)
                m_vNCcDcyFSt[iDcyFSt]++;
              else
                m_vNDcyFSt[iDcyFSt]++;
            }
            else
              m_vNDcyFSt[iDcyFSt]++;
          }
          m_iDcyTrIDcyFStMap[iDcyTr] = iDcyFSt;
          if(m_ccSwitch == true) m_iDcyTrICcDcyFStMap[iDcyTr] = iCcDcyFSt;
        }
        else
        {
          if(m_ccSwitch == true)
          {
            if(iCcDcyTr == -1)
              m_vNCcDcyTr[iDcyTr]++;
            else
              m_vNDcyTr[iDcyTr]++;
            if(iCcDcyFSt == -1)
              m_vNCcDcyFSt[iDcyFSt]++;
            else
              m_vNDcyFSt[iDcyFSt]++;
          }
          else
          {
            m_vNDcyTr[iDcyTr]++;
            m_vNDcyFSt[iDcyFSt]++;
          }
        }

        if((m_nDcyTrsToBeAnlzdMax != ULONG_MAX) && (i == m_nEtrsOfCtrSmpMax - 1))
        {
          if(m_ccSwitch == true)
          {
            sortBySumOf1stAnd2ndFromLrgToSml(m_vNDcyTr, m_vNCcDcyTr, m_vDcyTr, m_vCcDcyTr,
                                             m_vIDcyTr, m_vICcDcyTr);
            sortBySumOf1stAnd2ndFromLrgToSml(m_vNDcyFSt, m_vNCcDcyFSt, m_vDcyFSt, m_vCcDcyFSt,
                                             m_vIDcyFSt, m_vICcDcyFSt);
          }
          else
          {
            sortBy1stFromLrgToSml(m_vNDcyTr, m_vDcyTr, m_vIDcyTr);
            sortBy1stFromLrgToSml(m_vNDcyFSt, m_vDcyFSt, m_vIDcyFSt);
          }
          unsigned long nDcyTrsToBeAnlzd =
            m_nDcyTrsToBeAnlzdMax < m_vNDcyTr.size() ? m_nDcyTrsToBeAnlzdMax : m_vNDcyTr.size();
          if(nDcyTrsToBeAnlzd < m_vNDcyTr.size())
          {
            vector<vector<list<int>>>::iterator it_m_vDcyTr_b = m_vDcyTr.begin();
            vector<vector<list<int>>>::iterator it_m_vDcyTr_e = m_vDcyTr.end();
            m_vDcyTr.erase(it_m_vDcyTr_b + nDcyTrsToBeAnlzd, it_m_vDcyTr_e);
            vector<int> m_vIDcyFSt_temp;
            m_vIDcyFSt_temp.clear();
            vector<list<int>> m_vDcyFSt_new;
            m_vDcyFSt_new.clear();
            map<int, int> m_iDcyTrIDcyFStMap_new;
            m_iDcyTrIDcyFStMap_new.clear();
            int k_j;
            int l_k;
            for(unsigned int j = 0; j < nDcyTrsToBeAnlzd; j++)
            {
              k_j = -1;
              for(unsigned int k = 0; k < m_vIDcyFSt.size(); k++)
              {
                if(m_vIDcyFSt[k] == m_iDcyTrIDcyFStMap[m_vIDcyTr[j]])
                {
                  k_j = k;
                  break;
                }
              }
              l_k = -1;
              for(unsigned int l = 0; l < m_vIDcyFSt_temp.size(); l++)
              {
                if(m_vIDcyFSt[k_j] == m_vIDcyFSt_temp[l])
                {
                  l_k = l;
                  break;
                }
              }
              if(l_k == -1)
              {
                l_k = m_vIDcyFSt_temp.size();
                m_vIDcyFSt_temp.push_back(m_vIDcyFSt[k_j]);
                m_vDcyFSt_new.push_back(m_vDcyFSt[k_j]);
                m_iDcyTrIDcyFStMap_new[j] = l_k;
              }
              else
              {
                m_iDcyTrIDcyFStMap_new[j] = l_k;
              }
            }
            m_vDcyFSt          = m_vDcyFSt_new;
            m_iDcyTrIDcyFStMap = m_iDcyTrIDcyFStMap_new;

            m_vIDcyTr.clear();
            m_vNDcyTr.clear();
            for(unsigned int j = 0; j < nDcyTrsToBeAnlzd; j++)
            {
              m_vIDcyTr.push_back(j);
              m_vNDcyTr.push_back(0);
            }

            m_vIDcyFSt.clear();
            m_vNDcyFSt.clear();
            for(unsigned int j = 0; j < m_vIDcyFSt_temp.size(); j++)
            {
              m_vIDcyFSt.push_back(j);
              m_vNDcyFSt.push_back(0);
            }

            if(m_ccSwitch == true)
            {
              vector<vector<list<int>>>::iterator it_m_vCcDcyTr_b = m_vCcDcyTr.begin();
              vector<vector<list<int>>>::iterator it_m_vCcDcyTr_e = m_vCcDcyTr.end();
              m_vCcDcyTr.erase(it_m_vCcDcyTr_b + nDcyTrsToBeAnlzd, it_m_vCcDcyTr_e);
              vector<int>::iterator it_m_vICcDcyTr_b = m_vICcDcyTr.begin();
              vector<int>::iterator it_m_vICcDcyTr_e = m_vICcDcyTr.end();
              m_vICcDcyTr.erase(it_m_vICcDcyTr_b + nDcyTrsToBeAnlzd, it_m_vICcDcyTr_e);
              vector<int> m_vIDcyFSt_temp;
              m_vIDcyFSt_temp.clear();
              vector<list<int>> m_vCcDcyFSt_new;
              m_vCcDcyFSt_new.clear();
              vector<int> m_vICcDcyFSt_new;
              m_vICcDcyFSt_new.clear();
              map<int, int> m_iDcyTrICcDcyFStMap_new;
              m_iDcyTrICcDcyFStMap_new.clear();
              int k_j;
              int l_k;
              for(unsigned int j = 0; j < nDcyTrsToBeAnlzd; j++)
              {
                k_j = -1;
                for(unsigned int k = 0; k < m_vIDcyFSt.size(); k++)
                {
                  if(m_vIDcyFSt[k] == m_iDcyTrIDcyFStMap[m_vIDcyTr[j]])
                  {
                    k_j = k;
                    break;
                  }
                }
                l_k = -1;
                for(unsigned int l = 0; l < m_vIDcyFSt_temp.size(); l++)
                {
                  if(m_vIDcyFSt[k_j] == m_vIDcyFSt_temp[l])
                  {
                    l_k = l;
                    break;
                  }
                }
                if(l_k == -1)
                {
                  m_vIDcyFSt_temp.push_back(m_vIDcyFSt[k_j]);
                  m_vCcDcyFSt_new.push_back(m_vCcDcyFSt[k_j]);
                  m_vICcDcyFSt_new.push_back(m_vICcDcyFSt[k_j]);
                  m_iDcyTrICcDcyFStMap_new[j] = m_iDcyTrICcDcyFStMap[m_vIDcyTr[j]];
                }
                else
                {
                  m_iDcyTrICcDcyFStMap_new[j] = m_iDcyTrICcDcyFStMap[m_vIDcyTr[j]];
                }
              }
              m_vCcDcyFSt          = m_vCcDcyFSt_new;
              m_vICcDcyFSt         = m_vICcDcyFSt_new;
              m_iDcyTrICcDcyFStMap = m_iDcyTrICcDcyFStMap_new;

              m_vNCcDcyTr.clear();
              for(unsigned int j = 0; j < nDcyTrsToBeAnlzd; j++) { m_vNCcDcyTr.push_back(0); }

              m_vNCcDcyFSt.clear();
              for(unsigned int j = 0; j < m_vIDcyFSt_temp.size(); j++)
              { m_vNCcDcyFSt.push_back(0); } }
          }
          i        = 0;
          havesmpd = true;
          cout << "End sampling for top " << m_nDcyTrsToBeAnlzdMax << " decay trees ..." << endl
               << endl;
        }
      }
      else
      {
        iDcyTr  = -1;
        iDcyFSt = -1;
        if(m_ccSwitch == true)
        {
          iCcDcyTr  = -9999;
          iCcDcyFSt = -9999;
        }
        for(unsigned int j = 0; j < m_vDcyTr.size(); j++)
        {

          if(dcyTr == m_vDcyTr[j])
          {
            iDcyTr  = j;
            iDcyFSt = m_iDcyTrIDcyFStMap[iDcyTr];
            m_vNDcyTr[iDcyTr]++;
            m_vNDcyFSt[iDcyFSt]++;
            if(m_ccSwitch == true)
            {
              iCcDcyTr  = m_vICcDcyTr[j];
              iCcDcyFSt = m_iDcyTrICcDcyFStMap[iDcyTr];
            }
            break;
          }
          else if(m_ccSwitch == true && m_vICcDcyTr[j] != 0 && dcyTr == m_vCcDcyTr[j])
          {
            iDcyTr  = j;
            iDcyFSt = m_iDcyTrIDcyFStMap[iDcyTr];
            m_vNCcDcyTr[iDcyTr]++;
            m_vNCcDcyFSt[iDcyFSt]++;
            iCcDcyTr  = -1;
            iCcDcyFSt = -1 * m_iDcyTrICcDcyFStMap[iDcyTr];
            break;
          }
        }
      }
    }

    if(m_vPttaPid.size() > 0)
    {
      // Pay attention to that dcyBrCcP equate to ccDcyBrP for self-charge-conjugate particles.
      list<int> dcyBrP, dcyBrCcP;
      dcyBrP.clear();
      dcyBrCcP.clear();
      for(unsigned int j = 0; j < m_vPttaPid.size(); j++)
      {
        nP[j] = 0;
        if(m_ccSwitch == true && m_vPttaICcPid[j] != 0)
        {
          nCcP[j]  = 0;
          nTotP[j] = 0;
        }
      }
      for(unsigned int j = 1; j < dcyTr.size(); j++)
      {
        list<int>::iterator liit = dcyTr[j].begin();
        for(unsigned int k = 0; k < m_vPttaPid.size(); k++)
        {
          if((*liit) == m_vPttaPid[k])
          {
            dcyBrP = dcyTr[j];
            int _iDcyBrP =
              -1; // If the variable is still equal to -1 after the following loop, then the decay
                  // branch of the particle is a new decay branch of the particle.
            int _iCcDcyBrP = -9999;
            for(unsigned int l = 0; l < m_vVDcyBrP[k].size(); l++)
            {
              if(dcyBrP == m_vVDcyBrP[k][l])
              {
                _iDcyBrP = l;
                if(m_ccSwitch == true && m_vPttaICcPid[k] == 0) _iCcDcyBrP = m_vVIDcyBrCcP[k][l];
                break;
              }
              else if(m_ccSwitch == true && m_vPttaICcPid[k] == 0 && m_vVIDcyBrCcP[k][l] != 0 &&
                      dcyBrP == m_vVDcyBrCcP[k][l])
              {
                _iDcyBrP   = l;
                _iCcDcyBrP = -1;
                break;
              }
            }
            if(_iDcyBrP == -1)
            {
              _iDcyBrP                          = m_vVDcyBrP[k].size();
              iDcyBrP[k][(unsigned int)(nP[k])] = _iDcyBrP;
              m_vVDcyBrP[k].push_back(dcyBrP);
              m_vVIDcyBrP[k].push_back(_iDcyBrP);
              m_vVNDcyBrP[k].push_back(1);
              if(m_ccSwitch == true)
              {
                dcyBrCcP.clear();
                list<int>::iterator liit = dcyBrP.begin();
                for(liit++; liit != dcyBrP.end(); liit++) dcyBrCcP.push_back(getCcPid((*liit)));
                sortByPidAndPchrg(dcyBrCcP);
                liit = dcyBrP.begin();
                if(m_vPttaICcPid[k] != 0)
                {
                  dcyBrCcP.push_front(
                    getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                  m_vVDcyBrCcP[k].push_back(dcyBrCcP);
                  m_vVIDcyBrCcP[k].push_back(_iDcyBrP);
                  m_vVNDcyBrCcP[k].push_back(0);
                }
                else
                {
                  dcyBrCcP.push_front((*liit)); // Here, (*liit) is equal to getCcPid((*liit)).
                  if(dcyBrCcP == dcyBrP)
                    _iCcDcyBrP = 0;
                  else
                    _iCcDcyBrP = 1;
                  iCcDcyBrP[k][(unsigned int)(nP[k])] = _iCcDcyBrP;
                  m_vVDcyBrCcP[k].push_back(dcyBrCcP);
                  m_vVIDcyBrCcP[k].push_back(_iCcDcyBrP);
                  m_vVNDcyBrCcP[k].push_back(0);
                }
              }
            }
            else
            {
              iDcyBrP[k][(unsigned int)(nP[k])] = _iDcyBrP;
              if(m_ccSwitch == true && m_vPttaICcPid[k] == 0)
              {
                iCcDcyBrP[k][(unsigned int)(nP[k])] = _iCcDcyBrP;
                if(_iCcDcyBrP == -1)
                  m_vVNDcyBrCcP[k][_iDcyBrP]++;
                else
                  m_vVNDcyBrP[k][_iDcyBrP]++;
              }
              else
              {
                m_vVNDcyBrP[k][_iDcyBrP]++;
              }
            }
            nP[k]++;
            break;
          }
          else if(m_ccSwitch == true && (*liit) == m_vPttaCcPid[k])
          {
            dcyBrCcP = dcyTr[j];
            int _iDcyBrP =
              -1; // If the variable is still equal to -1 after the following loop, then the decay
                  // branch of the particle is a new decay branch of the particle.
            for(unsigned int l = 0; l < m_vVDcyBrCcP[k].size(); l++)
            {
              if(dcyBrCcP == m_vVDcyBrCcP[k][l])
              {
                _iDcyBrP = l;
                break;
              }
            }
            if(_iDcyBrP == -1)
            {
              _iDcyBrP                              = m_vVDcyBrP[k].size();
              iDcyBrCcP[k][(unsigned int)(nCcP[k])] = _iDcyBrP;
              m_vVDcyBrCcP[k].push_back(dcyBrCcP);
              m_vVIDcyBrCcP[k].push_back(_iDcyBrP);
              m_vVNDcyBrCcP[k].push_back(1);

              list<int> dcyBrP;
              dcyBrP.clear();
              list<int>::iterator liit = dcyBrCcP.begin();
              for(liit++; liit != dcyBrCcP.end(); liit++) dcyBrP.push_back(getCcPid((*liit)));
              sortByPidAndPchrg(dcyBrP);
              liit = dcyBrCcP.begin();
              dcyBrP.push_front(
                getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
              m_vVDcyBrP[k].push_back(dcyBrP);
              m_vVIDcyBrP[k].push_back(_iDcyBrP);
              m_vVNDcyBrP[k].push_back(0);
            }
            else
            {
              iDcyBrCcP[k][(unsigned int)(nCcP[k])] = _iDcyBrP;
              m_vVNDcyBrCcP[k][_iDcyBrP]++;
            }
            nCcP[k]++;
            break;
          }
        }
      }
      if(m_ccSwitch == true)
        for(unsigned int j = 0; j < m_vPttaPid.size(); j++)
          if(m_vPttaICcPid[j] != 0) nTotP[j] = nP[j] + nCcP[j];
    }

    if(m_vPttaIncDcyBr.size() > 0)
    {
      // Pay attention to that dcyBrCcIncDcyBr equate to ccDcyBrIncDcyBr for self-charge-conjugate
      // inclusive decays.
      list<int> dcyBrIncDcyBr, dcyBrCcIncDcyBr;
      dcyBrIncDcyBr.clear();
      dcyBrCcIncDcyBr.clear();
      for(unsigned int j = 0; j < m_vPttaIncDcyBr.size(); j++)
      {
        nIncDcyBr[j] = 0;
        if(m_ccSwitch == true && m_vPttaICcIncDcyBr[j] != 0)
        {
          nCcIncDcyBr[j]  = 0;
          nTotIncDcyBr[j] = 0;
        }
      }
      for(unsigned int j = 0; j < dcyTr.size(); j++)
      {
        for(unsigned int k = 0; k < m_vPttaIncDcyBr.size(); k++)
        {
          if(isLiaMatchedWithLib(m_vPttaIncDcyBr[k], dcyTr[j]))
          {
            dcyBrIncDcyBr = dcyTr[j];
            int _iDcyBrIncDcyBr =
              -1; // If the variable is still equal to -1 after the following loop, then the decay
                  // branch of the particle is a new decay branch of the particle.
            int _iCcDcyBrIncDcyBr = -9999;
            for(unsigned int l = 0; l < m_vVDcyBrIncDcyBr[k].size(); l++)
            {
              if(dcyBrIncDcyBr == m_vVDcyBrIncDcyBr[k][l])
              {
                _iDcyBrIncDcyBr = l;
                if(m_ccSwitch == true && m_vPttaICcIncDcyBr[k] == 0)
                  _iCcDcyBrIncDcyBr = m_vVIDcyBrCcIncDcyBr[k][l];
                break;
              }
              else if(m_ccSwitch == true && m_vPttaICcIncDcyBr[k] == 0 &&
                      m_vVIDcyBrCcIncDcyBr[k][l] != 0 && dcyBrIncDcyBr == m_vVDcyBrCcIncDcyBr[k][l])
              {
                _iDcyBrIncDcyBr   = l;
                _iCcDcyBrIncDcyBr = -1;
                break;
              }
            }
            if(_iDcyBrIncDcyBr == -1)
            {
              _iDcyBrIncDcyBr                                 = m_vVDcyBrIncDcyBr[k].size();
              iDcyBrIncDcyBr[k][(unsigned int)(nIncDcyBr[k])] = _iDcyBrIncDcyBr;
              m_vVDcyBrIncDcyBr[k].push_back(dcyBrIncDcyBr);
              m_vVIDcyBrIncDcyBr[k].push_back(_iDcyBrIncDcyBr);
              m_vVNDcyBrIncDcyBr[k].push_back(1);
              if(m_ccSwitch == true)
              {
                dcyBrCcIncDcyBr.clear();
                list<int>::iterator liit = dcyBrIncDcyBr.begin();
                if((*liit) == -11) liit++;
                for(liit++; liit != dcyBrIncDcyBr.end(); liit++)
                  dcyBrCcIncDcyBr.push_back(getCcPid((*liit)));
                sortByPidAndPchrg(dcyBrCcIncDcyBr);
                liit = dcyBrIncDcyBr.begin();
                if(m_vPttaICcIncDcyBr[k] != 0)
                {
                  if((*liit) != -11)
                  {
                    dcyBrCcIncDcyBr.push_front(
                      getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
                  }
                  else
                  {
                    dcyBrCcIncDcyBr.push_front(11);
                    dcyBrCcIncDcyBr.push_front(-11);
                  }
                  m_vVDcyBrCcIncDcyBr[k].push_back(dcyBrCcIncDcyBr);
                  m_vVIDcyBrCcIncDcyBr[k].push_back(_iDcyBrIncDcyBr);
                  m_vVNDcyBrCcIncDcyBr[k].push_back(0);
                }
                else
                {
                  if((*liit) != -11)
                  {
                    dcyBrCcIncDcyBr.push_front(
                      (*liit)); // Here, (*liit) is equal to getCcPid((*liit)).
                  }
                  else
                  {
                    dcyBrCcIncDcyBr.push_front(11);
                    dcyBrCcIncDcyBr.push_front(-11);
                  }
                  if(dcyBrCcIncDcyBr == dcyBrIncDcyBr)
                    _iCcDcyBrIncDcyBr = 0;
                  else
                    _iCcDcyBrIncDcyBr = 1;
                  iCcDcyBrIncDcyBr[k][(unsigned int)(nIncDcyBr[k])] = _iCcDcyBrIncDcyBr;
                  m_vVDcyBrCcIncDcyBr[k].push_back(dcyBrCcIncDcyBr);
                  m_vVIDcyBrCcIncDcyBr[k].push_back(_iCcDcyBrIncDcyBr);
                  m_vVNDcyBrCcIncDcyBr[k].push_back(0);
                }
              }
            }
            else
            {
              iDcyBrIncDcyBr[k][(unsigned int)(nIncDcyBr[k])] = _iDcyBrIncDcyBr;
              if(m_ccSwitch == true && m_vPttaICcIncDcyBr[k] == 0)
              {
                iCcDcyBrIncDcyBr[k][(unsigned int)(nIncDcyBr[k])] = _iCcDcyBrIncDcyBr;
                if(_iCcDcyBrIncDcyBr == -1)
                  m_vVNDcyBrCcIncDcyBr[k][_iDcyBrIncDcyBr]++;
                else
                  m_vVNDcyBrIncDcyBr[k][_iDcyBrIncDcyBr]++;
              }
              else
              {
                m_vVNDcyBrIncDcyBr[k][_iDcyBrIncDcyBr]++;
              }
            }
            nIncDcyBr[k]++;
            break;
          }
          else if(m_ccSwitch == true && isLiaMatchedWithLib(m_vPttaCcIncDcyBr[k], dcyTr[j]))
          {
            dcyBrCcIncDcyBr = dcyTr[j];
            int _iDcyBrIncDcyBr =
              -1; // If the variable is still equal to -1 after the following loop, then the decay
                  // branch of the particle is a new decay branch of the particle.
            for(unsigned int l = 0; l < m_vVDcyBrCcIncDcyBr[k].size(); l++)
            {
              if(dcyBrCcIncDcyBr == m_vVDcyBrCcIncDcyBr[k][l])
              {
                _iDcyBrIncDcyBr = l;
                break;
              }
            }
            if(_iDcyBrIncDcyBr == -1)
            {
              _iDcyBrIncDcyBr                                     = m_vVDcyBrIncDcyBr[k].size();
              iDcyBrCcIncDcyBr[k][(unsigned int)(nCcIncDcyBr[k])] = _iDcyBrIncDcyBr;
              m_vVDcyBrCcIncDcyBr[k].push_back(dcyBrCcIncDcyBr);
              m_vVIDcyBrCcIncDcyBr[k].push_back(_iDcyBrIncDcyBr);
              m_vVNDcyBrCcIncDcyBr[k].push_back(1);

              list<int> dcyBrIncDcyBr;
              dcyBrIncDcyBr.clear();
              list<int>::iterator liit = dcyBrCcIncDcyBr.begin();
              if((*liit) == -11) liit++;
              for(liit++; liit != dcyBrCcIncDcyBr.end(); liit++)
                dcyBrIncDcyBr.push_back(getCcPid((*liit)));
              sortByPidAndPchrg(dcyBrIncDcyBr);
              liit = dcyBrCcIncDcyBr.begin();
              if((*liit) != -11)
              {
                dcyBrIncDcyBr.push_front(
                  getCcPid((*liit))); // Here, (*liit) is not equal to getCcPid((*liit)).
              }
              else
              {
                dcyBrIncDcyBr.push_front(11);
                dcyBrIncDcyBr.push_front(-11);
              }
              m_vVDcyBrIncDcyBr[k].push_back(dcyBrIncDcyBr);
              m_vVIDcyBrIncDcyBr[k].push_back(_iDcyBrIncDcyBr);
              m_vVNDcyBrIncDcyBr[k].push_back(0);
            }
            else
            {
              iDcyBrCcIncDcyBr[k][(unsigned int)(nCcIncDcyBr[k])] = _iDcyBrIncDcyBr;
              m_vVNDcyBrCcIncDcyBr[k][_iDcyBrIncDcyBr]++;
            }
            nCcIncDcyBr[k]++;
            break;
          }
        }
      }
      if(m_ccSwitch == true)
        for(unsigned int j = 0; j < m_vPttaIncDcyBr.size(); j++)
          if(m_vPttaICcIncDcyBr[j] != 0) nTotIncDcyBr[j] = nIncDcyBr[j] + nCcIncDcyBr[j];
    }

    if(m_vSigPid.size() > 0)
    {
      for(unsigned int j = 0; j < m_vSigPid.size(); j++)
      {
        nSigP[j]   = 0;
        nCcSigP[j] = 0;
      }
      for(unsigned int j = 0; j < vPid.size(); j++)
      {
        for(unsigned int k = 0; k < m_vSigPid.size(); k++)
        {
          if(vPid[j] == m_vSigPid[k])
          {
            m_vNSigP[k]++;
            nSigP[k]++;
            break;
          }
          else if(m_ccSwitch == true && m_vICcSigP[k] != 0 && vPid[j] == m_vCcSigPid[k])
          {
            m_vNCcSigP[k]++;
            nCcSigP[k]++;
            break;
          }
        }
      }
      if(m_ccSwitch == true)
        for(unsigned int j = 0; j < m_vSigPid.size(); j++) nTotSigP[j] = nSigP[j] + nCcSigP[j];
    }

    if(m_vSigIncDcyBr.size() > 0)
    {
      for(unsigned int j = 0; j < m_vSigIncDcyBr.size(); j++)
      {
        nSigIncDcyBr[j]    = countIncLiaInVlib(m_vSigIncDcyBr[j], dcyTr);
        m_vNSigIncDcyBr[j] = m_vNSigIncDcyBr[j] + nSigIncDcyBr[j];
        if(m_ccSwitch == true)
        {
          nCcSigIncDcyBr[j]    = countIncLiaInVlib(m_vCcSigIncDcyBr[j], dcyTr);
          m_vNCcSigIncDcyBr[j] = m_vNCcSigIncDcyBr[j] + nCcSigIncDcyBr[j];
          nTotSigIncDcyBr[j]   = nSigIncDcyBr[j] + nCcSigIncDcyBr[j];
        }
      }
    }

    if(m_vSigSeqDcyBrs.size() > 0)
    {
      for(unsigned int j = 0; j < m_vSigSeqDcyBrs.size(); j++)
      {
        nSigSeqDcyBrs[j] =
          countSeqDcyBrsInDcyTr(m_vSigSeqDcyBrs[j], m_vVSigSeqDcyBrsIdxOfHead[j],
                                m_vVSigSeqDcyBrsMidxOfHead[j], dcyTr, vIdxOfHead, vMidxOfHead);
        m_vNSigSeqDcyBrs[j] = m_vNSigSeqDcyBrs[j] + nSigSeqDcyBrs[j];
        if(m_ccSwitch == true)
        {
          nCcSigSeqDcyBrs[j] =
            countSeqDcyBrsInDcyTr(m_vCcSigSeqDcyBrs[j], m_vVCcSigSeqDcyBrsIdxOfHead[j],
                                  m_vVCcSigSeqDcyBrsMidxOfHead[j], dcyTr, vIdxOfHead, vMidxOfHead);
          m_vNCcSigSeqDcyBrs[j] = m_vNCcSigSeqDcyBrs[j] + nCcSigSeqDcyBrs[j];
          nTotSigSeqDcyBrs[j]   = nSigSeqDcyBrs[j] + nCcSigSeqDcyBrs[j];
        }
      }
    }

    if(m_vSigIncSeqDcyBrs.size() > 0)
    {
      for(unsigned int j = 0; j < m_vSigIncSeqDcyBrs.size(); j++)
      {
        nSigIncSeqDcyBrs[j] = countIncSeqDcyBrsInDcyTr(
          m_vVIIncSigIncSeqDcyBrs[j], m_vSigIncSeqDcyBrs[j], m_vVSigIncSeqDcyBrsIdxOfHead[j],
          m_vVSigIncSeqDcyBrsMidxOfHead[j], dcyTr, vIdxOfHead, vMidxOfHead);
        m_vNSigIncSeqDcyBrs[j] = m_vNSigIncSeqDcyBrs[j] + nSigIncSeqDcyBrs[j];
        if(m_ccSwitch == true)
        {
          nCcSigIncSeqDcyBrs[j] = countIncSeqDcyBrsInDcyTr(
            m_vVIIncSigIncSeqDcyBrs[j], m_vCcSigIncSeqDcyBrs[j], m_vVCcSigIncSeqDcyBrsIdxOfHead[j],
            m_vVCcSigIncSeqDcyBrsMidxOfHead[j], dcyTr, vIdxOfHead, vMidxOfHead);
          m_vNCcSigIncSeqDcyBrs[j] = m_vNCcSigIncSeqDcyBrs[j] + nCcSigIncSeqDcyBrs[j];
          nTotSigIncSeqDcyBrs[j]   = nSigIncSeqDcyBrs[j] + nCcSigIncSeqDcyBrs[j];
        }
      }
    }

    if(m_vSigIRADcyBr.size() > 0)
    {
      for(unsigned int j = 0; j < m_vSigIRADcyBr.size(); j++)
      {
        nSigIRADcyBr[j]    = countIRADcyBr(vPid, vMidx, m_vSigIRADcyBr[j]);
        m_vNSigIRADcyBr[j] = m_vNSigIRADcyBr[j] + nSigIRADcyBr[j];
        if(m_ccSwitch == true)
        {
          nCcSigIRADcyBr[j]    = countIRADcyBr(vPid, vMidx, m_vCcSigIRADcyBr[j]);
          m_vNCcSigIRADcyBr[j] = m_vNCcSigIRADcyBr[j] + nCcSigIRADcyBr[j];
          nTotSigIRADcyBr[j]   = nSigIRADcyBr[j] + nCcSigIRADcyBr[j];
        }
      }
    }

    if(m_vSigIncOrIRASeqDcyBrs.size() > 0)
    {
      for(unsigned int j = 0; j < m_vSigIncOrIRASeqDcyBrs.size(); j++)
      {
        nSigIncOrIRASeqDcyBrs[j] = countIncOrIRASeqDcyBrsInDcyTr(
          m_vVIIncSigIncOrIRASeqDcyBrs[j], m_vVIIRASigIncOrIRASeqDcyBrs[j],
          m_vSigIncOrIRASeqDcyBrs[j], m_vVSigIncOrIRASeqDcyBrsIdxOfHead[j],
          m_vVSigIncOrIRASeqDcyBrsMidxOfHead[j], dcyTr, vIdxOfHead, vMidxOfHead, vPid, vMidx);
        m_vNSigIncOrIRASeqDcyBrs[j] = m_vNSigIncOrIRASeqDcyBrs[j] + nSigIncOrIRASeqDcyBrs[j];
        if(m_ccSwitch == true)
        {
          nCcSigIncOrIRASeqDcyBrs[j] = countIncOrIRASeqDcyBrsInDcyTr(
            m_vVIIncSigIncOrIRASeqDcyBrs[j], m_vVIIRASigIncOrIRASeqDcyBrs[j],
            m_vCcSigIncOrIRASeqDcyBrs[j], m_vVCcSigIncOrIRASeqDcyBrsIdxOfHead[j],
            m_vVCcSigIncOrIRASeqDcyBrsMidxOfHead[j], dcyTr, vIdxOfHead, vMidxOfHead, vPid, vMidx);
          m_vNCcSigIncOrIRASeqDcyBrs[j] =
            m_vNCcSigIncOrIRASeqDcyBrs[j] + nCcSigIncOrIRASeqDcyBrs[j];
          nTotSigIncOrIRASeqDcyBrs[j] = nSigIncOrIRASeqDcyBrs[j] + nCcSigIncOrIRASeqDcyBrs[j];
        }
      }
    }

    if(m_vSigDcyTr.size() > 0)
    {
      iSigDcyTr  = -1;
      iSigDcyFSt = -1;
      if(m_ccSwitch == true)
      {
        iCcSigDcyTr  = -9999;
        iCcSigDcyFSt = -9999;
      }
      for(unsigned int j = 0; j < m_vSigDcyTr.size(); j++)
      {

        if(dcyTr == m_vSigDcyTr[j])
        {
          iSigDcyTr  = j;
          iSigDcyFSt = m_iSigDcyTrISigDcyFStMap[iSigDcyTr];
          m_vNSigDcyTr[iSigDcyTr]++;
          m_vNSigDcyFSt[iSigDcyFSt]++;
          if(m_ccSwitch == true)
          {
            if(m_vNSigDcyTr[iSigDcyTr] + m_vNCcSigDcyTr[iSigDcyTr] == 1)
              m_iSigDcyTrIDcyTrMap[iSigDcyTr] = iDcyTr;
            if(m_vNSigDcyFSt[iSigDcyFSt] + m_vNCcSigDcyFSt[iSigDcyFSt] == 1)
              m_iSigDcyFStIDcyFStMap[iSigDcyFSt] = iDcyFSt;
            iCcSigDcyTr  = m_vICcSigDcyTr[j];
            iCcSigDcyFSt = m_iSigDcyTrICcSigDcyFStMap[iSigDcyTr];
            if(m_vNSigDcyTr[iSigDcyTr] + m_vNCcSigDcyTr[iSigDcyTr] == 1)
              m_iSigDcyTrICcDcyTrMap[iSigDcyTr] = iCcDcyTr;
            if(m_vNSigDcyFSt[iSigDcyFSt] + m_vNCcSigDcyFSt[iSigDcyFSt] == 1)
              m_iSigDcyFStICcDcyFStMap[iSigDcyFSt] = iCcDcyFSt;
          }
          else
          {
            if(m_vNSigDcyTr[iSigDcyTr] == 1) m_iSigDcyTrIDcyTrMap[iSigDcyTr] = iDcyTr;
            if(m_vNSigDcyFSt[iSigDcyFSt] == 1) m_iSigDcyFStIDcyFStMap[iSigDcyFSt] = iDcyFSt;
          }
          break;
        }
        else if(m_ccSwitch == true && m_vICcSigDcyTr[j] != 0 && dcyTr == m_vCcSigDcyTr[j])
        {
          iSigDcyTr  = j;
          iSigDcyFSt = m_iSigDcyTrISigDcyFStMap[iSigDcyTr];
          m_vNCcSigDcyTr[iSigDcyTr]++;
          m_vNCcSigDcyFSt[iSigDcyFSt]++;
          if(m_vNSigDcyTr[iSigDcyTr] + m_vNCcSigDcyTr[iSigDcyTr] == 1)
            m_iSigDcyTrIDcyTrMap[iSigDcyTr] = iDcyTr;
          if(m_vNSigDcyFSt[iSigDcyFSt] + m_vNCcSigDcyFSt[iSigDcyFSt] == 1)
            m_iSigDcyFStIDcyFStMap[iSigDcyFSt] = iDcyFSt;
          iCcSigDcyTr  = -1;
          iCcSigDcyFSt = -1 * m_iSigDcyTrICcSigDcyFStMap[iSigDcyTr];
          if(m_vNSigDcyTr[iSigDcyTr] + m_vNCcSigDcyTr[iSigDcyTr] == 1)
            m_iSigDcyTrICcDcyTrMap[iSigDcyTr] = -1 * iCcDcyTr;
          if(m_vNSigDcyFSt[iSigDcyFSt] + m_vNCcSigDcyFSt[iSigDcyFSt] == 1)
            m_iSigDcyFStICcDcyFStMap[iSigDcyFSt] = -1 * iCcDcyFSt;
          break;
        }
      }
    }

    if(m_vSigDcyFSt2.size() > 0)
    {
      iSigDcyFSt2   = -1;
      iCcSigDcyFSt2 = -9999;
      for(unsigned int j = 0; j < m_vSigDcyFSt2.size(); j++)
      {
        if(dcyFSt == m_vSigDcyFSt2[j])
        {
          iSigDcyFSt2 = j;
          m_vNSigDcyFSt2[iSigDcyFSt2]++;
          if(m_ccSwitch == true)
          {
            if(m_vNSigDcyFSt2[iSigDcyFSt2] + m_vNCcSigDcyFSt2[iSigDcyFSt2] == 1)
              m_iSigDcyFSt2IDcyFStMap[iSigDcyFSt2] = iDcyFSt;
            iCcSigDcyFSt2 = m_vICcSigDcyFSt2[iSigDcyFSt2];
            if(m_vNSigDcyFSt2[j] + m_vNCcSigDcyFSt2[j] == 1)
              m_iSigDcyFSt2ICcDcyFStMap[iSigDcyFSt2] = iCcDcyFSt;
          }
          else
          {
            if(m_vNSigDcyFSt2[iSigDcyFSt2] == 1) m_iSigDcyFSt2IDcyFStMap[iSigDcyFSt2] = iDcyFSt;
          }
          break;
        }
        else if(m_ccSwitch == true && m_vICcSigDcyFSt2[j] != 0 && dcyFSt == m_vCcSigDcyFSt2[j])
        {
          iSigDcyFSt2 = j;
          m_vNCcSigDcyFSt2[iSigDcyFSt2]++;
          if(m_vNSigDcyFSt2[iSigDcyFSt2] + m_vNCcSigDcyFSt2[iSigDcyFSt2] == 1)
            m_iSigDcyFSt2IDcyFStMap[iSigDcyFSt2] = iDcyFSt;
          iCcSigDcyFSt2 = -1 * m_vICcSigDcyFSt2[iSigDcyFSt2];
          if(m_vNSigDcyFSt2[j] + m_vNCcSigDcyFSt2[j] == 1)
            m_iSigDcyFSt2ICcDcyFStMap[iSigDcyFSt2] = -1 * iCcDcyFSt;
          break;
        }
      }
    }

    tr->Fill();

    if((i + 1 >= nEtrsForTiming) &&
       (((i + 1) % nEtrsForTiming == 0) || ((i + 1) == nEtrsToBePrcsd)))
    {
      if((i + 1) == nEtrsForTiming)
        cout << "Number of entries processed\tNumber of seconds elapsed" << endl << endl;
      cout << setiosflags(ios::right) << setw(14) << i + 1 << "\t\t\t" << setiosflags(ios::fixed)
           << setprecision(2) << setw(14) << (clock() - starttime) / ((double)CLOCKS_PER_SEC)
           << resetiosflags(ios::adjustfield) << endl;
      if((i + 1) == nEtrsToBePrcsd) cout << endl;
    }
    if(!m_cut.empty() && (i + 1) == nEtrsToBePrcsd)
      cout << "Note that only " << nEtrsThroughTheCut << " entries passed the cut." << endl << endl;

    closeTheOptRootFl1 =
      ((m_nEtrsMaxInASngOptRootFl != ULONG_MAX) && ((i + 1) % m_nEtrsMaxInASngOptRootFl == 0));
    closeTheOptRootFl2 = ((m_nEtrsMaxInASngOptRootFl == ULONG_MAX) &&
                          (((unsigned long)tr->GetTotBytes()) > nTotBytesMaxOfASngOptRootFl));
    closeTheOptRootFl3 = ((i + 1) == nEtrsToBePrcsd);
    if(closeTheOptRootFl1 || closeTheOptRootFl2 || closeTheOptRootFl3)
    {
      fl->Write();
      delete tr; // Pay attention to that replacing the "delete tr" by "tr->Delete()" will result in
                 // a problem of "*** Break *** segmentation violation".
      fl->Close();
      delete fl;
      openANewOptRootFl = true;
      iOptRootFls++;
    }

    if((i + 1) == nEtrsToBePrcsd)
    {
      for(unsigned int j = 0; j < Nasterisks; j++) cout << "*";
      cout << endl << endl;
    }
  }
  delete chn; // This statement is indispensable, or a tough problem will arise before the "return
              // 0;" statement in the main function.

  if(m_sttaDcyTrsAndDcyFSts == false)
  {
    if(m_ccSwitch == true)
    {
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNDcyTr, m_vNCcDcyTr, m_vDcyTr, m_vCcDcyTr, m_vIDcyTr,
                                       m_vICcDcyTr);
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNDcyFSt, m_vNCcDcyFSt, m_vDcyFSt, m_vCcDcyFSt, m_vIDcyFSt,
                                       m_vICcDcyFSt);
    }
    else
    {
      sortBy1stFromLrgToSml(m_vNDcyTr, m_vDcyTr, m_vIDcyTr);
      sortBy1stFromLrgToSml(m_vNDcyFSt, m_vDcyFSt, m_vIDcyFSt);
    }
  }
  if(m_vPttaPid.size() > 0)
  {
    for(unsigned int i = 0; i < m_vPttaPid.size(); i++)
    {
      if(m_ccSwitch == true)
      {
        sortBySumOf1stAnd2ndFromLrgToSml(m_vVNDcyBrP[i], m_vVNDcyBrCcP[i], m_vVDcyBrP[i],
                                         m_vVDcyBrCcP[i], m_vVIDcyBrP[i], m_vVIDcyBrCcP[i]);
      }
      else
      {
        sortBy1stFromLrgToSml(m_vVNDcyBrP[i], m_vVDcyBrP[i], m_vVIDcyBrP[i]);
      }
    }
  }

  if(m_vPttaIncDcyBr.size() > 0)
  {
    for(unsigned int i = 0; i < m_vPttaIncDcyBr.size(); i++)
    {
      if(m_ccSwitch == true)
      {
        sortBySumOf1stAnd2ndFromLrgToSml(m_vVNDcyBrIncDcyBr[i], m_vVNDcyBrCcIncDcyBr[i],
                                         m_vVDcyBrIncDcyBr[i], m_vVDcyBrCcIncDcyBr[i],
                                         m_vVIDcyBrIncDcyBr[i], m_vVIDcyBrCcIncDcyBr[i]);
      }
      else
      {
        sortBy1stFromLrgToSml(m_vVNDcyBrIncDcyBr[i], m_vVDcyBrIncDcyBr[i], m_vVIDcyBrIncDcyBr[i]);
      }
    }
  }

  if(m_vSigPid.size() > 0)
  {
    if(m_ccSwitch == true)
    {
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigP, m_vNCcSigP, m_vSigPid, m_vCcSigPid, m_vISigP,
                                       m_vICcSigP);
    }
    else
    {
      sortBy1stFromLrgToSml(m_vNSigP, m_vSigPid, m_vISigP);
    }
  }

  if(m_vSigIncDcyBr.size() > 0)
  {
    if(m_ccSwitch == true)
    {
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigIncDcyBr, m_vNCcSigIncDcyBr, m_vSigIncDcyBr,
                                       m_vCcSigIncDcyBr, m_vISigIncDcyBr, m_vICcSigIncDcyBr);
    }
    else
    {
      sortBy1stFromLrgToSml(m_vNSigIncDcyBr, m_vSigIncDcyBr, m_vISigIncDcyBr);
    }
  }

  if(m_vSigSeqDcyBrs.size() > 0)
  {
    if(m_ccSwitch == true)
    {
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigSeqDcyBrs, m_vNCcSigSeqDcyBrs, m_vSigSeqDcyBrs,
                                       m_vCcSigSeqDcyBrs, m_vISigSeqDcyBrs, m_vICcSigSeqDcyBrs,
                                       m_vVSigSeqDcyBrsIdxOfHead, m_vVCcSigSeqDcyBrsIdxOfHead,
                                       m_vVSigSeqDcyBrsMidxOfHead, m_vVCcSigSeqDcyBrsMidxOfHead);
    }
    else
    {
      sortBy1stFromLrgToSml(m_vNSigSeqDcyBrs, m_vSigSeqDcyBrs, m_vISigSeqDcyBrs,
                            m_vVSigSeqDcyBrsIdxOfHead, m_vVSigSeqDcyBrsMidxOfHead);
    }
  }

  if(m_vSigIncSeqDcyBrs.size() > 0)
  {
    if(m_ccSwitch == true)
    {
      sortBySumOf1stAnd2ndFromLrgToSml(
        m_vNSigIncSeqDcyBrs, m_vNCcSigIncSeqDcyBrs, m_vSigIncSeqDcyBrs, m_vCcSigIncSeqDcyBrs,
        m_vISigIncSeqDcyBrs, m_vICcSigIncSeqDcyBrs, m_vVSigIncSeqDcyBrsIdxOfHead,
        m_vVCcSigIncSeqDcyBrsIdxOfHead, m_vVSigIncSeqDcyBrsMidxOfHead,
        m_vVCcSigIncSeqDcyBrsMidxOfHead);
    }
    else
    {
      sortBy1stFromLrgToSml(m_vNSigIncSeqDcyBrs, m_vSigIncSeqDcyBrs, m_vISigIncSeqDcyBrs,
                            m_vVSigIncSeqDcyBrsIdxOfHead, m_vVSigIncSeqDcyBrsMidxOfHead);
    }
  }

  if(m_vSigIRADcyBr.size() > 0)
  {
    if(m_ccSwitch == true)
    {
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigIRADcyBr, m_vNCcSigIRADcyBr, m_vSigIRADcyBr,
                                       m_vCcSigIRADcyBr, m_vISigIRADcyBr, m_vICcSigIRADcyBr);
    }
    else
    {
      sortBy1stFromLrgToSml(m_vNSigIRADcyBr, m_vSigIRADcyBr, m_vISigIRADcyBr);
    }
  }

  if(m_vSigDcyTr.size() > 0)
  {
    if(m_ccSwitch == true)
    {
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigDcyTr, m_vNCcSigDcyTr, m_vSigDcyTr, m_vCcSigDcyTr,
                                       m_vISigDcyTr, m_vICcSigDcyTr);
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigDcyFSt, m_vNCcSigDcyFSt, m_vSigDcyFSt, m_vCcSigDcyFSt,
                                       m_vISigDcyFSt, m_vICcSigDcyFSt);
    }
    else
    {
      sortBy1stFromLrgToSml(m_vNSigDcyTr, m_vSigDcyTr, m_vISigDcyTr);
      sortBy1stFromLrgToSml(m_vNSigDcyFSt, m_vSigDcyFSt, m_vISigDcyFSt);
    }
  }

  if(m_vSigDcyFSt2.size() > 0)
  {
    if(m_ccSwitch == true)
    {
      sortBySumOf1stAnd2ndFromLrgToSml(m_vNSigDcyFSt2, m_vNCcSigDcyFSt2, m_vSigDcyFSt2,
                                       m_vCcSigDcyFSt2, m_vISigDcyFSt2, m_vICcSigDcyFSt2);
    }
    else
    {
      sortBy1stFromLrgToSml(m_vNSigDcyFSt2, m_vSigDcyFSt2, m_vISigDcyFSt2);
    }
  }
}
