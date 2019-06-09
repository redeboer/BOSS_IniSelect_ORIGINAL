#include "IniSelect/Exceptions/Exception.h"
#include <iostream>
using namespace std;

void Exception::Print() const
{
  cout << "EXCEPTION: " << fMessage << endl;
}