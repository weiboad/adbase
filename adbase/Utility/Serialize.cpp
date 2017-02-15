#include <adbase/Utility.hpp>

namespace adbase {
#define ENCODEBLOCKSIZE 7
#define DECODEBLOCKSIZE 4
// {{{ unsigned char toHex()

unsigned char toHex(unsigned char x) {
	int y = static_cast<int>(x);
    return  static_cast<unsigned char>(y > 9 ? y + 55 : y + 48);
}

// }}}
// {{{ unsigned char fromHex()

unsigned char fromHex(unsigned char x) {
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
    unsigned char y = 0;
    if (x >= 'A' && x <= 'Z') {
        y = x - 'A' + 10;
    } else if (x >= 'a' && x <= 'z') {
        y = x - 'a' + 10;
    } else if (x >= '0' && x <= '9') {
        y = x - '0';
    }
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wold-style-cast"
    return y;
}

// }}}
// {{{ void hexStringToBytes()

void hexStringToBytes(const std::string& str, char* buffer) {
#pragma GCC diagnostic ignored "-Wconversion"
	size_t length = str.length();
	for (size_t i=0 ; i < length; i += 2) {
		*(buffer + i / 2) = ((fromHex(str.at(i)) << 4) | fromHex(str.at(i + 1)));
	}
#pragma GCC diagnostic warning "-Wconversion"
}

// }}}
// {{{ std::string bytesToHexString()

std::string bytesToHexString(const char* buffer, size_t length) {
	std::string result = "";
	std::string keys = "0123456789abcdef";
	for (int i = 0; i < static_cast<int>(length); i++) {
		int b;
		b = 0x0f & (buffer[i] >> 4);
		result.append(1, keys.at(b));    
		b = 0x0f & buffer[i];
		result.append(1, keys.at(b));
	}
	return result;
}

// }}}
// {{{ std::string urlEncode()

std::string urlEncode(const std::string& str) {
#pragma GCC diagnostic ignored "-Wconversion"
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (isalnum(static_cast<unsigned char>(str[i])) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~')) {
            strTemp += str[i];
        } else if (str[i] == ' ') {
            strTemp += "+";
        } else {
            strTemp += '%';
            strTemp += toHex(static_cast<unsigned char>(str[i]) >> 4);
            strTemp += toHex(static_cast<unsigned char>(str[i]) % 16);
        }
    }
#pragma GCC diagnostic warning "-Wconversion"
    return strTemp;
}

// }}}
// {{{ std::string urlDecode()

std::string urlDecode(const std::string& str) {
#pragma GCC diagnostic ignored "-Wconversion"
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') {
            strTemp += ' ';
        } else if (str[i] == '%') {
            unsigned char high = fromHex(static_cast<unsigned char>(str[++i]));
            unsigned char low = fromHex(static_cast<unsigned char>(str[++i]));
            strTemp += high*16 + low;
        } else {
            strTemp += str[i];
        }
    }
#pragma GCC diagnostic warning "-Wconversion"
    return strTemp;
}

// }}}
// {{{ std::string base64Encode()

std::string base64Encode(const char* data, size_t len) {
#pragma GCC diagnostic ignored "-Wconversion"
    //编码表
    const char encodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    //返回值
    std::string strEncode;
    unsigned char tmp[4]={0};
    int lineLength = 0;
    for(size_t i = 0; i < (len / 3); i++) {
        tmp[1] = *data++;
        tmp[2] = *data++;
        tmp[3] = *data++;
        strEncode += encodeTable[tmp[1] >> 2];
        strEncode += encodeTable[((tmp[1] << 4) | (tmp[2] >> 4)) & 0x3f];
        strEncode += encodeTable[((tmp[2] << 2) | (tmp[3] >> 6)) & 0x3f];
        strEncode += encodeTable[tmp[3] & 0x3f];
        if(lineLength += 4, lineLength == 76) {
            strEncode+="\r\n";
            lineLength=0;
        }
    }
    //对剩余数据进行编码
    int mod = len % 3;
    if(mod == 1) {
        tmp[1] = *data++;
        strEncode += encodeTable[(tmp[1] & 0xfc) >> 2];
        strEncode += encodeTable[((tmp[1] & 0x03) << 4)];
        strEncode += "==";
    } else if (mod == 2) {
        tmp[1] = *data++;
        tmp[2] = *data++;
        strEncode += encodeTable[(tmp[1] & 0xfc) >> 2];
        strEncode += encodeTable[((tmp[1] & 0x03) << 4) | ((tmp[2] & 0xf0) >> 4)];
        strEncode += encodeTable[((tmp[2] & 0x0f) << 2)];
        strEncode += "=";
    }

#pragma GCC diagnostic warning "-Wconversion"
    return strEncode;
}

// }}}
// {{{ void base64Decode()

void base64Decode(char* data, int* dataLen, const std::string& str) {
#pragma GCC diagnostic ignored "-Wchar-subscripts"
#pragma GCC diagnostic ignored "-Wconversion"
    //解码表
    const char decodeTable[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };

    //返回值
    int nValue;
    size_t i = 0;
	*dataLen = 0;
	const char* srcData = str.data();
    while (i < str.size()) {
        if (*srcData != '\r' && *srcData != '\n') {
            nValue = decodeTable[*srcData++] << 18;
            nValue += decodeTable[*srcData++] << 12;
			*(data + *dataLen) = (nValue & 0x00ff0000) >> 16;
            (*dataLen)++;
            if (*srcData != '=') {
                nValue += decodeTable[*srcData++] << 6;
				*(data + *dataLen) = (nValue & 0x0000ff00) >> 8;
            	(*dataLen)++;
                if (*srcData != '=') {
                    nValue += decodeTable[*srcData++];
					*(data + *dataLen) = nValue & 0x000000ff;
					(*dataLen)++;
                }
            }
            i += 4;
        } else { // 回车换行,跳过
            srcData++;
            i++;
        }
    }
#pragma GCC diagnostic warning "-Wchar-subscripts"
#pragma GCC diagnostic warning "-Wconversion"
}

// }}}
// {{{ std::string encodeSegment()

std::string encodeSegment(std::string id) {
	char map[62] = {'0','1','2','3','4','5','6','7','8','9',
		'a','b','c','d','e','f','g','h','i','j','k','l','m',
		'n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L','M',
		'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
	errno = 0;
	uint64_t value = static_cast<uint64_t>(strtoull(id.c_str(), nullptr, 10));
	if (errno != 0) {
		return "";
	}
	std::string base62 = "";
	while (value != 0) {
		base62 += map[value % 62];
		value /= 62;
	}
	std::reverse(base62.begin(), base62.end());

	return base62;
}

// }}}
// {{{ std::string base62Encode()

std::string base62Encode(uint64_t id) {
	std::string mid = std::to_string(id);
	int midlen = static_cast<int>(mid.size());
	int segments = static_cast<int>(ceil(static_cast<double>(midlen) / ENCODEBLOCKSIZE));
	int start = midlen;
	std::string base62 = "";
	for (int i = 1; i < segments; ++i) {
		start -= ENCODEBLOCKSIZE;
		std::string seg = mid.substr(start, ENCODEBLOCKSIZE);
		seg = encodeSegment(seg);
		int segLen = static_cast<int>(seg.size());
		if (segLen < DECODEBLOCKSIZE) {
			seg.insert(seg.begin(), DECODEBLOCKSIZE - segLen, '0');
		} 
		base62 = seg + base62;
	}
	base62 = encodeSegment(mid.substr(0, start)) + base62;

	return base62;
}

// }}}
// {{{ uint64_t decodeSegment()

std::string decodeSegment(const std::string& str) {
	std::string map = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint64_t result = 0;
	int length = static_cast<int>(str.size());
	for (int i = 0; i < length; i++) {
		int index = static_cast<int>(map.find_first_of(str[i]));	
		result += static_cast<uint64_t>(index * pow( 62, length - i - 1));
	}

	return std::to_string(result);
}

// }}}
// {{{ uint64_t base62Decode()

uint64_t base62Decode(const std::string& str) {
	int strlen = static_cast<int>(str.size());
	int segments = static_cast<int>(ceil(static_cast<double>(strlen) / DECODEBLOCKSIZE));
	int start = strlen; 
	std::string mid = "";
	for (int i = 1; i < segments; ++i) {
		start -= DECODEBLOCKSIZE;
		std::string seg = str.substr(start, DECODEBLOCKSIZE);
		seg = decodeSegment(seg);
		int segLen = static_cast<int>(seg.size());
		if (segLen < ENCODEBLOCKSIZE) {
			seg.insert(seg.begin(), ENCODEBLOCKSIZE - segLen, '0');
		} 
		mid = seg + mid;
	}
	mid = decodeSegment(str.substr(0, start)) + mid;
	errno  = 0;
	uint64_t result = static_cast<uint64_t>(strtoull(mid.c_str(), nullptr, 10));
	if (errno != 0) {
		return 0;
	}
	
	return result;
}

// }}}
}
