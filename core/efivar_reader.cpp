#include "efivar_reader.h"
#include "utf16.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <stdexcept>

constexpr const char *EFI_PATH = "/sys/firmware/efi/efivars/";
constexpr const char *EFI_GUID = "8be4df61-93ca-11d2-aa0d-00e098032b8c";

static std::vector<uint8_t> readBinary(const std::string &path)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
    throw std::runtime_error("Failed to open " + path);
  return {std::istreambuf_iterator<char>(file), {}};
}

static std::string makePath(const std::string &name)
{
  return std::string(EFI_PATH) + name + "-" + EFI_GUID;
}

static uint16_t readU16(const std::vector<uint8_t> &data, size_t offset)
{
  return data[offset] | (data[offset + 1] << 8);
}

static std::string toHex4(uint16_t value)
{
  std::stringstream ss;
  ss << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << value;
  return ss.str();
}

std::vector<std::string> EFIVarReader::readBootOrder()
{
  auto data = readBinary(makePath("BootOrder"));
  std::vector<std::string> result;
  for (size_t i = 4; i + 1 < data.size(); i += 2)
  {
    uint16_t value = readU16(data, i);
    result.push_back(toHex4(value));
  }
  return result;
}

std::string EFIVarReader::readBootCurrent()
{
  auto data = readBinary(makePath("BootCurrent"));
  if (data.size() < 6)
    throw std::runtime_error("Invalid BootCurrent");
  uint16_t value = readU16(data, 4);
  return toHex4(value);
}

std::string EFIVarReader::readBootDescription(const std::string &id)
{
  auto data = readBinary(makePath("Boot" + id));
  size_t offset = 10;
  std::u16string u16;
  while (offset + 1 < data.size())
  {
    char16_t c = readU16(data, offset);
    offset += 2;
    if (c == u'\0')
      break;
    u16 += c;
  }
  return utf16_to_utf8(u16);
}
