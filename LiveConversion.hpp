#ifndef ARM_LIVE_CONVERSION_HPP
#define ARM_LIVE_CONVERSION_HPP
#include "ConvertFunctions.hpp"
#include <algorithm>
#include <utility>

// Each UI tick processes at most 32 KiB, ending at a UTF-8 sequence boundary.
class LiveConversion {
  std::string input;
  std::string output;
  std::size_t offset = 0;
  bool toUnicode = true;
public:
  void start(std::string text, bool direction) {
    input = std::move(text); output.clear(); output.reserve(input.size());
    offset = 0; toUnicode = direction;
  }
  bool step() {
    auto end = std::min(input.size(), offset + 32768);
    while (end < input.size() && end > offset &&
           (static_cast<unsigned char>(input[end]) & 0xC0) == 0x80) --end;
    // Defensive progress for malformed input containing long continuation runs.
    if (end == offset) end = std::min(input.size(), offset + 32768);
    const auto chunk = input.substr(offset, end - offset);
    output += toUnicode ? TimesToUnicode(chunk) : UnicodeToTimes(chunk);
    offset = end;
    return offset == input.size();
  }
  const std::string& result() const { return output; }
};
#endif
