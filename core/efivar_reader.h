#pragma once
#include <string>
#include <vector>

class EFIVarReader
{
public:
  static std::vector<std::string> readBootOrder();
  static std::string readBootCurrent();
  static std::string readBootDescription(const std::string &id);
  static void writeBootNext(const std::string &id);
};
