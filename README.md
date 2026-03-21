# tinyEFI

A minimal UEFI boot order manager with a Python GUI that reads existing EFI boot entries, displays the current boot order, allows users to reorder them visually, and writes the updated order back to firmware for controlling system startup behavior.

FOR DEVELOPMENT PURPOSES

```bash
mkdir build
cd build
cmake ..
make
```

Usage  

Listing out entries

```bash
sudo ./tinyEFI --list
```

Set a boot entry just for next boot

```bash
sudo ./tinyefi --next <BOOTENTRYID>
```

Shows the current boot entry 

```bash
sudo ./tinyefi --current
```

Change the description of a boot entry

```bash
sudo ./tinyefi --rename <BOOTENTRYID> <DESCRIPTION>
```

Bring a boot entry to the top

```bash
sudo ./tinyefi --set <BOOTENTRYID>
```
