#include "topoana.h"
#include "TBranch.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

void topoana::readCard(string topoAnaCardName)
{
  m_inputRootFileNames.clear();
  m_vSigPid.clear();
  m_vVSigPid.clear();
  m_vVSigMidx.clear();

  ifstream infile(topoAnaCardName.c_str(), ios::in);
  string   line = "";
  while(!infile.eof())
  {
    while(!infile.eof())
    {
      getline(infile, line);
      if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
    }
    if(line.empty() || line[0] == '#' || line[0] == ' ') { break; }
    else if(line == "Begin input root file names")
    {
      line = "";
      while(!infile.eof())
      {
        getline(infile, line);
        if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
      }
      if(line.empty() || line[0] == '#' || line[0] == ' ' || line == "End input root file names" ||
         line.substr(0, 5) == "Begin")
      {
        cerr
          << "Error : There are not root file names after the prompt Begin input root file names !"
          << endl;
        exit(111);
      }
      m_inputRootFileNames.push_back(line);
      line = "";
      while(1)
      {
        while(!infile.eof())
        {
          getline(infile, line);
          if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
        }
        if(line.empty() || line[0] == '#' || line[0] == ' ' || line.substr(0, 5) == "Begin")
        {
          cerr << "Error : the prompt End input root file names is missing !" << endl;
          exit(112);
        }
        else if(line == "End input root file names")
        {
          line = "";
          break;
        }
        else
        {
          m_inputRootFileNames.push_back(line);
          line = "";
        }
      }
    }
    else if(line == "Begin input tree name")
    {
      line = "";
      while(!infile.eof())
      {
        getline(infile, line);
        if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
      }
      if(line.empty() || line[0] == '#' || line[0] == ' ' || line == "End input tree name" ||
         line.substr(0, 5) == "Begin")
      {
        cerr << "Error : There is not tree name after the prompt Begin input tree name !" << endl;
        exit(113);
      }
      m_inputTreeName = line;
      line            = "";
      while(!infile.eof())
      {
        getline(infile, line);
        if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
      }
      if(line.empty() || line[0] == '#' || line[0] == ' ' || line != "End input tree name")
      {
        cerr << "Error : the prompt End input tree name is missing !" << endl;
        exit(114);
      }
    }
    else if(line == "Begin output main file name")
    {
      line = "";
      while(!infile.eof())
      {
        getline(infile, line);
        if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
      }
      if(line.empty() || line[0] == '#' || line[0] == ' ' || line == "End output main file name" ||
         line.substr(0, 5) == "Begin")
      {
        cerr << "Error : There is not main file name after the prompt Begin output main file name !"
             << endl;
        exit(115);
      }
      m_outputMainFileName = line;
      line                 = "";
      while(!infile.eof())
      {
        getline(infile, line);
        if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
      }
      if(line.empty() || line[0] == '#' || line[0] == ' ' || line != "End output main file name")
      {
        cerr << "Error : the prompt End output main file name is missing !" << endl;
        exit(116);
      }
    }
    else if(line == "Begin sigal particle identifications")
    {
      line = "";
      istringstream iss;
      iss.clear();
      int sigPid;
      while(!infile.eof())
      {
        getline(infile, line);
        if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
      }
      if(line.empty() || line[0] == '#' || line[0] == ' ' ||
         line == "End sigal particle identifications" || line.substr(0, 5) == "Begin")
      {
        cerr << "Error : There are not sigal particle identifications after the prompt Begin sigal "
                "particle identifications !"
             << endl;
        exit(117);
      }
      iss.str(line);
      iss >> sigPid;
      m_vSigPid.push_back(sigPid);
      line = "";
      iss.clear();
      while(1)
      {
        while(!infile.eof())
        {
          getline(infile, line);
          if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
        }
        if(line.empty() || line[0] == '#' || line[0] == ' ' || line.substr(0, 5) == "Begin")
        {
          cerr << "Error : the prompt End sigal particle identifications is missing !" << endl;
          exit(118);
        }
        else if(line == "End sigal particle identifications")
        {
          line = "";
          break;
        }
        else
        {
          iss.str(line);
          iss >> sigPid;
          m_vSigPid.push_back(sigPid);
          line = "";
          iss.clear();
        }
      }
    }
    else if(line == "Begin signal event decay trees")
    {
      line = "";
      istringstream iss;
      iss.clear();
      int         sigIdx;
      vector<int> vSigPid;
      vSigPid.clear();
      int         sigPid;
      vector<int> vSigMidx;
      vSigMidx.clear();
      int sigMidx;
      while(!infile.eof())
      {
        getline(infile, line);
        if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
      }
      if(line.empty() || line[0] == '#' || line[0] == ' ' ||
         line == "End signal event decay trees" || line.substr(0, 5) == "Begin")
      {
        cerr << "Error : There are not signal event decay trees after the prompt Begin signal "
                "event decay trees !"
             << endl;
        exit(119);
      }
      iss.str(line);
      iss >> sigIdx >> sigPid >> sigMidx;
      vSigPid.push_back(sigPid);
      vSigMidx.push_back(sigMidx);
      line = "";
      iss.clear();
      while(1)
      {
        while(!infile.eof())
        {
          getline(infile, line);
          if(!line.empty() && line[0] != '#' && line[0] != ' ') break;
        }
        if(line.empty() || line[0] == '#' || line[0] == ' ' || line.substr(0, 5) == "Begin")
        {
          cerr << "Error : End signal event decay trees is missing !" << endl;
          exit(120);
        }
        else if(line == "End signal event decay trees")
        {
          line = "";
          m_vVSigPid.push_back(vSigPid);
          vSigPid.clear();
          m_vVSigMidx.push_back(vSigMidx);
          vSigMidx.clear();
          break;
        }
        else
        {
          iss.str(line);
          iss >> sigIdx >> sigPid >> sigMidx;
          if(sigIdx == 0)
          {
            m_vVSigPid.push_back(vSigPid);
            vSigPid.clear();
            m_vVSigMidx.push_back(vSigMidx);
            vSigMidx.clear();
          }
          vSigPid.push_back(sigPid);
          vSigMidx.push_back(sigMidx);
          line = "";
          iss.clear();
        }
      }
    }
    else
    {
      cerr << "Error : the input line " << line << " is invalid !" << endl;
      exit(121);
    }
  }

  if(m_inputRootFileNames.size() == 0)
  {
    cerr << "Error : there are not root file names !" << endl;
    exit(131);
  }
  else
  {
    cout << "m_inputRootFileNames :" << endl;
    for(unsigned int i = 0; i < m_inputRootFileNames.size(); i++)
      cout << m_inputRootFileNames[i] << endl;
    cout << endl;
  }

  if(m_inputTreeName.empty())
  {
    cerr << "Error : there is not tree name !" << endl;
    exit(132);
  }
  else
  {
    cout << "m_inputTreeName : " << m_inputTreeName << endl << endl;
  }

  if(m_outputMainFileName.empty())
  {
    cout << "Error : there is not output main file name !" << endl;
    exit(133);
  }
  else
  {
    cout << "m_outputMainFileName : " << m_outputMainFileName << endl << endl;
  }

  if(m_vSigPid.size() == 0)
  { cerr << "Info : there are not sigal particle identifications ." << endl; } else
  {
    cout << "m_vSigPid :" << endl;
    for(unsigned int i = 0; i < m_vSigPid.size(); i++) cout << m_vSigPid[i] << endl;
    cout << endl;
  }

  if(m_vVSigPid.size() == 0) { cerr << "Info : there are not signal event decay trees ." << endl; }
  else
  {
    cout << "signal event decay trees :" << endl << endl;
    for(unsigned int i = 0; i < m_vVSigPid.size(); i++)
    {
      for(unsigned int j = 0; j < m_vVSigPid[i].size(); j++)
      { cout << j << "\t" << m_vVSigPid[i][j] << "\t" << m_vVSigMidx[i][j] << endl; } cout << endl;
    }
    cout << endl;
  }
}

void topoana::makePidThreeMChargeMap()
{
  m_pidThreeMPchargeMap.clear();
  int      pid, threempcharge;
  ifstream infile("pid_threempcharge.dat", ios::in);
  while(infile >> pid >> threempcharge)
  {
    m_pidThreeMPchargeMap[pid] = threempcharge;
    // cout<<pid<<"\t"<<m_pidChargeMap[pid]<<endl;
  }
}

void topoana::sortByPidAndPcharge(vector<int>& via, vector<int>& vib, vector<int>& vic)
{
  if(via.size() != vib.size() || vib.size() != vic.size())
  {
    cerr << "Error: the three vectors have different size!" << endl;
    cerr << "Error: the size of the first vector is " << via.size() << "." << endl;
    cerr << "Error: the size of the second vector is " << vib.size() << "." << endl;
    cerr << "Error: the size of the third vector is " << vic.size() << "." << endl;
    cerr << "Error: please check the three vectors!" << endl;
    exit(211);
  }
  if(via.size() == 0)
  {
    cerr << "Info: the sizes of the three vectors is zero" << endl;
    return;
  }

  int iatemp, ibtemp, ictemp;
  if(m_pidThreeMPchargeMap.size() == 0) makePidThreeMChargeMap();
  for(unsigned int i = 0; i < via.size() - 1; i++)
    for(unsigned int j = i + 1; j < via.size(); j++)
    {
      if(abs(via[i]) > abs(via[j]))
      {
        if(abs(via[j]) != 22)
        {
          iatemp = via[i];
          via[i] = via[j];
          via[j] = iatemp;
          ibtemp = vib[i];
          vib[i] = vib[j];
          vib[j] = ibtemp;
          ictemp = vic[i];
          vic[i] = vic[j];
          vic[j] = ictemp;
        }
      }
      else if(abs(via[i]) == abs(via[j]))
      {
        if(m_pidThreeMPchargeMap[via[i]] < m_pidThreeMPchargeMap[via[j]])
        {
          iatemp = via[i];
          via[i] = via[j];
          via[j] = iatemp;
          ibtemp = vib[i];
          vib[i] = vib[j];
          vib[j] = ibtemp;
          ictemp = vic[i];
          vic[i] = vic[j];
          vic[j] = ictemp;
        }
        else if(m_pidThreeMPchargeMap[via[i]] == m_pidThreeMPchargeMap[via[j]])
        {
          if(via[i] < via[j])
          {
            iatemp = via[i];
            via[i] = via[j];
            via[j] = iatemp;
            ibtemp = vib[i];
            vib[i] = vib[j];
            vib[j] = ibtemp;
            ictemp = vic[i];
            vic[i] = vic[j];
            vic[j] = ictemp;
          }
        }
      }
      else
      {
        if(abs(via[i]) == 22)
        {
          iatemp = via[i];
          via[i] = via[j];
          via[j] = iatemp;
          ibtemp = vib[i];
          vib[i] = vib[j];
          vib[j] = ibtemp;
          ictemp = vic[i];
          vic[i] = vic[j];
          vic[j] = ictemp;
        }
      }
    }
}

void topoana::evtReorder(vector<int>& vPid, vector<int>& vMidx)
{
  if(vPid.size() != vMidx.size())
  {
    cerr << "Error: the vector vPid and vMidx have different size!" << endl;
    cerr << "Error: the size of the vector vPid is " << vPid.size() << "." << endl;
    cerr << "Error: the size of the vector vMidx is " << vMidx.size() << "." << endl;
    cerr << "Error: please check the two vectors!" << endl;
    exit(311);
  }
  /*cout<<"before reorder :"<<endl;
  for(unsigned int i=0;i<vPid.size();i++)
    {
      cout<<"pid: "<<vPid[i]<<"\t"<<"midx: "<<vMidx[i]<<endl;
    }*/
  vector<int> vPidFinal, vMidxFinal, vIdxFinal, vPidTemp, vMidxTemp, vIdxTempOld, vIdxTempYoung,
    vIdxTempYoungSubset;
  for(unsigned int i = 0; i < vPid.size(); i++)
  {
    if(((unsigned int)vMidx[i]) == i)
    {
      vPidTemp.push_back(vPid[i]);
      vMidxTemp.push_back(vMidx[i]);
      vIdxTempOld.push_back(i);
    }
  }
  sortByPidAndPcharge(vPidTemp, vMidxTemp, vIdxTempOld);
  for(unsigned int i = 0; i < vPidTemp.size(); i++)
  {
    vPidFinal.push_back(vPidTemp[i]);
    vMidxFinal.push_back(vMidxTemp[i]);
    vIdxFinal.push_back(vIdxTempOld[i]);
  }
  vPidTemp.clear();
  vMidxTemp.clear();
  while(vIdxTempOld.size() != 0)
  {
    for(unsigned int i = 0; i < vIdxTempOld.size(); i++)
    {
      for(unsigned int j = 0; j < vPid.size(); j++)
      {
        // if(((unsigned int) vMidx[j])!=j&&vMidx[j]==vIdxTempOld[i])
        if(((unsigned int)vMidx[j]) != j && vPid[((unsigned int)vMidx[j])] != 111 &&
           vMidx[j] == vIdxTempOld[i])
        {
          vPidTemp.push_back(vPid[j]);
          vMidxTemp.push_back(vMidx[j]);
          vIdxTempYoungSubset.push_back(j);
        }
      }
      if(vPidTemp.size() != 0) sortByPidAndPcharge(vPidTemp, vMidxTemp, vIdxTempYoungSubset);
      for(unsigned int k = 0; k < vPidTemp.size(); k++)
      {
        vPidFinal.push_back(vPidTemp[k]);
        vMidxFinal.push_back(vMidxTemp[k]);
        vIdxFinal.push_back(vIdxTempYoungSubset[k]);
        vIdxTempYoung.push_back(vIdxTempYoungSubset[k]);
      }
      vPidTemp.clear();
      vMidxTemp.clear();
      vIdxTempYoungSubset.clear();
    }
    vIdxTempOld.clear();
    vIdxTempOld = vIdxTempYoung;
    vIdxTempYoung.clear();
  }
  for(unsigned int i = 0; i < vMidxFinal.size(); i++)
  {
    for(unsigned int j = 0; j < vMidxFinal.size(); j++)
    {
      if(vMidxFinal[i] == vIdxFinal[j])
      {
        vMidxFinal[i] = j;
        break;
      }
    }
  }
  vPid.clear();
  vMidx.clear();
  vPid  = vPidFinal;
  vMidx = vMidxFinal;

  if(vPid.size() > 2)
  {
    for(unsigned int i = 0; i < vPid.size() - 2; i++)
    {
      for(unsigned int j = i + 1; j < vPid.size() - 1; j++)
      {
        if((vPid[i] == vPid[j]) &&
           ((((unsigned int)vMidx[i]) == i && ((unsigned int)vMidx[j]) == j) ||
            vMidx[i] == vMidx[j]))
        {
          vector<int> vIdxofdsofi, vIdxofdsofj;
          for(unsigned int k = j + 1; k < vPid.size(); k++)
          {
            if(((unsigned int)vMidx[k]) == i)
              vIdxofdsofi.push_back(k);
            else if(((unsigned int)vMidx[k]) == j)
              vIdxofdsofj.push_back(k);
          }
          bool flag = false;
          if(vIdxofdsofi.size() > vIdxofdsofj.size())
            flag = true;
          else if(vIdxofdsofi.size() == vIdxofdsofj.size())
          {
            for(unsigned int l = 0; l < vIdxofdsofi.size(); l++)
            {
              if(vPid[vIdxofdsofi[l]] > vPid[vIdxofdsofj[l]])
              {
                flag = true;
                break;
              }
            }
          }
          if(flag == true)
          {
            for(unsigned int m = 0; m < vIdxofdsofi.size(); m++) { vMidx[vIdxofdsofi[m]] = j; }
            for(unsigned int n = 0; n < vIdxofdsofj.size(); n++) { vMidx[vIdxofdsofj[n]] = i; }
          }
        }
      }
    }
  }
  /*cout<<"after reorder :"<<endl;
  for(unsigned int i=0;i<vPid.size();i++)
    {
      cout<<"pid: "<<vPid[i]<<"\t"<<"midx: "<<vMidx[i]<<endl;
    }*/
}

void topoana::getEvtDcyTr(vector<int>& vPid, vector<int>& vMidx, vector<list<int> >& evtDcyTr)
{
  evtDcyTr.clear();
  list<int> evtDcyChn;
  evtDcyChn.clear();
  for(unsigned int i = 0; i < vPid.size(); i++)
  {
    if(((unsigned int)vMidx[i]) == i) evtDcyChn.push_back(vPid[i]);
  }
  evtDcyChn.push_front(11);
  evtDcyChn.push_front(-11);
  evtDcyTr.push_back(evtDcyChn);
  evtDcyChn.clear();
  for(unsigned int i = 0; i < vPid.size(); i++)
  {
    for(unsigned int j = i + 1; j < vPid.size(); j++)
    {
      if(((unsigned int)vMidx[j]) == i) evtDcyChn.push_back(vPid[j]);
    }
    if(evtDcyChn.size() == 0) continue;
    if(abs(vPid[i]) != 1 && abs(vPid[i]) != 2 && abs(vPid[i]) != 3 && abs(vPid[i]) != 4 &&
       abs(vPid[i]) != 5 && abs(vPid[i]) != 6)
    { evtDcyChn.push_front(vPid[i]); } else
    {
      evtDcyChn.push_front(-abs(vPid[i]));
      evtDcyChn.push_front(abs(vPid[i]));
    }
    evtDcyTr.push_back(evtDcyChn);
    evtDcyChn.clear();
  }
}

void topoana::sortByPidAndPcharge(list<int>& lia)
{
  if(lia.size() == 0)
  {
    cerr << "Info: the size of the list is zero" << endl;
    return;
  }
  int                 iatemp;
  list<int>::iterator liait1;
  list<int>::iterator liait2;
  list<int>::iterator liaitLastButOne = lia.end();
  liaitLastButOne--;
  if(m_pidThreeMPchargeMap.size() == 0) makePidThreeMChargeMap();
  for(liait1 = lia.begin(); liait1 != liaitLastButOne; liait1++)
    for(liait2 = liait1, liait2++; liait2 != lia.end(); liait2++)
    {
      if(abs((*liait1)) > abs((*liait2)))
      {
        if(abs((*liait2)) != 22)
        {
          iatemp    = (*liait1);
          (*liait1) = (*liait2);
          (*liait2) = iatemp;
        }
      }
      else if(abs((*liait1)) == abs((*liait2)))
      {
        if(m_pidThreeMPchargeMap[(*liait1)] < m_pidThreeMPchargeMap[(*liait2)])
        {
          iatemp    = (*liait1);
          (*liait1) = (*liait2);
          (*liait2) = iatemp;
        }
        else if(m_pidThreeMPchargeMap[(*liait1)] == m_pidThreeMPchargeMap[(*liait2)])
        {
          if((*liait1) < (*liait2))
          {
            iatemp    = (*liait1);
            (*liait1) = (*liait2);
            (*liait2) = iatemp;
          }
        }
      }
      else
      {
        if(abs((*liait1)) == 22)
        {
          iatemp    = (*liait1);
          (*liait1) = (*liait2);
          (*liait2) = iatemp;
        }
      }
    }
}

void topoana::getEvtDcyIFChn(vector<int>& vPid, vector<int>& vMidx, list<int>& evtDcyIFChn)
{
  evtDcyIFChn.clear();
  bool fsp;
  for(unsigned int i = 0; i < vPid.size(); i++)
  {
    fsp = true;
    for(unsigned int j = i + 1; j < vPid.size(); j++)
    {
      if(((unsigned int)vMidx[j]) == i)
      {
        fsp = false;
        break;
      }
    }
    if(fsp && abs(vPid[i]) != 1 && abs(vPid[i]) != 2 && abs(vPid[i]) != 3 && abs(vPid[i]) != 4 &&
       abs(vPid[i]) != 5 && abs(vPid[i]) != 6)
      evtDcyIFChn.push_back(vPid[i]);
  }
  sortByPidAndPcharge(evtDcyIFChn);
  evtDcyIFChn.push_front(11);
  evtDcyIFChn.push_front(-11);
}

void topoana::sortByFirstFromLargeToSmall(vector<int>& via, vector<vector<list<int> > >& vib,
                                          vector<int>& vic)
{
  if(via.size() != vib.size() || vib.size() != vic.size())
  {
    cerr << "Error: the three vectors have different size!" << endl;
    cerr << "Error: the size of the first vector is " << via.size() << "." << endl;
    cerr << "Error: the size of the second vector is " << vib.size() << "." << endl;
    cerr << "Error: the size of the third vector is " << vic.size() << "." << endl;
    cerr << "Error: please check the three vectors!" << endl;
    exit(411);
  }
  if(via.size() == 0)
  {
    cerr << "Info: the sizes of the two vectors is zero" << endl;
    return;
  }
  int                iatemp;
  vector<list<int> > ibtemp;
  int                ictemp;
  for(unsigned int i = 0; i < via.size() - 1; i++)
    for(unsigned int j = i + 1; j < via.size(); j++)
      if(via[i] < via[j])
      {
        iatemp = via[i];
        via[i] = via[j];
        via[j] = iatemp;
        ibtemp = vib[i];
        vib[i] = vib[j];
        vib[j] = ibtemp;
        ictemp = vic[i];
        vic[i] = vic[j];
        vic[j] = ictemp;
      }
}

void topoana::sortByFirstFromLargeToSmall(vector<int>& via, vector<list<int> >& vib,
                                          vector<int>& vic)
{
  if(via.size() != vib.size() || vib.size() != vic.size())
  {
    cerr << "Error: the three vectors have different size!" << endl;
    cerr << "Error: the size of the first vector is " << via.size() << "." << endl;
    cerr << "Error: the size of the second vector is " << vib.size() << "." << endl;
    cerr << "Error: the size of the third vector is " << vic.size() << "." << endl;
    cerr << "Error: please check the three vectors!" << endl;
    exit(412);
  }
  if(via.size() == 0)
  {
    cerr << "Info: the sizes of the two vectors is zero" << endl;
    return;
  }
  int       iatemp;
  list<int> ibtemp;
  int       ictemp;
  for(unsigned int i = 0; i < via.size() - 1; i++)
    for(unsigned int j = i + 1; j < via.size(); j++)
      if(via[i] < via[j])
      {
        iatemp = via[i];
        via[i] = via[j];
        via[j] = iatemp;
        ibtemp = vib[i];
        vib[i] = vib[j];
        vib[j] = ibtemp;
        ictemp = vic[i];
        vic[i] = vic[j];
        vic[j] = ictemp;
      }
}

void topoana::sortByFirstFromLargeToSmall(vector<int>& via, vector<int>& vib, vector<int>& vic)
{
  if(via.size() != vib.size() || vib.size() != vic.size())
  {
    cerr << "Error: the three vectors have different size!" << endl;
    cerr << "Error: the size of the first vector is " << via.size() << "." << endl;
    cerr << "Error: the size of the second vector is " << vib.size() << "." << endl;
    cerr << "Error: the size of the third vector is " << vic.size() << "." << endl;
    cerr << "Error: please check the three vectors!" << endl;
    exit(413);
  }
  if(via.size() == 0)
  {
    cerr << "Info: the sizes of the two vectors is zero" << endl;
    return;
  }
  int iatemp, ibtemp, ictemp;
  for(unsigned int i = 0; i < via.size() - 1; i++)
    for(unsigned int j = i + 1; j < via.size(); j++)
      if(via[i] < via[j])
      {
        iatemp = via[i];
        via[i] = via[j];
        via[j] = iatemp;
        ibtemp = vib[i];
        vib[i] = vib[j];
        vib[j] = ibtemp;
        ictemp = vic[i];
        vic[i] = vic[j];
        vic[j] = ictemp;
      }
}

void topoana::getResult()
{
  for(unsigned int i = 0; i < m_inputRootFileNames.size(); i++)
  {
    TFile* f = new TFile(m_inputRootFileNames[i].c_str());
    if(f->IsZombie())
    {
      cerr << "Error: File " << m_inputRootFileNames[i] << " is Zombie!" << endl;
      exit(511);
    }
    if(f) delete f;
  }

  TChain* chain = new TChain(m_inputTreeName.c_str());
  for(unsigned int i = 0; i < m_inputRootFileNames.size(); i++)
  { chain->Add(m_inputRootFileNames[i].c_str()); } Int_t Nmcps, Pid[50], Midx[50];
  chain->SetBranchAddress("Nmcps", &Nmcps);
  chain->SetBranchAddress("Pid", &Pid);
  chain->SetBranchAddress("Midx", &Midx);

  string outputRootFileName = m_outputMainFileName + ".root";
  TFile* file               = new TFile(outputRootFileName.c_str(), "recreate");
  TTree* tree               = chain->CloneTree(0);
  int    iEvtDcyTr, iEvtDcyIFChn, nSigPid[50], iSigEvtDcyTr, iSigEvtDcyIFChn;
  for(int i = 0; i < 50; i++) nSigPid[i] = 0;
  tree->Branch("IEvtDcyTr", &iEvtDcyTr, "iEvtDcyTr/I");
  tree->Branch("IEvtDcyIFChn", &iEvtDcyIFChn, "iEvtDcyIFChn/I");
  if(m_vSigPid.size() > 0) { tree->Branch("NSigPid", &nSigPid, "nSigPid[50]/I"); }
  if(m_vVSigPid.size() > 0)
  {
    tree->Branch("ISigEvtDcyTr", &iSigEvtDcyTr, "iSigEvtDcyTr/I");
    tree->Branch("ISigEvtDcyIFChn", &iSigEvtDcyIFChn, "iSigEvtDcyIFChn/I");
  }

  vector<int> vPid;
  vector<int> vMidx;

  m_vEvtDcyTr.clear();
  vector<list<int> > evtDcyTr;
  m_vNEvtDcyTr.clear();
  m_vEvtDcyIFChn.clear();
  list<int> evtDcyIFChn;
  m_vNEvtDcyIFChn.clear();

  if(m_vSigPid.size() > 0)
  {
    m_vNSigPid.clear();
    for(unsigned int i = 0; i < m_vSigPid.size(); i++)
    {
      m_vISigPid.push_back(i);
      m_vNSigPid.push_back(0);
    }
  }
  if(m_vVSigPid.size() > 0)
  {
    m_vSigEvtDcyTr.clear();
    vector<list<int> > sigEvtDcyTr;
    m_vNSigEvtDcyTr.clear();
    m_vSigEvtDcyIFChn.clear();
    list<int> sigEvtDcyIFChn;
    m_vNSigEvtDcyIFChn.clear();
    for(unsigned int i = 0; i < m_vVSigPid.size(); i++)
    {
      evtReorder(m_vVSigPid[i], m_vVSigMidx[i]);
      sigEvtDcyTr.clear();
      getEvtDcyTr(m_vVSigPid[i], m_vVSigMidx[i], sigEvtDcyTr);
      for(unsigned int j = 0; j < m_vSigEvtDcyTr.size(); j++)
      {
        if(sigEvtDcyTr == m_vSigEvtDcyTr[j])
        {
          cerr << "Error : The " << i + 1 << " signal event decay tree is same as the " << j + 1
               << " signal event decay tree ! Please check the input card and remove one of them !"
               << endl;
          exit(512);
        }
      }
      m_vSigEvtDcyTr.push_back(sigEvtDcyTr);
      m_vISigEvtDcyTr.push_back(m_vSigEvtDcyTr.size() - 1);
      m_vNSigEvtDcyTr.push_back(0);

      sigEvtDcyIFChn.clear();
      getEvtDcyIFChn(m_vVSigPid[i], m_vVSigMidx[i], sigEvtDcyIFChn);
      bool newSigEvtDcyIFChn = true;
      for(unsigned int j = 0; j < m_vSigEvtDcyIFChn.size(); j++)
      {
        if(sigEvtDcyIFChn == m_vSigEvtDcyIFChn[j])
        {
          cout << "Info : the final states of the " << i + 1
               << " signal event decay tree is same as those of one of former event decay trees ."
               << endl;
          newSigEvtDcyIFChn = false;
        }
      }
      if(newSigEvtDcyIFChn)
      {
        m_vSigEvtDcyIFChn.push_back(sigEvtDcyIFChn);
        m_vISigEvtDcyIFChn.push_back(m_vSigEvtDcyIFChn.size() - 1);
        m_vNSigEvtDcyIFChn.push_back(0);
      }
    }
  }
  for(unsigned int i = 0; i < chain->GetEntries(); i++)
  {
    // cout<<i<<endl;
    for(int j = 0; j < 50; j++) nSigPid[j] = 0;
    chain->GetEntry(i);
    vPid.clear();
    vMidx.clear();
    for(Int_t j = 0; j < Nmcps; j++)
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

    evtReorder(vPid, vMidx);
    evtDcyTr.clear();
    getEvtDcyTr(vPid, vMidx, evtDcyTr);
    evtDcyIFChn.clear();
    getEvtDcyIFChn(vPid, vMidx, evtDcyIFChn);
    bool newEvtDcyTr = true;
    int  whichEvtDcyTr;
    for(unsigned int j = 0; j < m_vEvtDcyTr.size(); j++)
    {
      if(evtDcyTr == m_vEvtDcyTr[j])
      {
        newEvtDcyTr   = false;
        whichEvtDcyTr = j;
        break;
      }
    }
    if(newEvtDcyTr == true)
    {
      m_vEvtDcyTr.push_back(evtDcyTr);
      m_vIEvtDcyTr.push_back(m_vEvtDcyTr.size() - 1);
      m_vNEvtDcyTr.push_back(1);
      iEvtDcyTr = m_vNEvtDcyTr.size() - 1;
    }
    else
    {
      m_vNEvtDcyTr[whichEvtDcyTr]++;
      iEvtDcyTr = whichEvtDcyTr;
    }
    bool newEvtDcyIFChn = true;
    int  whichEvtDcyIFChn;
    for(unsigned int j = 0; j < m_vEvtDcyIFChn.size(); j++)
    {
      if(evtDcyIFChn == m_vEvtDcyIFChn[j])
      {
        newEvtDcyIFChn   = false;
        whichEvtDcyIFChn = j;
        break;
      }
    }
    if(newEvtDcyIFChn == true)
    {
      m_vEvtDcyIFChn.push_back(evtDcyIFChn);
      m_vIEvtDcyIFChn.push_back(m_vEvtDcyIFChn.size() - 1);
      m_vNEvtDcyIFChn.push_back(1);
      iEvtDcyIFChn = m_vNEvtDcyIFChn.size() - 1;
    }
    else
    {
      m_vNEvtDcyIFChn[whichEvtDcyIFChn]++;
      iEvtDcyIFChn = whichEvtDcyIFChn;
    }
    if(m_vSigPid.size() > 0)
    {
      for(unsigned int j = 0; j < vPid.size(); j++)
      {
        for(unsigned int k = 0; k < m_vSigPid.size(); k++)
        {
          if(vPid[j] == m_vSigPid[k])
          {
            m_vNSigPid[k]++;
            nSigPid[k]++;
            break;
          }
        }
      }
    }
    if(m_vVSigPid.size() > 0)
    {
      for(unsigned int j = 0; j < m_vSigEvtDcyTr.size(); j++)
      {
        if(evtDcyTr == m_vSigEvtDcyTr[j])
        {
          m_vNSigEvtDcyTr[j]++;
          iSigEvtDcyTr = j;
          break;
        }
      }
      for(unsigned int j = 0; j < m_vSigEvtDcyIFChn.size(); j++)
      {
        if(evtDcyIFChn == m_vSigEvtDcyIFChn[j])
        {
          m_vNSigEvtDcyIFChn[j]++;
          iSigEvtDcyIFChn = j;
          break;
        }
      }
    }
    tree->Fill();
  }
  file->Write();
  sortByFirstFromLargeToSmall(m_vNEvtDcyTr, m_vEvtDcyTr, m_vIEvtDcyTr);
  sortByFirstFromLargeToSmall(m_vNEvtDcyIFChn, m_vEvtDcyIFChn, m_vIEvtDcyIFChn);
  if(m_vSigPid.size() > 0) { sortByFirstFromLargeToSmall(m_vNSigPid, m_vSigPid, m_vISigPid); }
  if(m_vVSigPid.size() > 0)
  {
    sortByFirstFromLargeToSmall(m_vNSigEvtDcyTr, m_vSigEvtDcyTr, m_vISigEvtDcyTr);
    sortByFirstFromLargeToSmall(m_vNSigEvtDcyIFChn, m_vSigEvtDcyIFChn, m_vISigEvtDcyIFChn);
  }
}

void topoana::makePidTxtpnameMap()
{
  m_pidTxtpnameMap.clear();
  int      pid;
  string   txtpname;
  ifstream infile("pid_txtpname.dat", ios::in);
  while(infile >> pid >> txtpname)
  {
    m_pidTxtpnameMap[pid] = txtpname;
    // cout<<pid<<"\t"<<m_pidTxtpnameMap[pid]<<endl;
  }
}

void topoana::outputResultIntoTxtFile()
{
  makePidTxtpnameMap();
  string   outputTxtFileName = m_outputMainFileName + ".txt";
  ofstream outfile(outputTxtFileName.c_str(), ios::out);

  outfile << "Event decay trees infomation :" << endl << endl;
  vector<list<int> > evtDcyTr;
  list<int>          evtDcyChn;
  int                naccdevents = 0;
  for(unsigned int i = 0; i < m_vNEvtDcyTr.size(); i++)
  {
    evtDcyTr.clear();
    evtDcyTr    = m_vEvtDcyTr[i];
    naccdevents = naccdevents + m_vNEvtDcyTr[i];
    outfile << "index:  " << i + 1 << "\tiEvtDcyTr:  " << m_vIEvtDcyTr[i]
            << "\tnevents:  " << m_vNEvtDcyTr[i] << "\tnaccdevents:  " << naccdevents << endl;
    for(unsigned int j = 0; j < evtDcyTr.size(); j++)
    {
      evtDcyChn.clear();
      evtDcyChn                = evtDcyTr[j];
      list<int>::iterator liit = evtDcyChn.begin();
      outfile << "  " << m_pidTxtpnameMap[*liit];
      if(j == 0 || (*liit) == 1 || (*liit) == 2 || (*liit) == 3 || (*liit) == 4 || (*liit) == 5 ||
         (*liit) == 6)
      {
        liit++;
        outfile << " " << m_pidTxtpnameMap[*liit];
      }
      outfile << " --> ";
      for(liit++; liit != evtDcyChn.end(); liit++) { outfile << m_pidTxtpnameMap[*liit] << " "; }
      outfile << endl;
    }
    outfile << endl;
  }
  outfile << endl << endl << endl;

  outfile << "Event decay initial-final chains infomation :" << endl << endl;
  list<int> evtDcyIFChn;
  naccdevents = 0;
  for(unsigned int i = 0; i < m_vNEvtDcyIFChn.size(); i++)
  {
    evtDcyIFChn.clear();
    evtDcyIFChn = m_vEvtDcyIFChn[i];
    naccdevents = naccdevents + m_vNEvtDcyIFChn[i];
    outfile << "index:  " << i + 1 << "\tiEvtDcyIFChn:  " << m_vIEvtDcyIFChn[i]
            << "\tnevents:  " << m_vNEvtDcyIFChn[i] << "\tnaccdevents:  " << naccdevents << endl;
    list<int>::iterator liit = evtDcyIFChn.begin();
    outfile << "  " << m_pidTxtpnameMap[*liit];
    liit++;
    outfile << " " << m_pidTxtpnameMap[*liit] << " --> ";
    for(liit++; liit != evtDcyIFChn.end(); liit++) { outfile << m_pidTxtpnameMap[*liit] << " "; }
    outfile << endl << endl;
  }
  outfile << endl << endl << endl;

  if(m_vSigPid.size() > 0)
  {
    outfile << "Signal particles infomation :" << endl << endl;
    naccdevents = 0;
    for(unsigned int i = 0; i < m_vNSigPid.size(); i++)
    {
      naccdevents = naccdevents + m_vNSigPid[i];
      outfile << "index:  " << i + 1 << "\tiSigPid:  " << m_vISigPid[i]
              << "\tSigName:  " << m_pidTxtpnameMap[m_vSigPid[i]]
              << "\tnevnets:   " << m_vNSigPid[i] << "\tnaccdevents:  " << naccdevents << endl
              << endl;
    }
    outfile << endl << endl << endl;
  }

  if(m_vVSigPid.size() > 0)
  {
    outfile << "Signal event decay trees infomation :" << endl << endl;
    vector<list<int> > sigEvtDcyTr;
    list<int>          sigEvtDcyChn;
    naccdevents = 0;
    for(unsigned int i = 0; i < m_vNSigEvtDcyTr.size(); i++)
    {
      sigEvtDcyTr.clear();
      sigEvtDcyTr = m_vSigEvtDcyTr[i];
      naccdevents = naccdevents + m_vNSigEvtDcyTr[i];
      outfile << "index:  " << i + 1 << "\tiSigEvtDcyTr:  " << m_vISigEvtDcyTr[i]
              << "\tnevents:  " << m_vNSigEvtDcyTr[i] << "\tnaccdevents:  " << naccdevents << endl;
      for(unsigned int j = 0; j < sigEvtDcyTr.size(); j++)
      {
        sigEvtDcyChn.clear();
        sigEvtDcyChn             = sigEvtDcyTr[j];
        list<int>::iterator liit = sigEvtDcyChn.begin();
        outfile << "  " << m_pidTxtpnameMap[*liit];
        if(j == 0)
        {
          liit++;
          outfile << " " << m_pidTxtpnameMap[*liit];
        }
        outfile << " --> ";
        for(liit++; liit != sigEvtDcyChn.end(); liit++)
        { outfile << m_pidTxtpnameMap[*liit] << " "; } outfile << endl;
      }
      outfile << endl;
    }
    outfile << endl << endl << endl;

    outfile << "Signal event decay initial-final chains infomation :" << endl << endl;
    list<int> sigEvtDcyIFChn;
    naccdevents = 0;
    for(unsigned int i = 0; i < m_vNSigEvtDcyIFChn.size(); i++)
    {
      sigEvtDcyIFChn.clear();
      sigEvtDcyIFChn = m_vSigEvtDcyIFChn[i];
      naccdevents    = naccdevents + m_vNSigEvtDcyIFChn[i];
      outfile << "index:  " << i + 1 << "\tiSigEvtDcyIFChn:  " << m_vISigEvtDcyIFChn[i]
              << "\tnevents:  " << m_vNSigEvtDcyIFChn[i] << "\tnaccdevents:  " << naccdevents
              << endl;
      list<int>::iterator liit = sigEvtDcyIFChn.begin();
      outfile << "  " << m_pidTxtpnameMap[*liit];
      liit++;
      outfile << " " << m_pidTxtpnameMap[*liit] << " --> ";
      for(liit++; liit != sigEvtDcyIFChn.end(); liit++)
      { outfile << m_pidTxtpnameMap[*liit] << " "; } outfile << endl << endl;
    }
  }
}

void topoana::makePidTexpnameMap()
{
  m_pidTexpnameMap.clear();
  int      pid;
  string   texpname;
  ifstream infile("pid_texpname.dat", ios::in);
  while(infile >> pid >> texpname)
  {
    m_pidTexpnameMap[pid] = texpname;
    // cout<<pid<<"\t"<<m_pidTexpnameMap[pid]<<endl;
  }
}

void topoana::outputResultIntoTexFile()
{
  makePidTexpnameMap();
  string   outputTexFileName = m_outputMainFileName + ".tex";
  ofstream outfile(outputTexFileName.c_str(), ios::out);

  outfile << "\\documentclass[landscape]{article}" << endl;
  outfile << "\\usepackage{geometry}" << endl;
  outfile << "\\geometry{left=0.0cm,right=0.0cm,top=2.5cm,bottom=2.5cm}" << endl;
  outfile << "\\usepackage{array}" << endl;
  outfile << "\\begin{document}" << endl;
  outfile << "\\title{topology analysis}" << endl;
  outfile << "\\author{Xingyu Zhou}" << endl;
  outfile << "\\maketitle" << endl;
  outfile << endl << endl << endl;

  vector<list<int> > evtDcyTr;
  list<int>          evtDcyChn;
  int                naccdevents = 0;
  for(unsigned int i = 0; i < m_vNEvtDcyTr.size(); i++)
  {
    if(i % 35 == 0)
    {
      outfile << "\\clearpage" << endl;
      outfile << "\\begin{table}[htbp!]" << endl;
      if(i == 0) outfile << "\\caption{event decay trees}" << endl;
      outfile << "\\small" << endl;
      outfile << "\\centering" << endl;
      outfile << "\\begin{tabular}{|c|>{\\centering\\arraybackslash}p{20cm}|c|c|c|}" << endl;
      outfile << "\\hline" << endl;
      outfile << "index & event decay tree & iEvtDcyTr & nevents & naccdevents \\\\" << endl;
      outfile << "\\hline" << endl;
    }
    outfile << i + 1 << " & $";
    evtDcyTr.clear();
    evtDcyTr                   = m_vEvtDcyTr[i];
    naccdevents                = naccdevents + m_vNEvtDcyTr[i];
    unsigned int nVldEvtDcyChn = 0;
    for(unsigned int j = 0; j < evtDcyTr.size(); j++)
    {
      evtDcyChn.clear();
      evtDcyChn                = evtDcyTr[j];
      list<int>::iterator liit = evtDcyChn.begin();
      if(((unsigned int)(*liit)) == 111)
        continue;
      else
        nVldEvtDcyChn++;
      outfile << m_pidTexpnameMap[*liit] << " ";
      if(j == 0 || (*liit) == 1 || (*liit) == 2 || (*liit) == 3 || (*liit) == 4 || (*liit) == 5 ||
         (*liit) == 6)
      {
        liit++;
        outfile << m_pidTexpnameMap[*liit] << " ";
      }
      outfile << "\\rightarrow ";
      for(liit++; liit != evtDcyChn.end(); liit++) { outfile << m_pidTexpnameMap[*liit] << " "; }
      bool      lastOneOrAllPiZerosBehind = true;
      list<int> evtDcyChntemp;
      for(unsigned int k = j + 1; k < evtDcyTr.size(); k++)
      {
        evtDcyChntemp.clear();
        evtDcyChntemp                = evtDcyTr[k];
        list<int>::iterator liittemp = evtDcyChntemp.begin();
        if(((unsigned int)(*liittemp)) != 111)
        {
          lastOneOrAllPiZerosBehind = false;
          break;
        }
      }
      if(!lastOneOrAllPiZerosBehind)
      {
        outfile << ",";
        if(nVldEvtDcyChn % 8 == 0) outfile << "\\newline" << endl;
      }
      else
        break;
    }
    outfile << "$ & " << m_vIEvtDcyTr[i] << " & " << m_vNEvtDcyTr[i] << " & " << naccdevents
            << " \\\\" << endl;
    outfile << "\\hline" << endl;
    if(i % 35 == 34 || i == m_vNEvtDcyTr.size() - 1)
    {
      outfile << "\\end{tabular}" << endl;
      outfile << "\\end{table}" << endl;
    }
  }
  outfile << endl << endl << endl;

  list<int> evtDcyIFChn;
  naccdevents = 0;
  for(unsigned int i = 0; i < m_vNEvtDcyIFChn.size(); i++)
  {
    if(i % 35 == 0)
    {
      outfile << "\\clearpage" << endl;
      outfile << "\\begin{table}[htbp!]" << endl;
      if(i == 0) outfile << "\\caption{event decay initial-final chain}" << endl;
      outfile << "\\small" << endl;
      outfile << "\\centering" << endl;
      outfile << "\\begin{tabular}{|c|c|c|c|c|}" << endl;
      outfile << "\\hline" << endl;
      outfile
        << "index & event decay initial-final chain & iEvtDcyIFChn & nevents & naccdevents \\\\"
        << endl;
      outfile << "\\hline" << endl;
    }
    outfile << i + 1 << " & $";
    evtDcyIFChn.clear();
    evtDcyIFChn              = m_vEvtDcyIFChn[i];
    naccdevents              = naccdevents + m_vNEvtDcyIFChn[i];
    list<int>::iterator liit = evtDcyIFChn.begin();
    outfile << m_pidTexpnameMap[*liit] << " ";
    liit++;
    outfile << m_pidTexpnameMap[*liit] << " ";
    outfile << "\\rightarrow ";
    for(liit++; liit != evtDcyIFChn.end(); liit++) { outfile << m_pidTexpnameMap[*liit] << " "; }
    outfile << "$ & " << m_vIEvtDcyIFChn[i] << " & " << m_vNEvtDcyIFChn[i] << " & " << naccdevents
            << " \\\\" << endl;
    outfile << "\\hline" << endl;
    if(i % 35 == 34 || i == m_vNEvtDcyIFChn.size() - 1)
    {
      outfile << "\\end{tabular}" << endl;
      outfile << "\\end{table}" << endl;
    }
  }
  outfile << endl << endl << endl;

  naccdevents = 0;
  if(m_vSigPid.size() > 0)
  {
    for(unsigned int i = 0; i < m_vNSigPid.size(); i++)
    {
      if(i % 35 == 0)
      {
        outfile << "\\clearpage" << endl;
        outfile << "\\begin{table}[htbp!]" << endl;
        if(i == 0) outfile << "\\caption{signal particle}" << endl;
        outfile << "\\small" << endl;
        outfile << "\\centering" << endl;
        outfile << "\\begin{tabular}{|c|c|c|c|c|}" << endl;
        outfile << "\\hline" << endl;
        outfile << "index & signal particle & iSigPid & nevents & naccdevents \\\\" << endl;
        outfile << "\\hline" << endl;
      }
      naccdevents = naccdevents + m_vNSigPid[i];
      outfile << i + 1 << " & $" << m_pidTexpnameMap[m_vSigPid[i]] << " "
              << "$ & " << m_vISigPid[i] << " & " << m_vNSigPid[i] << " & " << naccdevents
              << " \\\\" << endl;
      outfile << "\\hline" << endl;
      if(i % 35 == 34 || i == m_vNSigPid.size() - 1)
      {
        outfile << "\\end{tabular}" << endl;
        outfile << "\\end{table}" << endl;
      }
    }
    outfile << endl << endl << endl;
  }

  if(m_vVSigPid.size() > 0)
  {
    vector<list<int> > sigEvtDcyTr;
    list<int>          sigEvtDcyChn;
    naccdevents = 0;
    for(unsigned int i = 0; i < m_vNSigEvtDcyTr.size(); i++)
    {
      if(i % 35 == 0)
      {
        outfile << "\\clearpage" << endl;
        outfile << "\\begin{table}[htbp!]" << endl;
        if(i == 0) outfile << "\\caption{signal event decay trees}" << endl;
        outfile << "\\small" << endl;
        outfile << "\\centering" << endl;
        outfile << "\\begin{tabular}{|c|>{\\centering\\arraybackslash}p{20cm}|c|c|c|}" << endl;
        outfile << "\\hline" << endl;
        outfile << "index & signal event decay trees & iSigEvtDcyTr & nevents & naccdevents \\\\"
                << endl;
        outfile << "\\hline" << endl;
      }
      outfile << i + 1 << " & $";
      sigEvtDcyTr.clear();
      sigEvtDcyTr                   = m_vSigEvtDcyTr[i];
      naccdevents                   = naccdevents + m_vNSigEvtDcyTr[i];
      unsigned int nVldSigEvtDcyChn = 0;
      for(unsigned int j = 0; j < sigEvtDcyTr.size(); j++)
      {
        sigEvtDcyChn.clear();
        sigEvtDcyChn             = sigEvtDcyTr[j];
        list<int>::iterator liit = sigEvtDcyChn.begin();
        if(((unsigned int)(*liit)) == 111)
          continue;
        else
          nVldSigEvtDcyChn++;
        outfile << m_pidTexpnameMap[*liit] << " ";
        if(j == 0)
        {
          liit++;
          outfile << m_pidTexpnameMap[*liit] << " ";
        }
        outfile << "\\rightarrow ";
        for(liit++; liit != sigEvtDcyChn.end(); liit++)
        { outfile << m_pidTexpnameMap[*liit] << " "; } bool lastOneOrAllPiZerosBehind = true;
        list<int>                                           sigEvtDcyChntemp;
        for(unsigned int k = j + 1; k < sigEvtDcyTr.size(); k++)
        {
          sigEvtDcyChntemp.clear();
          sigEvtDcyChntemp             = sigEvtDcyTr[k];
          list<int>::iterator liittemp = sigEvtDcyChntemp.begin();
          if(((unsigned int)(*liittemp)) != 111)
          {
            lastOneOrAllPiZerosBehind = false;
            break;
          }
        }
        if(!lastOneOrAllPiZerosBehind)
        {
          outfile << ",";
          if(nVldSigEvtDcyChn % 8 == 0) outfile << "\\newline" << endl;
        }
        else
          break;
      }
      outfile << "$ & " << m_vISigEvtDcyTr[i] << " & " << m_vNSigEvtDcyTr[i] << " & " << naccdevents
              << " \\\\" << endl;
      outfile << "\\hline" << endl;
      if(i % 35 == 34 || i == m_vNSigEvtDcyTr.size() - 1)
      {
        outfile << "\\end{tabular}" << endl;
        outfile << "\\end{table}" << endl;
      }
    }
    outfile << endl << endl << endl;

    list<int> sigEvtDcyIFChn;
    naccdevents = 0;
    for(unsigned int i = 0; i < m_vNSigEvtDcyIFChn.size(); i++)
    {
      if(i % 35 == 0)
      {
        outfile << "\\clearpage" << endl;
        outfile << "\\begin{table}[htbp!]" << endl;
        if(i == 0) outfile << "\\caption{signal event decay initial-final chain}" << endl;
        outfile << "\\small" << endl;
        outfile << "\\centering" << endl;
        outfile << "\\begin{tabular}{|c|c|c|c|c|}" << endl;
        outfile << "\\hline" << endl;
        outfile << "index & signal event decay initial-final chain & iSigEvtDcyIFChn & nevents & "
                   "naccdevents \\\\"
                << endl;
        outfile << "\\hline" << endl;
      }
      outfile << i + 1 << " & $";
      sigEvtDcyIFChn.clear();
      sigEvtDcyIFChn           = m_vSigEvtDcyIFChn[i];
      naccdevents              = naccdevents + m_vNSigEvtDcyIFChn[i];
      list<int>::iterator liit = sigEvtDcyIFChn.begin();
      outfile << m_pidTexpnameMap[*liit] << " ";
      liit++;
      outfile << m_pidTexpnameMap[*liit] << " ";
      outfile << "\\rightarrow ";
      for(liit++; liit != sigEvtDcyIFChn.end(); liit++)
      { outfile << m_pidTexpnameMap[*liit] << " "; }
      outfile << "$ & " << m_vISigEvtDcyIFChn[i] << " & " << m_vNSigEvtDcyIFChn[i] << " & "
              << naccdevents << " \\\\" << endl;
      outfile << "\\hline" << endl;
      if(i % 35 == 34 || i == m_vNSigEvtDcyIFChn.size() - 1)
      {
        outfile << "\\end{tabular}" << endl;
        outfile << "\\end{table}" << endl;
      }
    }
    outfile << endl << endl << endl;
  }

  outfile << "\\end{document}" << endl;
}

void topoana::convertTexFileIntoPdfFile()
{
  string outputTexFileName = m_outputMainFileName + ".tex";
  string pdflatexcmd       = "pdflatex " + outputTexFileName;
  system(pdflatexcmd.c_str());

  string outputAUXFileName = m_outputMainFileName + ".aux";
  string outputLOGFileName = m_outputMainFileName + ".log";
  string rmcmd             = "rm " + outputAUXFileName + " " + outputLOGFileName;
  system(rmcmd.c_str());
}

int main(int argc, char* argv[])
{
  topoana ta;
  if(argc > 1) { ta.readCard(argv[1]); }
  else
  {
    string defaultTopoAnaCardName = "topoana.card";
    ta.readCard(defaultTopoAnaCardName);
  }
  cout << "debug1" << endl;
  ta.getResult();
  cout << "debug2" << endl;
  ta.outputResultIntoTxtFile();
  ta.outputResultIntoTexFile();
  ta.convertTexFileIntoPdfFile();
  return 0;
}
