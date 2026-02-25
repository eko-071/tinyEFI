#include "efivar_reader.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

// bruhh all this code is just for pretty printing...

int main(int argc, char *argv[])
{
  std::cout << "tinyEFI\n";
  if (argc > 1 && std::string(argv[1]) == "--list")
  {
    try
    {
      EFIVarReader reader;
      std::string current = reader.readBootCurrent();
      std::vector<std::string> order = reader.readBootOrder();

      std::cout << "\n=== Boot Entries ===\n\n";
      std::cout << "Current Boot : " << current << "\n\n";
      std::cout << std::left
                << std::setw(8) << "ID"
                << std::setw(10) << "Active"
                << "Description\n";

      std::cout << "-------------------------------------------------\n";
      for (const auto &id : order)
      {
        std::string desc = reader.readBootDescription(id);

        std::cout << std::left
                  << std::setw(8) << id
                  << std::setw(10) << (id == current ? "*" : "")
                  << desc
                  << "\n";
      }
      std::cout << "\nTotal Entries: " << order.size() << "\n";
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }
  else
  {
    std::cout << "\nUsage:\n";
    std::cout << "  tinyefi --list    List EFI boot entries\n";
  }

  return 0;
}
