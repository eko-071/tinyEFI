#pragma once // header only included for one time
#include <string>
#include <vector>

struct BootEntry
{
  std::string id;
  std::string name;
};

struct BootData
{
  std::string current;
  std::vector<std::string> order;
  std::vector<BootEntry> EntryTable;
};

class EFIManager // getBootData => public API
{
public:
  BootData getBootData();
};
