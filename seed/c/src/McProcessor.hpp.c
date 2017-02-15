//@IF @adserver
#ifndef @ADINF_PROJECT_NAME|upper@_MC_PROCESSOR_HPP_
#define @ADINF_PROJECT_NAME|upper@_MC_PROCESSOR_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Mc.hpp>
#include "AdbaseConfig.hpp"

class McProcessor {
public:
	McProcessor(AdServerContext* context);
	~McProcessor();
    adbase::mc::ProtocolBinaryResponseStatus add(const void* key,
            uint16_t keylen,
            const void *data,
            uint32_t datalen,
            uint32_t flags,
            uint32_t exptime,
            uint64_t* cas);
    adbase::mc::ProtocolBinaryResponseStatus append(const void* key,
            uint16_t keylen,
            const void *data,
            uint32_t datalen,
            uint64_t cas,
            uint64_t* resultCas);
	adbase::mc::ProtocolBinaryResponseStatus decrement(const void* key,
			uint16_t keylen,
			uint64_t delta,
			uint64_t initial,
			uint32_t expiration,
			uint64_t* result,
			uint64_t* resultCas);
	adbase::mc::ProtocolBinaryResponseStatus deleteOp(const void* key,
			uint16_t keylen,
			uint64_t cas);
	adbase::mc::ProtocolBinaryResponseStatus flush(uint32_t when);
	adbase::mc::ProtocolBinaryResponseStatus get(const void* key,
			uint16_t keylen,
			adbase::Buffer *data);
	adbase::mc::ProtocolBinaryResponseStatus increment(const void* key,
			uint16_t keylen,
			uint64_t delta,
			uint64_t initial,
			uint32_t expiration,
			uint64_t* result,
			uint64_t* resultCas);	
	adbase::mc::ProtocolBinaryResponseStatus prepend(const void* key,
			uint16_t keylen,
			const void *data,
			uint32_t datalen,
			uint64_t cas,
			uint64_t* resultCas);	
	adbase::mc::ProtocolBinaryResponseStatus noop();
	adbase::mc::ProtocolBinaryResponseStatus quit();
	adbase::mc::ProtocolBinaryResponseStatus replace(const void* key,
			uint16_t keylen,
			const void *data,
			uint32_t datalen,
			uint32_t flags,
			uint32_t exptime,
			uint64_t cas,
			uint64_t* resultCas);
	adbase::mc::ProtocolBinaryResponseStatus set(const void* key,
			uint16_t keylen,
			const void *data,
			uint32_t datalen,
			uint32_t flags,
			uint32_t exptime,
			uint64_t cas,
			uint64_t* resultCas);
	adbase::mc::ProtocolBinaryResponseStatus stat(const void* key,
			uint16_t keylen,
			adbase::Buffer *data);
	adbase::mc::ProtocolBinaryResponseStatus version(adbase::Buffer *data);
	adbase::mc::ProtocolBinaryResponseStatus verbosity(uint32_t);
	void preExecute();
	void postExecute();
	void unknownExecute();

private:
	/// 传输上下文指针
	AdServerContext* _context; 
	AdbaseConfig* _configure;
};

#endif
//@ENDIF
