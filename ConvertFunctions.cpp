#include "ConvertFunctions.hpp"
#include "SymbolsHashTable.hpp"

namespace {
std::size_t sequenceLength(const std::string& text, std::size_t start) {
  const auto first = static_cast<unsigned char>(text[start]);
  const std::size_t length = first < 0x80 ? 1 : first >= 0xC2 && first <= 0xDF ? 2
      : first >= 0xE0 && first <= 0xEF ? 3 : first >= 0xF0 && first <= 0xF4 ? 4 : 1;
  if (start + length > text.size()) return 1;
  for (std::size_t i = 1; i < length; ++i)
    if ((static_cast<unsigned char>(text[start + i]) & 0xC0) != 0x80) return 1;
  return length;
}
std::string packedBytes(std::uint32_t value) {
  std::string bytes;
  do { bytes.insert(bytes.begin(), static_cast<char>(value & 0xFF)); value >>= 8; } while (value);
  return bytes;
}
template<class Table>
std::string convert(const std::string& text, const Table& mapping) {
  std::string result;
  result.reserve(text.size());
  for (std::size_t start = 0; start < text.size();) {
    const auto length = sequenceLength(text, start);
    std::uint32_t packed = 0;
    for (std::size_t i = 0; i < length; ++i)
      packed = (packed << 8) | static_cast<unsigned char>(text[start + i]);
    const auto found = mapping.find(packed);
    if (found == mapping.end()) result.append(text, start, length);
    else result += packedBytes(found->second);
    start += length;
  }
  return result;
}
}
std::string UnicodeToTimes(const std::string& text) { return convert(text, UniToTimesSymbols); }
std::string TimesToUnicode(const std::string& text) { return convert(text, TimesToUniSymbols); }
