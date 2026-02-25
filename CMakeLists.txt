cmake_minimum_required(VERSION 3.20)
project(tinyefi)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tinyefi
    core/main.cpp
    core/efi_manager.cpp
    core/efivar_reader.cpp
    core/utf16.cpp
)
