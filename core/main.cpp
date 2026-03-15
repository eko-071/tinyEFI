#include "efivar_reader.h"
#include "efivar_writer.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

void printUsage()
{
  std::cout << "\nUsage:\n";
  std::cout << "  tinyefi --list            List EFI boot entries\n";
  std::cout << "  tinyefi --current         Show current EFI boot entry\n";
  std::cout << "  tinyefi --next <BootID>   Set boot entry for next boot only\n\n";
}

int listCmd()
{
  try
  {
    EFIVarReader reader;
    std::string current = reader.readBootCurrent();
    std::vector<std::string> order = reader.readBootOrder();
    std::cout << "\n=== Boot Entries ===\n\n";
    std::cout << "Current Boot : " << current << "\n\n";
    std::cout << std::left << std::setw(8) << "ID" << std::setw(10) << "Active" << "Description\n";
    std::cout << "-------------------------------------------------\n";
    for (const auto &id : order)
    {
      std::string desc = reader.readBootDescription(id);
      std::cout << std::left << std::setw(8) << id << std::setw(10) << (id == current ? "*" : "") << desc << "\n";
    }
    std::cout << "\nTotal Entries: " << order.size() << "\n";
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}

int currentCmd()
{
  try
  {
    EFIVarReader reader;
    std::string current = reader.readBootCurrent();
    std::vector<std::string> order = reader.readBootOrder();
    std::string description = "Unknown";
    for (const auto &id : order)
    {
      if (id == current)
      {
        description = reader.readBootDescription(id);
        break;
      }
    }
    std::cout << "Current EFI Boot Entry => " << current << ", " << description << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

int nextCmd(const std::string &id)
{
  try
  {
    // Validate the ID exists in boot order
    EFIVarReader reader;
    std::vector<std::string> order = reader.readBootOrder();
    bool found = false;
    for (const auto &entry : order)
    {
      if (entry == id)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      std::cerr << "Error: Boot entry " << id << " not found in boot order\n";
      return 1;
    }

    EFIVarWriter::writeBootNext(id);
    std::string desc = reader.readBootDescription(id);
    std::cout << "BootNext set to " << id << " (" << desc << ")\n";
    std::cout << "This entry will be used for the next boot only.\n";
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}

int renameCmd(const std::string &id, const std::string &newName)
{
  EFIVarReader reader;
  std::vector<std::string> order = reader.readBootOrder();
  bool found = false;
  for (const auto &entry : order)
  {
    if (entry == id)
    {
      found = true;
      break;
    }
  }
  if (!found)
  {
    std::cerr << "Error: Boot entry " << id << " not found in boot order\n";
    return 1;
  }
  EFIVarWriter::renameBootEntry(id, newName);
  return 0;
}

int setCmd(const std::string &id)
{
  try
  {
    EFIVarReader reader;
    std::vector<std::string> order = reader.readBootOrder();
    bool found = false;
    for (const auto &entry : order)
    {
      if (entry == id)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      std::cerr << "Error: Boot entry " << id << " not found in boot order\n";
      return 1;
    }
    EFIVarWriter::setBootOrderFirst(id);
    std::string desc = reader.readBootDescription(id);
    std::cout << "Boot entry " << id << " (" << desc << ") " << "is now the default boot entry.\n";
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printUsage();
    return 1;
  }
  std::string command = argv[1];
  if (command == "--list")
    return listCmd();
  else if (command == "--current")
    return currentCmd();
  else if (command == "--next")
  {
    if (argc < 3)
    {
      std::cerr << "Error: --next requires a boot entry ID\n";
      return 1;
    }
    return nextCmd(argv[2]);
  }
  else if (command == "--rename")
  {
    if (argc < 3)
    {
      std::cerr << "Error: --rename requires a boot entry ID\n";
      return 1;
    }
    else if (argc < 4)
    {
      std::cerr << "Error: --rename requires new description\n";
      return 1;
    }
    return renameCmd(argv[2], argv[3]);
  }
  else if (command == "--set")
  {
    if (argc < 3)
    {
      std::cerr << "Error: --set requires a boot entry ID\n";
      return 1;
    }
    return setCmd(argv[2]);
  }
  else
  {
    printUsage();
    return 1;
  }
}
