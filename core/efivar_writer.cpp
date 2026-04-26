#include "utf16.h"
#include "efivar_writer.h"
#include "efivar_reader.h"
#include "utils.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#define DESCRIPTION_OFFSET 10

constexpr const char *EFI_PATH = "/sys/firmware/efi/efivars/";
constexpr const char *EFI_GUID = "8be4df61-93ca-11d2-aa0d-00e098032b8c";

void EFIVarWriter::writeBootNext(const std::string &id)
{
  std::string path = std::string(EFI_PATH) + "BootNext-" + EFI_GUID;

  uint16_t value;
  std::stringstream ss;
  ss << std::hex << id;
  ss >> value;

  std::string chattr_cmd = "chattr -i " + path + " 2>/dev/null";
  system(chattr_cmd.c_str());

  std::vector<uint8_t> data(6);
  data[0] = 0x07;
  data[1] = 0x00;
  data[2] = 0x00;
  data[3] = 0x00;
  data[4] = value & 0xFF;        // Low byte
  data[5] = (value >> 8) & 0xFF; // High byte

  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  if (!file)
    throw std::runtime_error("Failed to write BootNext. Are you running as root?");

  file.write(reinterpret_cast<const char *>(data.data()), data.size());
  file.close();

  chattr_cmd = "chattr +i " + path + " 2>/dev/null";
  system(chattr_cmd.c_str());
}

void EFIVarWriter::renameBootEntry(const std::string &id, const std::string &newName)
{
  std::string path = std::string(EFI_PATH) + "Boot" + id + "-" + EFI_GUID;

  // turn off immutable flag
  std::string chattr_cmd = "chattr -i " + path + " 2>/dev/null";
  system(chattr_cmd.c_str());

  std::ifstream file(path, std::ios::binary);
  if (!file)
    throw std::runtime_error("Failed to load efi variable. Are you running as root?");

  std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();

  if (data.size() < DESCRIPTION_OFFSET)
    throw std::runtime_error("Invalid EFI variable format");

  std::vector<uint8_t> newData;
  newData.insert(newData.end(), data.begin(), data.begin() + DESCRIPTION_OFFSET);

  size_t offset = DESCRIPTION_OFFSET;
  bool found = false;

  while (offset + 1 < data.size())
  {
    if (data[offset] == 0x00 && data[offset + 1] == 0x00)
    {
      offset += 2;
      found = true;
      break;
    }
    offset += 2;
  }

  if (!found)
    throw std::runtime_error("Failed to find description terminator");

  std::u16string utf16name = utf8_to_utf16(newName);

  for (char16_t c : utf16name)
  {
    newData.push_back(c & 0xFF);
    newData.push_back((c >> 8) & 0xFF);
  }

  newData.push_back(0x00);
  newData.push_back(0x00);

  // append remaining things after description
  newData.insert(newData.end(), data.begin() + offset, data.end());

  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  if (!out)
    throw std::runtime_error("Failed to write efi variable");

  out.write(reinterpret_cast<const char *>(newData.data()), newData.size());
  out.close();

  // turn on immutable flag
  chattr_cmd = "chattr +i " + path + " 2>/dev/null";
  system(chattr_cmd.c_str());
  std::cout << "Success\n";
}

void EFIVarWriter::setBootOrderFirst(const std::string &id)
{
  EFIVarReader reader;
  auto order = reader.readBootOrder();
  if (std::find(order.begin(), order.end(), id) == order.end())
    throw std::runtime_error("Boot ID not found in BootOrder");
  order.erase(std::remove(order.begin(), order.end(), id), order.end());
  order.insert(order.begin(), id);

  std::string path = (std::string)EFI_PATH + "BootOrder" + "-" + EFI_GUID;
  Utils::flagDown(path);
  std::vector<uint8_t> data;

  data.push_back(0x07);
  data.push_back(0x00);
  data.push_back(0x00);
  data.push_back(0x00);

  for (const auto &entry : order)
  {
    uint16_t value = Utils::hexToU16(entry);
    data.push_back(value & 0xFF);
    data.push_back((value >> 8) & 0xFF);
  }

  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  if (!file)
    throw std::runtime_error("Failed to write bootorder. Run as root!");
  file.write(reinterpret_cast<const char *>(data.data()), data.size());
  file.close();
  Utils::flagUp(path);
}

void EFIVarWriter::deleteBootEntry(const std::string &id)
{
  // 1. Remove from boot order
  EFIVarReader reader;
  auto order = reader.readBootOrder();
  if (std::find(order.begin(), order.end(), id) == order.end())
    throw std::runtime_error("Boot ID not found in BootOrder");
  order.erase(std::remove(order.begin(), order.end(), id), order.end());

  std::string bootOrderPath = (std::string)EFI_PATH + "BootOrder" + "-" + EFI_GUID;
  Utils::flagDown(bootOrderPath);
  std::vector<uint8_t> data;

  data.push_back(0x07);
  data.push_back(0x00);
  data.push_back(0x00);
  data.push_back(0x00);

  for (const auto &entry : order)
  {
    uint16_t value = Utils::hexToU16(entry);
    data.push_back(value & 0xFF);
    data.push_back((value >> 8) & 0xFF);
  }

  std::ofstream file(bootOrderPath, std::ios::binary | std::ios::trunc);
  if (!file)
    throw std::runtime_error("Failed to write bootorder. Run as root!");
  file.write(reinterpret_cast<const char *>(data.data()), data.size());
  file.close();
  Utils::flagUp(bootOrderPath);

  // 2. Delete the boot entry variable
  std::string bootEntryPath = std::string(EFI_PATH) + "Boot" + id + "-" + EFI_GUID;
  Utils::flagDown(bootEntryPath);
  if (!std::filesystem::remove(bootEntryPath))
  {
    Utils::flagUp(bootEntryPath);
    throw std::runtime_error("Failed to delete boot entry file: " + bootEntryPath);
  }
  std::cout << "Successfully deleted boot entry " << id << std::endl;
}
