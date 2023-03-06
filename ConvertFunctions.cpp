#include "ConvertFunctions.hpp"

std::string UnicodeToTimes(const std::string &str) {
	std::string res;

	for(size_t i = 0; i < str.size(); ++i) {
		if(str[i] < 0) {
			res.push_back(str[i]);	

			// How many bytes to read (+1)
			unsigned int count = 0;
			unsigned char bits = str[i];
			while(bits & (unsigned char)128) {
				++count;
				bits <<= 1;
			}

			// If surely not Arm letter, just add as it is
			if(count > 4) {
				while(--count && (i < str.size())) {
					res.push_back(str[++i]);
				}
				continue;
			}

			unsigned int count_reserve = count;
			unsigned int tmp = 0;
			tmp |= (unsigned char)str[i];
			++i;

			// Read and add to res
			while(--count && (i < str.size())) {
				res.push_back(str[i]);
				tmp <<= 8;
				tmp |= (unsigned char)str[i];
				++i;
			}

			// If it was Arm Unicode char
			// Delete what we added
			// And replace it with Times char
			auto l = UniToTimesSymbols.find(tmp);
			if(l != UniToTimesSymbols.end()) {
				while(--count_reserve) {
					res.pop_back();
				}
				res.pop_back(); // And the bits
				res += char(l->second / 256);
				res += char(l->second % 256);
			}

			--i;
		} else {
			res += str[i];
		}
	}

	return res;
}

std::string TimesToUnicode(const std::string &str) {
	std::string res;

	for(size_t i = 0; i < str.size(); ++i) {
		if(str[i] < 0) {

			// It shouldn't happen, as high bit is 1
			// but let's keep it safe anyway
			if (str[i + 1] == '\0') {
				break;
			}

			// Get symbol from 2 chars
			const u_int16_t symb = (str[i] << 8) | (unsigned char)(str[i + 1]);
			++i;

			auto l = TimesToUniSymbols.find(symb);
			if (l != TimesToUniSymbols.end()) {
				// If it is that one and lonely
				// symbol with 3 byte representation
				if (symb == 0xC2AE) {
					res += "\xe2\x80\xa6";
				}
				else {
					res += char(l->second / 256);
					res += char(l->second % 256);
				}
			}
			else {
				res += char(symb / 256);
				res += char(symb % 256);
			}
		}
		else {
			res += str[i];
		}
	}
	return res;
}

