#include "utf16.h"
#include <codecvt>
#include <locale>

std::string utf16_to_utf8(const std::u16string &input)
{
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  return convert.to_bytes(input);
}
