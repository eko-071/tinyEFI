#pragma once
#include <string>
#include <cstdint>

class Utils
{
public:
  static uint16_t hexToU16(const std::string &hex);
  static void flagDown(const std::string path);
  static void flagUp(const std::string path);
};
