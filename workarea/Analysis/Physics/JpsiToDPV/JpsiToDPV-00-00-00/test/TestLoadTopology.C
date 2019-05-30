#include "TDatabasePDG.h"
#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include <stdexcept>
#include <iomanip>
#include <iostream>
using namespace std;

void TestLoadTopology()
{
  const char* filename = "TopoAna_vector.root";
  const char* treename = "topology";

  const char* runidName    = "runid";
  const char* evtidName    = "evtid";
  const char* particleName = "PDG1";
  const char* motherName   = "mother";

  Bool_t printStructure = false;
  Int_t  maxEvents = 5;

  TFile f(filename);
  if(f.IsZombie()) throw runtime_error(Form("File \"%s\" does not exist", filename));

  TTree* t = dynamic_cast<TTree*>(f.Get(treename));
  if(!t) throw runtime_error(Form("No tree named \"%s\"", treename));

  if(printStructure)
  {
    f.ls();
    t->Print();
  }

  Int_t          runid;
  Int_t          evtid;
  vector<Int_t>* particle = NULL; // nullptr initialisation is essential!
  vector<Int_t>* mother   = NULL; // nullptr initialisation is essential!

  if(t->SetBranchAddress(runidName, &runid))
    throw runtime_error(Form("Failed to load branch \"%s\"", runidName));
  if(t->SetBranchAddress(evtidName, &evtid))
    throw runtime_error(Form("Failed to load branch \"%s\"", evtidName));
  if(t->SetBranchAddress(particleName, &particle))
    throw runtime_error(Form("Failed to load branch \"%s\"", particleName));
  if(t->SetBranchAddress(motherName, &mother))
    throw runtime_error(Form("Failed to load branch \"%s\"", motherName));

  TDatabasePDG d;

  cout << "\033[1mLooping over tree with " << t->GetEntries() << " entries"
       << "\033[0m" << endl;
  for(Int_t i = 0; i < maxEvents && i < t->GetEntries(); ++i)
  {
    t->GetEntry(i);
    cout << "\033[1m  Entry " << i << " (run " << runid << ", evt " << evtid << "):\033[0m" << endl;
    if(!particle) continue;
    if(!mother) continue;
    cout << "    has " << particle->size() << " particles:" << endl;
    for(size_t j = 0; j < particle->size(); ++j)
    {
      Int_t p = particle->at(j);
      Int_t m = particle->at(mother->at(j));
      cout << setw(11);
      cout << d.GetParticle(p)->GetName() << " from ";
      cout << d.GetParticle(m)->GetName() << endl;
    }
  }
}

int main()
{
  TestLoadTopology();
  return 0;
}