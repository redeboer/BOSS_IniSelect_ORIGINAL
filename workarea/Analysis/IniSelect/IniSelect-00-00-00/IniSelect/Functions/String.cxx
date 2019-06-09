#include "IniSelect/Functions/String.h"

TString IniSelect::String::BaseName(TString path)
{
  Ssiz_t pos = path.Last('/');
  if(pos >= 0) path.Remove(0, pos + 1);
  return path;
}

TString IniSelect::String::NoMainDir(TString path)
{
  Ssiz_t pos = path.First('/');
  if(pos >= 0) path.Remove(0, pos + 1);
  return path;
}

TString IniSelect::String::NoExtension(TString path)
{
  Ssiz_t pos = path.Last('.');
  if(pos >= 0) path.Resize(pos);
  return path;
}