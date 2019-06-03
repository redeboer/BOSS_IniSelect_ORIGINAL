#ifndef Physics_JpsiToDPV_ObjComparer_H
#define Physics_JpsiToDPV_ObjComparer_H

#include "TString.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <map>

#include "ObjComparer.h"
#include <TClass.h>
#include <TString.h>
#include <TObject.h>
#include <TCollection.h>
#include <TIterator.h>
#include <TRealData.h>
#include <TDataMember.h>
#include <TDataType.h>
#include <TList.h>
#include <Riostream.h>
#include <TVirtualCollectionProxy.h>
#include <TGenCollectionProxy.h>
#include <TInterpreter.h>
#include <TDirectory.h>
#include <TROOT.h>
#include <TBaseClass.h>

#include <utility>
#include <map>
#include <vector>
#include <cstdlib> // for atoi, atof
#include <string>
#include <memory>

class TObject;
class TClass;

// a service class for the purpose to cmp
// 2 instances of an arbitrary ROOT object
class ROOTObjComparer
{
public:
  // constructor
  ROOTObjComparer() :
    mVariableToStringTable(),
    mVariableToValueDiffTable(),
    mVariableToValueTable(),
    mVerbose(true),
    mDebug(false),
    mShowNonPersistent(false),
    mCompareBaseClassMembers(false),
    mPrintTypes(true),
    mResolveTObjectTypes(true),
    mBitsetAsString(false)
  {}

  // perform a diff on 2 objects
  bool Diff(TObject const* obj1, TObject const* obj2);

  // print an object
  void Print(TObject const* obj);

  // clear the internal states;
  void Clear();

  void SetVerbose(bool v) { mVerbose = v; }
  void SetDebug(bool v) { mDebug = v; }
  void SetCompareBaseClassMembers(bool v) { mCompareBaseClassMembers = v; }
  void SetBitSetAsString(bool v) { mBitsetAsString = v; }

private:
  // private helper data

  void EmitToTable(TString key, const char* valuestring, bool diffmode = false);
  void EmitToStringTable(TString key, const char* valuestring, bool diffmode = false);

  void EmitToTable(TString label, int type, void* data, bool diffmode = false);

  // function walking the object and adding tables
  void DumpObject(TClass*, void*, TString prefix, bool diffmode);

  std::map<std::string, double>      mVariableToValueTable;
  std::map<std::string, double>      mVariableToValueDiffTable;
  std::map<std::string, std::string> mVariableToStringTable;

  // integral types
  std::map<std::string, char>          mCharValues;
  std::map<std::string, unsigned char> mUCharValues;
  std::map<std::string, Short_t>       mShortValues;
  std::map<std::string, UShort_t>      mUShortValues;
  std::map<std::string, Int_t>         mIntValues;
  std::map<std::string, UInt_t>        mUIntValues;
  std::map<std::string, Long_t>        mLongValues;
  std::map<std::string, ULong_t>       mULongValues;
  std::map<std::string, Long64_t>      mLong64Values;
  std::map<std::string, ULong64_t>     mULong64Values;

  // floating point types
  std::map<std::string, Float_t>    mFloatValues;
  std::map<std::string, Double_t>   mDoubleValues;
  std::map<std::string, Double32_t> mDouble32Values; // might not be needed

  // bool type
  std::map<std::string, bool> mBoolValues;

  template <typename T>
  std::map<std::string, T>* GetMap()
  {
    return nullptr;
  }

  bool        mVerbose; // printing out parsed data?
  bool        mDebug;
  bool        mShowNonPersistent;
  bool        mCompareBaseClassMembers;
  bool        mPrintTypes;          // print type information about (intermediate) types
  bool        mResolveTObjectTypes; // whether to resolve the actual type of a TObject
  bool        mBitsetAsString;      // whether to represent a bitset as a string
  std::string mCurrentTypeName;

  template <typename T>
  bool IsSame(T x, T y)
  {
    return x == y;
  }

  void unsupp() { std::cerr << "currently unsupported\n"; }
  template <typename T>
  T GetValue(void* data)
  {
    return *((T*)data);
  }

  template <typename T>
  void insert(std::string key, void* valueptr, bool diffmode = false)
  {
    std::map<std::string, T>* mapptr = GetMap<T>();
    assert(mapptr != nullptr);
    // note the &: we do not want a copy but a reference
    std::map<std::string, T>& map   = *mapptr;
    T     value = GetValue<T>(valueptr);
    if(mVerbose) { std::cout << key << " : " << value << "\n"; }
    if(!diffmode)
    {
      // just inserting here; no key should occur twice
      map.insert(std::pair<std::string, T>(key, value));
    }
    else
    {
      if(map.find(key) != map.end())
      {
        T oldvalue = map[key];
        T newvalue = value;
        if(!IsSame(oldvalue, newvalue))
        {
          std::cout << "FOUND NUMERIC DIFFERENCE FOR KEY " << key << " ABSOLUTE "
                    << oldvalue - newvalue << "\n";
          //                  << " RELATIVE " << (oldvalue - newvalue)/oldvalue << "\n";
        }
        else
        {
          // std::cout << "MATCHES\n";
        }
      }
      else
      {
        std::cerr << "found elements " << key << " not present in table (first object)\n";
      }
    }
  }
};

// template specialization return correct map
template <>
std::map<std::string, char>* ROOTObjComparer::GetMap()
{
  return &mCharValues;
}

// template specialization return correct map
template <>
std::map<std::string, unsigned char>* ROOTObjComparer::GetMap()
{
  return &mUCharValues;
}
// template specialization return correct map
template <>
std::map<std::string, Short_t>* ROOTObjComparer::GetMap()
{
  return &mShortValues;
}
// template specialization return correct map
template <>
std::map<std::string, UShort_t>* ROOTObjComparer::GetMap()
{
  return &mUShortValues;
}
// template specialization return correct map
template <>
std::map<std::string, Int_t>* ROOTObjComparer::GetMap()
{
  return &mIntValues;
}
// template specialization return correct map
template <>
std::map<std::string, UInt_t>* ROOTObjComparer::GetMap()
{
  return &mUIntValues;
}
// template specialization return correct map
template <>
std::map<std::string, Long_t>* ROOTObjComparer::GetMap()
{
  return &mLongValues;
}
// template specialization return correct map
template <>
std::map<std::string, ULong_t>* ROOTObjComparer::GetMap()
{
  return &mULongValues;
}
// template specialization return correct map
template <>
std::map<std::string, Long64_t>* ROOTObjComparer::GetMap()
{
  return &mLong64Values;
}
// template specialization return correct map
template <>
std::map<std::string, ULong64_t>* ROOTObjComparer::GetMap()
{
  return &mULong64Values;
}
// template specialization return correct map
template <>
std::map<std::string, Float_t>* ROOTObjComparer::GetMap()
{
  return &mFloatValues;
}
// template specialization return correct map
template <>
std::map<std::string, Double_t>* ROOTObjComparer::GetMap()
{
  return &mDoubleValues;
}

// template specialization return correct map
template <>
std::map<std::string, bool>* ROOTObjComparer::GetMap()
{
  return &mBoolValues;
}

// template specialization return correct map
//    template<>
//    std::map<std::string, Double32_t> *ROOTObjComparer::GetMap() {
//      return &mDouble32Values;
//    }

static const double Delta = 1.E-9;
template <>
bool ROOTObjComparer::IsSame<double>(double x, double y)
{
  return std::abs(x - y) < Delta;
}

template <>
bool ROOTObjComparer::IsSame<std::string>(std::string x, std::string y)
{
  return x.compare(y) == 0;
}



// build list of all base classes of a class cl
void BuildListOfAllBases(TClass *cl, TList *list) {
  if (!cl) {
    return;
  }
  TList *bases = cl->GetListOfBases();
  if (bases) {
    for (int i = 0; i < bases->GetEntries(); ++i) {
      TClass *b = ((TBaseClass *)bases->At(i))->GetClassPointer();
      list->AddLast(b);
      BuildListOfAllBases(b, list);
    }
  }
}



// helper function to get TClass
TClass *GetClass(const char *name){
  TObject *obj3 = TClass::GetClass(name);
  return (TClass*)obj3;
}

void ROOTObjComparer::Clear() {
  mVariableToStringTable.clear();
  mVariableToValueTable.clear();
  mVariableToValueDiffTable.clear();
}

void ROOTObjComparer::Print(const TObject *obj) {
  // we print using the verbose parsing mode
  // backup the verbose mode
  bool backupVerbose = mVerbose;
  SetVerbose(true);
  DumpObject(obj->Class(), (void*)obj, obj->ClassName(), false);
  SetVerbose(backupVerbose);
}

bool ROOTObjComparer::Diff(const TObject *obj1, const TObject *obj2) {
  DumpObject(obj1->Class(), (void*)obj1, obj1->ClassName(), false);
  DumpObject(obj2->Class(), (void*)obj2, obj2->ClassName(), true);
  return true;
}

void ROOTObjComparer::DumpObject(TClass *cl, void *obj, TString prefix, bool diffmode)
{
  if (!cl) {
    std::cout << " Null dictionary encountered; cannot analyse this class \n";
    return;
  }
  if (!obj){
    if(mDebug) std::cout << " Null object encountered; no data to analyse \n";
    return;
  }

  if(mDebug){
    std::cout << "## treating class " << cl->GetName() << "\n";
  }

  if(cl == TClass::Class()){
    // not interested in TClass stuff
    if(mDebug){
      std::cout << " exiting for class " << cl->GetName() << "\n";
    }
    return;
  }

  TVirtualCollectionProxy *collectionproxy = cl->GetCollectionProxy();
  // treat std::bitset (needs some special attention)
  if (collectionproxy && collectionproxy->GetCollectionType() == 8) {
    if (mDebug) {
      std::cout << "Treating stl bitset\n";
    }
    TVirtualCollectionProxy *collp = cl->GetCollectionProxy()->Generate();
    if (collp) {
      if (collp->GetProperties() & TVirtualCollectionProxy::kIsEmulated) {
        std::cerr << "bitset proxy is emulated (this is an error); returning here ...\n";
        return;
      }

      // create second object for this class
      // which is a hack (communicated by the ROOT team) to be able to read out the bits
      void *objcopy = cl->New();
      collp->PushProxy(objcopy);

      if (mBitsetAsString){
        char bitstring[collp->Size()];
        for (int i=0 ;i < collp->Size(); ++i){
          TVirtualCollectionProxy::TPushPop helper2(collp,obj);
          bool bit = *((bool*)collp->At(i));
          bitstring[i] = bit ? '1' : '0';
      }
        EmitToStringTable(prefix, &bitstring[0], diffmode);
      }
      else {
      // we might be able to speed this up by directly returning an integer or string encoding of whole bitset
      for (int i=0 ;i < collp->Size(); ++i){
        TVirtualCollectionProxy::TPushPop helper2(collp,obj);
        bool bit = *((bool*)collp->At(i));
        TString prefixArr = TString::Format("%s[%d]",prefix.Data(),i);
        EmitToTable(prefixArr, 18, (void*)&bit, diffmode);
      }}
      if (objcopy) cl->Destructor(objcopy);
      return;
    }
    delete collp;
  }

  // treat other stl classes
  if (!cl->InheritsFrom(TCollection::Class()) && collectionproxy) {
    if(mDebug){
      std::cout << "Treating stl proxy\n";
    }
    collectionproxy->PushProxy(obj);
    if(mDebug){
      std::cout << " ContainerSize " << collectionproxy->Size() << "\n";
    }

    TClass * elementclass = collectionproxy->GetValueClass();
    // gives an enumeration signifying a concrete pod type
    EDataType elementaltype = collectionproxy->GetType();
    // if this is zero we have a class type ... otherwise an elemental type
    for (UInt_t i=0; i < collectionproxy->Size(); ++i) {
      TString prefixArr = TString::Format("%s[%d]",prefix.Data(),i);
      if (!elementaltype){
        DumpObject(elementclass, collectionproxy->At(i), prefixArr, diffmode);
      }
      else {
        EmitToTable(prefixArr, elementaltype, collectionproxy->At(i), diffmode);
      }
    }
    return;
  }

  // treat special case of ROOT collection classes
  // In this case, the basic elements are all of type TObject*
  if (cl->InheritsFrom(TCollection::Class())) {
    if(mDebug){
      std::cout << "## treating ROOT collection " << cl->GetName() << "\n";
    }
    TIter myiter((TCollection*)obj);
    TObject *arObject=0;
    Int_t counter=0;
    while ((arObject = (TObject*)myiter.Next())) {
      TString prefixArr = TString::Format("%s[%d]",prefix.Data(),counter);
      TClass *nextcl = GetClass(arObject->ClassName());
      DumpObject(nextcl,(void*)arObject,prefixArr, diffmode);
      counter++;
    }
    return;
  }

  // special case of TString
  if (cl==TString::Class())
  {
    EmitToStringTable(prefix, ((TString*)obj)->Data(), diffmode);
    return;
  }

  // special case of std::string
  if (cl==GetClass("string")) {
    EmitToStringTable(prefix, ((std::string*)obj)->c_str(),diffmode);
    return;
  }

  if (cl==TObject::Class()){
    TClass *nextcl = GetClass(((TObject*)obj)->ClassName());
    DumpObject(nextcl, obj, prefix, diffmode);
    return;
  }

  // iterate over PERSISTENT members of this class
  // TODO: we need to fetch data from mother classes

  // build class list to iterate over
  TList *classparts;
  classparts = new TList();
  BuildListOfAllBases(cl, classparts);
  classparts->AddFirst(cl);

  TClass *currclass;
  TIter classiter(classparts);
  while ((currclass = (TClass *)classiter())) {
    if (mDebug) {
      std::cout << "teating class part" << currclass->GetName() << "\n";
    }

    TIter next(currclass->GetListOfDataMembers());
    TDataMember *dm;
    while ((dm = (TDataMember *)next())) {
      if (!dm->IsPersistent() && !mShowNonPersistent)
        continue;

      // filter out static members for now
      // FIXME: add configurable treatment
      if (dm->Property() & kIsStatic) {
        continue;
      }

      if (dm->IsEnum()) {
        // FIXME: add treatment for enums
        continue;
      }

      TDataType *dtype = dm->GetDataType();
      if (mDebug) {
        std::cout << "--" << dm->GetName()
                  << " True Type name: " << dm->GetTrueTypeName() << "\t";
      }
      Int_t offset = dm->GetOffset();

      // this points to the data in objects represented by dtype;
      char *pointer = ((char *)obj) + offset;

      TString prefixNext =
          TString::Format("%s.%s", prefix.Data(), dm->GetName());
      if (mPrintTypes) {
        if (strcmp(dm->GetTrueTypeName(), "TObject*") == 0) {
          void **pointee = (void **)pointer;
          prefixNext +=
              "{" + TString(((TObject *)(*pointee))->ClassName()) + "}";
        } else {
          prefixNext += "{" + TString(dm->GetTrueTypeName()) + "}";
        }
      }

      // pointer data
      if (dm->IsaPointer()) {
        void **pointee = (void **)pointer;
        if (!pointee)
          continue;
        // special treatment as this could be an array of pods
        if (dm->IsBasic()) {
          if (dm->GetArrayIndex()) {
            // this seems to be a dynamic array of pods
            TDataMember *indexDataMember =
                currclass->GetDataMember(dm->GetArrayIndex());
            if (!indexDataMember) {
             // std::cerr << "null ptr encountered; ...\n";
              return;
            }

            Int_t indexOffset = indexDataMember->GetOffset();
            char *indexpointer = (char *)obj + indexOffset;
            // interpret this an int to fetch number of indices into memory
            Int_t size = (Int_t) * ((Int_t *)indexpointer);
            // std::cerr << " number of elements in this pointer " << size <<
            // "\n";

            // put this into a separate piece of code
            for (int i = 0; i < size; ++i) {
               EmitToTable(TString::Format("%s[%d]", prefixNext.Data(), i), dtype->GetType(),
                          (void*)((char *)*pointee + dtype->Size() * i), diffmode);
             //    EmitToTable(TString::Format("%s[%d]", prefixNext.Data(), i),
             //               dtype->AsString((char *)*pointee + dtype->Size() * i),
             //               diffmode);
            }
          } else {
            std::cerr
                << "Should not happen (basic pointer type without dimension)\n";
          }
        } else {
          // fetch the object this thing is pointing to and recurse
          // use "GetTypeName" which deletes pointer whereas "GetFullTypeName"
          // keeps it
          TClass *nextcl = GetClass(dm->GetTypeName());
          DumpObject(nextcl, (void *)*pointee, prefixNext, diffmode);
        }
      }

      // --  the only one doing actual output; finally all recursion land here
      // --
      else if (dm->IsBasic()) {
        int nDim = dm->GetArrayDim();
        // this is a scalar value
        if (nDim == 0) {
          EmitToTable(prefixNext, dtype->GetType(), (void*)pointer, diffmode);
         } else {
          // not a scalar value
          // TODO: find a general way to iterate over multidimensional arrays
          for (int i = 0; i < dm->GetMaxIndex(0); ++i) {
              EmitToTable(TString::Format("%s[%d]", prefixNext.Data(), i), dtype->GetType(),
                          (void*)(pointer + dtype->Size() * i), diffmode);
          }
        }
      } else {
        // filter out stl
        TClass *nextcl = GetClass(dm->GetTypeName());
        if (dm->IsSTLContainer()) {
          if (mDebug) {
            std::cout << " Found stl container " << dm->GetTrueTypeName()
                      << " nextcl " << nextcl << "\n";
          }
          DumpObject(nextcl, (void *)pointer, prefixNext, diffmode);
          continue;
        }
        if (mDebug) {
          std::cerr << "SOME UNHANDLED CASE HAPPENED for " << prefixNext.Data()
                    << "\n";
        }
        DumpObject(nextcl, (void *)pointer, prefixNext, diffmode);
      }
    }
  } // and iterator over class parts
}

// diffmode = true checks for existing keys and actually compares against
// the table
void ROOTObjComparer::EmitToTable(TString key, const char * valuestring, bool diffmode) {
  if(mVerbose) {
    std::cout << key.Data() << " : " << valuestring << "\n";
  }
  if (!diffmode){
    // just inserting here; no key should occur twice
    mVariableToValueTable.insert(std::pair<std::string, double>(std::string(key.Data()), atof(valuestring)));
  }
  else {
    if (mVariableToValueTable.find(std::string(key.Data()))!=mVariableToValueTable.end())
    {
       double oldvalue = mVariableToValueTable[std::string(key.Data())];
       double newvalue = atof(valuestring);
       if (!IsSame(oldvalue,newvalue)) {
         std::cout << "FOUND NUMERIC DIFFERENCE FOR KEY " << key << " ABSOLUTE " << oldvalue - newvalue
                   << " RELATIVE " << (oldvalue - newvalue)/oldvalue << "\n";
       }
       else {
        // std::cout << "MATCHES\n";
       }
    }
    else {
      std::cerr << "found elements not present in table (first object)\n";
    }
  }
}


// diffmode = true checks for existing keys and actually compares against
// the table
void ROOTObjComparer::EmitToStringTable(TString key, const char * valuestring, bool diffmode) {
  if(mVerbose) {
    std::cout << key.Data() << " : \"" << valuestring << "\"\n";
  }
  if (!diffmode){
    // just inserting here; no key should occur twice
    mVariableToStringTable.insert(std::pair<std::string, std::string>(std::string(key.Data()), valuestring));
  }
  else {
    if (mVariableToStringTable.find(std::string(key.Data()))!=mVariableToStringTable.end())
    {
      std::string oldvalue = mVariableToStringTable[std::string(key.Data())];
      std::string newvalue(valuestring);
      if (!IsSame(oldvalue,newvalue)) {
        std::cout << "FOUND STRING DIFFERENCE FOR KEY " << key << " " << oldvalue << " vs " << newvalue << "\n";
      }
      else {
        // std::cout << "MATCHES\n";
      }
    }
    else {
      std::cerr << "found elements not present in table (first object)\n";
    }
  }
}

// this function can be used to get access to the real values
// (not going via intermediate strings)
void ROOTObjComparer::EmitToTable(TString label, int type, void *data, bool diffmode) {
  switch (type) {
    case kChar_t: {
        insert<char>(std::string(label.Data()), data, diffmode);
      break;
    }
    case kUChar_t: {
      insert<unsigned char>(std::string(label.Data()), data, diffmode);
      break;
    }
    case kShort_t: {
      insert<short>(std::string(label.Data()), data, diffmode);
      break;
  }
  case kUShort_t: {
      insert<unsigned short>(std::string(label.Data()), data, diffmode);
      break;
  }
  case kInt_t: {
      insert<int>(std::string(label.Data()), data, diffmode);
      break;
  }
  case kUInt_t: {
      insert<unsigned int>(std::string(label.Data()), data, diffmode);
    break;
  }
  case kLong_t: {
      insert<long>(std::string(label.Data()), data, diffmode);
    break;
  }
  case kULong_t: {
      insert<unsigned long>(std::string(label.Data()), data, diffmode);
    break;
  }
  case kFloat_t: {
      insert<float>(std::string(label.Data()), data, diffmode);
    break;
  }
  case kDouble_t: {
      insert<double>(std::string(label.Data()), data, diffmode);
      break;
  }
  case kDouble32_t: {
      insert<double>(std::string(label.Data()), data, diffmode);
      break;
  }
  case kchar: {
    unsupp();
    break;
  }
  case kBool_t: {
    insert<bool>(std::string(label.Data()), data, diffmode);
    break;
  }
  case kLong64_t: {
    insert<long long>(std::string(label.Data()), data, diffmode);
    break;
  }
  case kULong64_t: {
    insert<unsigned long long>(std::string(label.Data()), data, diffmode);
    break;
  }
  case kOther_t: {
    unsupp();
    break;
  }
  case kNoType_t: {
    unsupp();
    break;
  }
  case kFloat16_t: {
    unsupp();
    break;
  }
  case kCounter: {
    unsupp();
    break;
  }
  case kCharStar: {
    insert<char *>(std::string(label.Data()), data, diffmode);
    break;
  }
  case kBits: {
    unsupp();
    break;
  }
  case kVoid_t: {
    unsupp();
    break;
  }
  case kDataTypeAliasUnsigned_t: {
    unsupp();
    break;
  }
    /*
  case kDataTypeAliasSignedChar_t: {
    unsupp();
    break;
  }
  case kNumDataTypes: {
    unsupp();
    break;
    }*/
  default: {
    unsupp();
    break;
  }
} // end switch
}



#endif