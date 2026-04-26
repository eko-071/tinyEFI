#pragma once
#include <string>

class EFIVarWriter
{
public:
  static void renameBootEntry(const std::string &id, const std::string &newname);
  static void writeBootNext(const std::string &id);
  static void setBootOrderFirst(const std::string &id);
  static void deleteBootEntry(const std::string &id);
};
