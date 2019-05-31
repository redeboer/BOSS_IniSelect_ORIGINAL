#include "TError.h"
#include "TFile.h"
#include "TKey.h"
#include "TList.h"
#include "TString.h"
#include "TTree.h"
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

Bool_t compareSize = false;
char buffer [1024];

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
  // if(tree1->GetEntries() != tree2->GetEntries()) Throw("Unequal number of entries for tree \"%s\"", tree1->GetName());
  if(tree1->GetListOfBranches()->GetSize() != tree2->GetListOfBranches()->GetSize())
    Throw("Unequal number of branches for tree \"%s\"", tree1->GetName());
  vector<TString> branchName;
  vector<Bool_t> ok(tree1->GetListOfBranches()->GetSize());
  vector<Double_t> val1(tree1->GetListOfBranches()->GetSize());
  vector<Double_t> val2(tree2->GetListOfBranches()->GetSize());
  size_t j = 0;
  TIter it(tree1->GetListOfBranches());
  while(TObject* branch = it.Next())
  {
    branchName.push_back(branch->GetName());
    Int_t result1 = tree1->SetBranchAddress(branch->GetName(), &val1[j]);
    Int_t result2 = tree1->SetBranchAddress(branch->GetName(), &val1[j]);
    ok[j] = !result1 && !result2;
    // if(result1 != result2)
    //   Throw("Different branch type tree \"%s\": %s vs %s", tree1->GetName(), branch->GetTitle(), tree2->GetBranch(branch->GetName())->GetTitle());
    ++j;
  }

  for(Int_t i = 0; i < tree1->GetEntries(); ++i)
  {
    if(tree1->GetEntry(i) != tree2->GetEntry(i)) Throw("Output for GetEntry in entry %d unequal", i);
    for(j = 0; j < val1.size(); ++j)
    {
      if(!ok[j]) continue;
      cout << branchName[j] << ": " << val1[j] << ", " << val2[j] << endl;
      // if(val1[j] != val2[j]) Throw("Entry %l unequal in branch \"%s\" of tree \"%s\": %g vs %g", i, branchName[j].Data(), tree1->GetName(), val1[j], val2[j]);
    }
  }
}

int CompareRootFiles(const char* name1, const char* name2)
{
  gErrorIgnoreLevel = 6000;
  try
  {
    TFile file1(name1);
    TFile file2(name2);
    if(file1.IsZombie()) Throw("Failed to load file \"%s\"", name1);
    if(file2.IsZombie()) Throw("Failed to load file \"%s\"", name2);

    if(compareSize && (file1.GetSize() != file2.GetSize()))
      Throw("\033[31mUnequal file size for files:\n  \"%s\"\n  \"%s\"\033[0m", name1, name2);

    TIter it1(file1.GetListOfKeys());
    TIter it2(file2.GetListOfKeys());


    while(TKey* key = (TKey*)it1.Next())
    {
      TObject* obj1 = key->ReadObj();
      if(!obj1) Throw("Missing object \"%s\" in file \"%s\"", obj1->GetName(), name1);
      TObject* obj2 = file2.Get(obj1->GetName());
      if(!obj2) Throw("Missing object \"%s\" in file \"%s\"", obj2->GetName(), name2);
      CompareObject(obj1, obj2);
      TTree* tree1 = dynamic_cast<TTree*>(obj1);
      TTree* tree2 = dynamic_cast<TTree*>(obj2);
      if(tree1 && tree2) CompareTree(tree1, tree2);
    }
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