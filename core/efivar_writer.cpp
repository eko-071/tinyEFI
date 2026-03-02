#include "utf16.h"
#include <vector>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include "efivar_writer.h"

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
