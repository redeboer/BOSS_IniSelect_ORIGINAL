#include "TFile.h"
#include "TString.h"
#include "TTree.h"
#include <iomanip>
#include <iostream>
#include <stdexcept>
using namespace std;

int CompareRootFiles(const char* name1, const char* name2)
{
  TFile file1(name1);
  TFile file2(name2);
  if(file1.IsZombie()) throw runtime_error(Form("Failed to load file \"%s\"", name1));
  if(file2.IsZombie()) throw runtime_error(Form("Failed to load file \"%s\"", name2));


  if(file1.GetSize() == file2.GetSize())
  {
    cout << "\033[31mUnequal file size for files:\033[0m" << endl;
    cout << "\033[31m  \"" << name1 << "\"\033[0m" << endl;
    cout << "\033[31m  \"" << name2 << "\"\033[0m" << endl;
    return 1;
  }

  TIter it1(file1.GetListOfKeys());
  TIter it2(file2.GetListOfKeys());


  while(TKey* key = (TKey*)it1.Next())
  {
    TObject* obj1 = key->ReadObj();
    if(!obj1)
    {
      cout << "\033[31mMissing object \"" << obj1->GetName() << " in file \"" << name1 << "\"\033[0m" << endl;
      return 1;
    }
    TObject* obj2 = file2.Get(obj1->GetName());
    if(!obj2)
    {
      cout << "\033[31mMissing object \"" << obj2->GetName() << " in file \"" << name2 << "\"\033[0m" << endl;
      return 1;
    }
    if(obj1->Compare(obj2)) return 1;
  }
  cout << "\033[32mFiles are identical!\033[0m" << endl;
  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 3) throw runtime_error("CompareRootFiles needs 2 arguments");
  return CompareRootFiles(argv[1], argv[2]);
}