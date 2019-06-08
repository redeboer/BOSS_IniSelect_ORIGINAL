#include "TBranch.h"
#include "TError.h"
#include "TFile.h"
#include "TKey.h"
#include "TList.h"
#include "TString.h"
#include "TTree.h"
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
using namespace std;

Bool_t compareFileSize = false;
Bool_t compareNentries = false;
Bool_t printTree = false;
char buffer [1024];

template <class T>
struct AddressPair
{
  T val1;
  T val2;
};

void Throw(char const * const format, ...)
{
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  throw runtime_error(Form("\033[31m%s\033[0m", buffer));
}

void CompareObject(TObject* obj1, TObject* obj2)
{
  if(obj1->Compare(obj2)) Throw("Unequal object \"%s\"", obj1->GetName());
}

void CompareTree(TTree* tree1, TTree* tree2)
{
  if(!tree1 || !tree2) Throw("Nullpointer trees");
  TString name = tree1->GetName();
  if(!name.EqualTo(tree2->GetName())) Throw("Tree names are not the same: \"%s\" vs \"%s\"", tree1->GetName(), tree2->GetName());
  if(compareNentries) if(tree1->GetEntries() != tree2->GetEntries()) Throw("Unequal number of entries for tree \"%s\"", tree1->GetName());

  TObjArray* list1 = tree1->GetListOfBranches();
  TObjArray* list2 = tree2->GetListOfBranches();
  if(list1->GetSize() != list2->GetSize())
    Throw("Unequal number of branches for tree \"%s\"", tree1->GetName());
  map<string, AddressPair<Double_t> > addresses;

  cout << "Comparing tree \"" << tree1->GetName() << "\"" << endl;
  for(Int_t i = 0; i < list1->GetSize(); ++i)
  {
    TBranch* branch1 = dynamic_cast<TBranch*>(list1->At(i));
    TBranch* branch2 = dynamic_cast<TBranch*>(list2->At(i));
    if((branch1 && !branch2) || (!branch1 && branch2)) Throw("Different pointer types for branch nr. %d", i);
    if(!branch1 || !branch2) continue;
    string branchName = branch1->GetName();
    if(branchName.compare(branch2->GetName())) Throw("Unequal branch name: \"%s\" vs \"\"", branch1->GetName(), branch2->GetName());
    Int_t result1 = tree1->SetBranchAddress(branch1->GetName(), &addresses[branchName].val1);
    Int_t result2 = tree2->SetBranchAddress(branch2->GetName(), &addresses[branchName].val2);
    if(!result1 || !result2)
    {
      tree1->ResetBranchAddress(branch1);
      tree2->ResetBranchAddress(branch2);
    }
    if(result1 != result2)
      Throw("Different branch type tree \"%s\": %s vs %s", tree1->GetName(), branch1->GetTitle(), tree2->GetBranch(branch1->GetName())->GetTitle());
  }
  if(printTree)
  {
    cout << endl;
    tree1->Print();
    cout << endl;
  }
  for(Int_t i = 0; i < tree1->GetEntries() && i < tree2->GetEntries(); ++i)
  {
    Int_t result1 = tree1->GetEntry(i);
    Int_t result2 = tree2->GetEntry(i);
    cout << "  entry " << i + 1 << "/" << tree1->GetEntries() << "\t" << result1 << "\t" << result2 << endl;
    // if(result1 != result2) Throw("Output for GetEntry in entry %d unequal", i);
    map<string, AddressPair<Double_t> >::iterator it;
    for(it = addresses.begin(); it != addresses.end(); ++it)
    {
      cout << "    " << it->first << ": " << it->second.val1 << ", " << it->second.val2 << endl;
      if(it->second.val1 != it->second.val2) Throw("Entry %l unequal in branch \"%s\" of tree \"%s\": %g vs %g", i, it->first.c_str(), tree1->GetName(), it->second.val1, it->second.val2);
    }
  }
  tree1->ResetBranchAddresses();
  tree2->ResetBranchAddresses();
}

int CompareRootFiles(const char* name1, const char* name2)
{
  gErrorIgnoreLevel = 6000;
  try
  {
    TFile* file1 = new TFile(name1);
    TFile* file2 = new TFile(name2);
    if(file1->IsZombie()) Throw("Failed to load file \"%s\"", name1);
    if(file2->IsZombie()) Throw("Failed to load file \"%s\"", name2);

    if(compareFileSize && (file1->GetSize() != file2->GetSize()))
      Throw("\033[31mUnequal file size for files:\n  \"%s\"\n  \"%s\"\033[0m", name1, name2);

    TIter it1(file1->GetListOfKeys());
    TIter it2(file2->GetListOfKeys());


    while(TKey* key = (TKey*)it1.Next())
    {
      TObject* obj1 = key->ReadObj();
      if(!obj1) Throw("Missing object \"%s\" in file \"%s\"", obj1->GetName(), name1);
      TObject* obj2 = file2->Get(obj1->GetName());
      if(!obj2) Throw("Missing object \"%s\" in file \"%s\"", obj2->GetName(), name2);
      CompareObject(obj1, obj2);
      TTree* tree1 = dynamic_cast<TTree*>(obj1);
      TTree* tree2 = dynamic_cast<TTree*>(obj2);
      if(tree1 && tree2) CompareTree(tree1, tree2);
    }
    file1->Close();
    file2->Close();
  }
  catch(const runtime_error& e)
  {
    cout << e.what() << endl;
    return 1;
  }
  cout << "\033[32mOutput files are identical!\033[0m" << endl;
  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 3) Throw("CompareRootFiles needs 2 arguments");
  return CompareRootFiles(argv[1], argv[2]);
}