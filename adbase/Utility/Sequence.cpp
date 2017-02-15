#include <adbase/Utility.hpp>
#include <sys/time.h>
#include <math.h>
#include <algorithm>

namespace adbase {
uint64_t Sequence::_currentTime = 0;
uint64_t Sequence::_seqId = 0;
// {{{ Sequence::Sequence()

Sequence::Sequence() {
}

// }}}
// {{{ Sequence::~Sequence()

Sequence::~Sequence() {
}

// }}}
// {{{ uint64_t Sequence::getSeqId()

uint64_t Sequence::getSeqId(uint16_t macId, uint16_t appid) {
	uint64_t result = 0;
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	uint64_t time = tv.tv_sec - SEQUENCE_BASE_TIME;
	if (time == Sequence::_currentTime) {
		if (_seqId >= 32768) { // 最大 15bit
			Sequence::_seqId = 0;
		} else {
			Sequence::_seqId++;
		}
	} else {
		Sequence::_seqId = 0;
		Sequence::_currentTime = time;
	}

	if (macId > 512) { /// max size 512 -> 9bit
		macId = 0;	
	}
	if (appid > 1024) { /// max size 1024 -> 10bit
		appid = 0;	
	}

	// appid 10
	result = (result | appid) << 54;	
	// time 30
	result |= (_currentTime << 24);
	// seq 15
	result |= (_seqId << 9); 
	// macId 9
	result |= macId; 

	return result;
}

// }}}
// {{{ std::string Sequence::debug64()

std::string Sequence::debug64(uint64_t value) {
	uint64_t base = 0x01;
	std::string str = "";
	for (int i = 63; i >= 0; i--) {
		str += (value & (base << i)) ? "1 " : "0 ";
	}

	return str;
}

// }}}
}
