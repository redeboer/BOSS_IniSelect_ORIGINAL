#include "IniSelect/Handlers/NTupleBooker.h"
#include <iostream>
using namespace std;

void NTupleBooker::BookAll(const char* filename)
{
  cout << "Booking " << NTupleContainer::instances.size() << " NTuples:" << endmsg;
  map<string, NTupleContainer*>::iterator it = NTupleContainer::instances.begin();
  for(it; it != NTupleContainer::instances.end(); ++it) Book(*it->second, filename);
}

void NTupleBooker::Book(NTupleContainer& tuple, const char* filename)
{
  if(!tuple.DoWrite())
  {
    cout << "  NTuple \"" << tuple.Name() << "\" will not be written" << endl;
    return;
  }
  const char* bookName = Form("%s/%s", filename, tuple.Name().c_str());
  NTuplePtr   nt(fNTupleSvc, bookName);
  if(nt)
    cout << "  loaded NTuple \"" << tuple.Name() << "\" from " << filename << endl;
  else
  {
    cout << "  booked NTuple \"" << tuple.Name() << "\"" << endl;
    nt = fNTupleSvc->book(bookName, CLID_ColumnWiseTuple, tuple.Description());
    if(!nt)
      cout << "  --> cannot book N-tuple: " << long(nt) << " (\"" << tuple.Name() << "\")" << endl;
  }
  tuple.SetTuplePtr(nt);
}