#ifndef Physics_JpsiToDPV_ObjComparer_H
#define Physics_JpsiToDPV_ObjComparer_H

#include "TString.h"
#include <cassert>
#include <iostream>
#include <map>

class TObject;
class TClass;

// a service class for the purpose to cmp
// 2 instances of an arbitrary ROOT object
class ROOTObjComparer
{
public:
  // constructor
  ROOTObjComparer() : mVariableToStringTable(), mVariableToValueDiffTable(), mVariableToValueTable()
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

  bool        mVerbose                 = true; // printing out parsed data?
  bool        mDebug                   = false;
  bool        mShowNonPersistent       = false;
  bool        mCompareBaseClassMembers = false;
  bool        mPrintTypes              = true;  // print type information about (intermediate) types
  bool        mResolveTObjectTypes     = true;  // whether to resolve the actual type of a TObject
  bool        mBitsetAsString          = false; // whether to represent a bitset as a string
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
    auto mapptr = GetMap<T>();
    assert(mapptr != nullptr);
    // note the &: we do not want a copy but a reference
    auto& map   = *mapptr;
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

static constexpr double Delta = 1.E-9;
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
