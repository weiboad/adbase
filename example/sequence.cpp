#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main() {
	adbase::Sequence seq;
	uint64_t seqId = seq.getSeqId(3, 3);
	std::string seqIdStr = adbase::base62Encode(seqId);
    LOG_INFO << seqId;
    LOG_INFO << seqIdStr;
	LOG_INFO << adbase::base62Decode(seqIdStr);
    return 0;
}
