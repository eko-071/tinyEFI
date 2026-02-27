#include "efivar_reader.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

void printUsage()
{
  std::cout << "\nUsage:\n";
  std::cout << "  tinyefi --list      List EFI boot entries" << "\n";
  std::cout << "  tinyefi --current   Show current EFI boot entry\n\n";
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
  else
  {
    printUsage();
    return 1;
  }
}
