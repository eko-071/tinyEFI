#include "efi_manager.h"
#include "efivar_reader.h"

BootData EFIManager::getBootData()
{

  BootData data;

  data.order = EFIVarReader::readBootOrder();
  data.current = EFIVarReader::readBootCurrent();

  for (const auto &id : data.order)
  {
    BootEntry entry;
    entry.id = id;
    entry.name = EFIVarReader::readBootDescription(id);
    data.EntryTable.push_back(entry);
  }

  return data;
}
