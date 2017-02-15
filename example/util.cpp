#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <string>
#include <thread>
#include <cstring>
#include <iostream>

int main() {
	/// url encode/decode example
	std::string url = "http://www.weibo.com?test=1&test??????"; 
	std::string encode = adbase::urlEncode(url);
	std::cout << encode << std::endl;
	std::cout << adbase::urlDecode(encode) << std::endl;

	/// base64 encode/decode example
	std::string base64 = "test base64";
	std::string base64encode = adbase::base64Encode(base64.c_str(), base64.size());
	std::cout << base64encode << std::endl;
	char base64decode[base64encode.size()];
	memset(base64decode, 0, base64encode.size());
	int length = 0;
	adbase::base64Decode(base64decode, &length, base64encode);
	std::cout << base64decode << std::endl;

	// hex string <---> bits
	std::string str = "1d23456adbcdef";
	char buffer[str.size() / 2];
	adbase::hexStringToBytes(str, buffer);
	std::cout << adbase::bytesToHexString(buffer, str.size() / 2) << std::endl;

	// sha1
	const std::string input = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	adbase::Sha1 checksum;
	checksum.update(input);
	char hash[20] = {0};
	checksum.final(hash);
	std::cout << "The SHA-1 of \"" << input << "\" is: " << adbase::base64Encode(hash, 20) << std::endl;

	// Timestamp
	adbase::Timestamp time(10000010);
	std::cout << time.toString() << std::endl;
	std::cout << time.toFormattedString() << std::endl;
	adbase::Timestamp invlidTime(-1);
	if (invlidTime.valid()) {
		std::cout << "Timestamp valid" << std::endl;
	} else {
		std::cout << "Timestamp invalid" << std::endl;
	}

	adbase::Timestamp t1 = adbase::Timestamp::now();
	adbase::Timestamp t2 = adbase::addTime(t1, 300);
	std::cout << "t1:" << t1.toFormattedString() << std::endl;
	std::cout << "t2:" << t2.toFormattedString() << std::endl;
	t1.swap(t2);	
	std::cout << "t1:" << t1.toFormattedString() << std::endl;
	std::cout << "t2:" << t2.toFormattedString() << std::endl;

	// string replace
	std::string search = "the";
	std::string replace = "< an the test >";
	std::string subject = "The substring is the portion of the object that starts at character position pos and spans len characters (or until the end of the string, whichever comes first).";
	int count;
	std::cout << subject << std::endl;
	std::cout << adbase::replace(search, replace, subject, count) << std::endl;
	std::cout << count << std::endl;

	std::cout << adbase::procFdNum() << std::endl;

	// hmac_sha1
	std::string token = "OXRNNWQQVWVPXNXONYUWYWT=ONYOQYRN=ONYUSYPT=ONYOQYRXORTUUVTWPSXNXNX8wKUaoQ2Pe";
	std::string tokenSecret = "b201d7582deac692c252";
	std::string param = "uid=1810958350"; 
	std::string tokenStr = adbase::urlEncode(token);
	char signOut[20] = {0};
	adbase::Sha1::hmacSha1(tokenSecret.c_str(), tokenSecret.size(), param.c_str(), param.size(), signOut);
	std::string sign = adbase::urlEncode(adbase::base64Encode(signOut, 20));
	std::cout << "sign: " << sign << std::endl;
	std::cout << "token: " << tokenStr << std::endl;

	adbase::Sequence seq;
	uint64_t seqId = seq.getSeqId(3, 3);
	std::string seqIdStr = adbase::base62Encode(seqId);
	std::cout << seqId << std::endl;
	std::cout << seqIdStr << std::endl;
	std::cout << adbase::base62Decode(seqIdStr) << std::endl;

	std::unordered_map<std::string, std::string> stats = adbase::procStats();
	for (auto &t : stats) {
		std::cout << t.first << " -> " << t.second << std::endl;
	}


	auto ips = adbase::ifconfig();
	for (auto &t : ips) {
		for (auto &i : t.second) {
			std::cout << t.first << " -> " << i.second << std::endl;
		}
	}

    //bool ret = adbase::mkdirRecursive("aaa", 0777);
    //LOG_INFO << ret;

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}
