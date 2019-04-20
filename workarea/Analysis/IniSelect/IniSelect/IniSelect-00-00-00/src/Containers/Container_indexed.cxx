#include "IniSelect/Containers/Container_indexed.h"

Container_indexed::Container_indexed(const std::string& name, const std::string& description) :
  Container_base(name, description)
{
  Insert(name);
}

Container_indexed::~Container_indexed()
{
  fInstances.erase(Name());
}

std::map<std::string, Container_indexed*> Container_indexed::fInstances;

void Container_indexed::SetName(const std::string& name)
{
  Insert(name);
  Container_base::SetName(name);
}

void Container_indexed::SetName(const std::string& name, const std::string& desc)
{
  Container_indexed::SetName(name);
  Container_base::SetDescription(name);
}

bool Container_indexed::KeyExists(const std::string& key) const
{
  return KeyExists(fInstances, key);
}

void Container_indexed::Insert(const std::string& key)
{
  Insert(fInstances, key, this);
}