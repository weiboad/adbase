#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main() {
	const std::string input = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	adbase::Sha1 checksum;
	checksum.update(input);
	char hash[20] = {0};
	checksum.final(hash);
	LOG_INFO << "The SHA-1 of \"" << input << "\" is: " << adbase::base64Encode(hash, 20);

    std::string key = "86ae80bd15656981816e5ea3bda86c3e";
    char out[20] = {0};
    adbase::Sha1::hmacSha1(key.c_str(), key.size(), input.c_str(), input.size(), out);
    LOG_INFO << adbase::base64Encode(out, 20);
    return 0;
}
