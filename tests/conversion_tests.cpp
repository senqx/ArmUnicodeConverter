#include "ConvertFunctions.hpp"
#include "SymbolsHashTable.hpp"
#include <stdexcept>
#include <iostream>
std::string bytes(std::uint32_t value) {
  std::string result;
  do {result.insert(result.begin(), static_cast<char>(value & 255));value >>= 8;} while(value);
  return result;
}
void expect(bool condition) {if(!condition)throw std::runtime_error("Conversion mismatch");}
int main() {
  for(const auto& entry : TimesToUniSymbols)
    expect(TimesToUnicode(bytes(entry.first)) == bytes(entry.second));
  for(const auto& entry : UniToTimesSymbols)
    expect(UnicodeToTimes(bytes(entry.first)) == bytes(entry.second));
  for(const std::string& text : {std::string("ASCII 123\n"), std::string(u8"你好 🙂"), std::string("\xE2\x82"), std::string("\xFF",1)}) {
    expect(TimesToUnicode(text)==text);expect(UnicodeToTimes(text)==text);
  }
  expect(UnicodeToTimes(u8"ԱԲԳ") == bytes(0xC2B2)+bytes(0xC2B4)+bytes(0xC2B6));
  expect(TimesToUnicode(bytes(0xC2AE)) == u8"…");
  std::cout << "All mapping entries, mixed Unicode, ASCII and truncated sequences passed\n";
}
