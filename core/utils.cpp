#include "utils.h"
#include <sstream>
#include <cstdint>
#include <string>

uint16_t Utils::hexToU16(const std::string &hex)
{
  uint16_t value;
  std::stringstream ss;
  ss << std::hex << hex;
  ss >> value;
  return value;
}

void Utils::flagDown(const std::string path)
{
  std::string chattr_cmd = "chattr -i " + path + " 2>/dev/null";
  system(chattr_cmd.c_str());
}
void Utils::flagUp(const std::string path)
{
  std::string chattr_cmd = "chattr +i " + path + " 2>/dev/null";
  system(chattr_cmd.c_str());
}
