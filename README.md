# tinyEFI

A minimal UEFI boot order manager with a Python GUI that reads existing EFI boot entries, displays the current boot order, allows users to reorder them visually, and writes the updated order back to firmware for controlling system startup behavior.

FOR DEVELOPMENT PURPOSES

```bash
mkdir build
cd build
cmake ..
make
```

Usage for listing out entries

```bash
sudo ./tinyEFI --list
```
