#include "../include/topoana.h"
#include <iostream>

unsigned int topoana::countIncOrIRASeqDcyBrsInDcyTr(vector<int> vIIncIncOrIRASeqDcyBrs, vector<int> vIIRAIncOrIRASeqDcyBrs, vector< list<int> > & incOrIRASeqDcyBrs, vector<int> vIdxOfHead1, vector<int> vMidxOfHead1, vector< list<int> > & dcyTr, vector<int> vIdxOfHead2, vector<int> vMidxOfHead2, vector<int> vPid, vector<int> vMidx)
{
  unsigned int nCount=0;

  if(incOrIRASeqDcyBrs.size()==0)
    {
      cerr<<"Infor: The size of the vector for the inclusive or intermediate-resonance-allowed sequential decay branches is zero!"<<endl<<endl;
      return nCount;
    }

  if(dcyTr.size()==0)
    {
      cerr<<"Infor: The size of the vector for the decay tree is zero!"<<endl<<endl;
      return nCount;
    }

  if(incOrIRASeqDcyBrs.size()<=dcyTr.size())
    {
      /*for(unsigned int i=0;i<incOrIRASeqDcyBrs.size();i++)
        {
          list<int>::iterator liit,liitTmp=incOrIRASeqDcyBrs[i].end();liitTmp--;
          for(liit=incOrIRASeqDcyBrs[i].begin();liit!=incOrIRASeqDcyBrs[i].end();liit++)
            {
              if(liit!=liitTmp) cout<<(*liit)<<"\t";
              else cout<<(*liit)<<endl;
            }
        }
      cout<<endl;
      for(unsigned int i=0;i<dcyTr.size();i++)
        {
          list<int>::iterator liit,liitTmp=dcyTr[i].end();liitTmp--;
          for(liit=dcyTr[i].begin();liit!=dcyTr[i].end();liit++)
            {
              if(liit!=liitTmp) cout<<(*liit)<<"\t";
              else cout<<(*liit)<<endl;
            }
        }
      cout<<endl;*/

      vector<int> vMIDcyBr1WRTIDcyBr1;
      vMIDcyBr1WRTIDcyBr1.push_back(-1);
      for(unsigned int i=1;i<incOrIRASeqDcyBrs.size();i++)
        {
          if(vIdxOfHead1[i]==vMidxOfHead1[i])
            {
              vMIDcyBr1WRTIDcyBr1.push_back(0);
            }
          else
            {
              for(unsigned int j=0;j<i;j++)
                {
                  if(vIdxOfHead1[j]==vMidxOfHead1[i])
                    {
                      vMIDcyBr1WRTIDcyBr1.push_back(j);
                      break;
                    }
                }
            }
        }

      vector< vector<int> > vVIDcyBr2WRTIDcyBr1;
      vVIDcyBr2WRTIDcyBr1.clear();
      vector<int> vIDcyBr2WRTIDcyBr1;
      vector<int> subsetOfVPidFromAP;
      vector<int> subsetOfVMidxFromAP; 
      bool headsAreRequiredToBeMatched=true;
      if(vIIncIncOrIRASeqDcyBrs[0]==0)
        {
          for(unsigned int i=0;i<dcyTr.size();i++)
            {
              if(dcyTr[i]==incOrIRASeqDcyBrs[0])
                {
                  vIDcyBr2WRTIDcyBr1.clear();
                  vIDcyBr2WRTIDcyBr1.push_back(i);
                  vVIDcyBr2WRTIDcyBr1.push_back(vIDcyBr2WRTIDcyBr1);
                }
            }
        }
      else
        {
          for(unsigned int i=0;i<dcyTr.size();i++)
            {
              if(isLiaMatchedWithLib(incOrIRASeqDcyBrs[0],dcyTr[i]))
                {
                  vIDcyBr2WRTIDcyBr1.clear();
                  vIDcyBr2WRTIDcyBr1.push_back(i);
                  vVIDcyBr2WRTIDcyBr1.push_back(vIDcyBr2WRTIDcyBr1);
                }
            }
        } 

      for(unsigned int i=0;i<vVIDcyBr2WRTIDcyBr1.size();i++)
        {
          for(unsigned int j=1;j<incOrIRASeqDcyBrs.size();j++)
            {
              for(unsigned int k=vVIDcyBr2WRTIDcyBr1[i][j-1]+1;k<dcyTr.size();k++)
                {
                  if(vIIncIncOrIRASeqDcyBrs[j]==0)
                    {                  
                      if(vIIRAIncOrIRASeqDcyBrs[j]==0)
                        {
                          if(dcyTr[k]==incOrIRASeqDcyBrs[j]&&(vMidxOfHead2[k]==vIdxOfHead2[k]||(vIIRAIncOrIRASeqDcyBrs[j-1]==0&&vMidxOfHead2[k]==vIdxOfHead2[(unsigned int) vVIDcyBr2WRTIDcyBr1[i][(unsigned int) vMIDcyBr1WRTIDcyBr1[j]]])||(vIIRAIncOrIRASeqDcyBrs[j-1]==1&&isPaADescendantOfPb(vMidx,vIdxOfHead2[k],vIdxOfHead2[(unsigned int) vVIDcyBr2WRTIDcyBr1[i][(unsigned int) vMIDcyBr1WRTIDcyBr1[j]]]))))
                            {
                              vVIDcyBr2WRTIDcyBr1[i].push_back(k);
                              break;
                            }
                        }
                      else
                        {
                          if((*(dcyTr[k].begin()))==(*(incOrIRASeqDcyBrs[j].begin())))
                            {
                              subsetOfVPidFromAP=vPid;
                              subsetOfVMidxFromAP=vMidx;
                              getSubsetsOfVPidandVMidxFromAP(subsetOfVPidFromAP,subsetOfVMidxFromAP,vIdxOfHead2[k]);
                              if(countIRADcyBr(subsetOfVPidFromAP,subsetOfVMidxFromAP,incOrIRASeqDcyBrs[j],headsAreRequiredToBeMatched)&&(vMidxOfHead2[k]==vIdxOfHead2[k]||(vIIRAIncOrIRASeqDcyBrs[j-1]==0&&vMidxOfHead2[k]==vIdxOfHead2[(unsigned int) vVIDcyBr2WRTIDcyBr1[i][(unsigned int) vMIDcyBr1WRTIDcyBr1[j]]])||(vIIRAIncOrIRASeqDcyBrs[j-1]==1&&isPaADescendantOfPb(vMidx,vIdxOfHead2[k],vIdxOfHead2[(unsigned int) vVIDcyBr2WRTIDcyBr1[i][(unsigned int) vMIDcyBr1WRTIDcyBr1[j]]]))))
                                {
                                  vVIDcyBr2WRTIDcyBr1[i].push_back(k);
                                  break;
                                }
                            }                    
                        }
                    }
                  else
                    {
                      if(isLiaMatchedWithLib(incOrIRASeqDcyBrs[j],dcyTr[k])&&(vMidxOfHead2[k]==vIdxOfHead2[k]||(vIIRAIncOrIRASeqDcyBrs[j-1]==0&&vMidxOfHead2[k]==vIdxOfHead2[(unsigned int) vVIDcyBr2WRTIDcyBr1[i][(unsigned int) vMIDcyBr1WRTIDcyBr1[j]]])||(vIIRAIncOrIRASeqDcyBrs[j-1]==1&&isPaADescendantOfPb(vMidx,vIdxOfHead2[k],vIdxOfHead2[(unsigned int) vVIDcyBr2WRTIDcyBr1[i][(unsigned int) vMIDcyBr1WRTIDcyBr1[j]]]))))
                        {
                          vVIDcyBr2WRTIDcyBr1[i].push_back(k);
                          break;
                        }
                    }
                }
              if(vVIDcyBr2WRTIDcyBr1[i].size()!=j+1) break;
            }
          if(vVIDcyBr2WRTIDcyBr1[i].size()==incOrIRASeqDcyBrs.size()) nCount++;
        }
    }

  return nCount;
}
