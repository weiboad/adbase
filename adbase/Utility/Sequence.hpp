#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_SEQUENCE_HPP_
#define ADBASE_UTILITY_SEQUENCE_HPP_

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/


#define SEQUENCE_BASE_TIME 1451577600 // 2016-01-01 00:00:00

///appid time  seq   mac
///10  +  30  + 15 +  9
class Sequence {
public:
	Sequence();
	uint64_t getSeqId(uint16_t macId, uint16_t appid);
	~Sequence();

private:
	static uint64_t _currentTime;
	static uint64_t _seqId;
	std::string debug64(uint64_t value);
};

/*@}*/

}	

#endif
