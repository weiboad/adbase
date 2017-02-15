#if !defined ADBASE_MC_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_MC_INTERFACE_HPP_
#define ADBASE_MC_INTERFACE_HPP_

#include <functional>
#include <adbase/Mc/Binary.hpp>

namespace adbase {

/**
 * @addtogroup mc adbase::Mc
 */
/*@{*/

class Buffer;
namespace mc {
// {{{ Callbacks define

// AddHandler
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													const void *data,
													uint32_t datalen,
													uint32_t flags,
													uint32_t exptime,
													uint64_t* cas)> AddHandler;

// AppendHandler
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													const void *data,
													uint32_t datalen,
													uint64_t cas,
													uint64_t* resultCas)> AppendHandler;	

// DecrementHandler 
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													uint64_t delta,
													uint64_t initial,
													uint32_t expiration,
													uint64_t* result,
													uint64_t* resultCas)> DecrementHandler;	

// DeleteHandler
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													uint64_t cas)> DeleteHandler;	

// FlushHandler
typedef std::function<ProtocolBinaryResponseStatus (uint32_t when)> FlushHandler;

// GetHandler
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													Buffer *data)> GetHandler;

// IncrementHandler 
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													uint64_t delta,
													uint64_t initial,
													uint32_t expiration,
													uint64_t* result,
													uint64_t* resultCas)> IncrementHandler;	

// PrependHandler
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													const void *data,
													uint32_t datalen,
													uint64_t cas,
													uint64_t* resultCas)> PrependHandler;	

// NoopHandler
typedef std::function<ProtocolBinaryResponseStatus ()> NoopHandler;

// QuitHandler
typedef std::function<ProtocolBinaryResponseStatus ()> QuitHandler;

// ReplaceHander
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													const void *data,
													uint32_t datalen,
													uint32_t flags,
													uint32_t exptime,
													uint64_t cas,
													uint64_t* resultCas)> ReplaceHander;

// SetHandler
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													const void *data,
													uint32_t datalen,
													uint32_t flags,
													uint32_t exptime,
													uint64_t cas,
													uint64_t* resultCas)> SetHandler;

// StatHandler
typedef std::function<ProtocolBinaryResponseStatus (const void* key,
													uint16_t keylen,
													Buffer *data)> StatHandler;

// VersionHandler
typedef std::function<ProtocolBinaryResponseStatus (Buffer *data)> VersionHandler;

// VerbosityHandler
typedef std::function<ProtocolBinaryResponseStatus (uint32_t)> VerbosityHandler;

// PreExecute
typedef std::function<void ()> PreExecute;

// PostExecute
typedef std::function<void ()> PostExecute;

// UnknownExecute
typedef std::function<void ()> UnknownExecute;

// }}}
class Interface {
public:
	Interface();
	~Interface();	
	void setAddHandler(const AddHandler& addHandler);
	void setAppendHandler(const AppendHandler& appendHandler);
	void setDecrementHandler(const DecrementHandler& decrementHandler);
	void setDeleteHandler(const DeleteHandler& deleteHandler);
	void setFlushHandler(const FlushHandler& flushHandler);
	void setGetHandler(const GetHandler& getHandler);
	void setIncrementHandler(const IncrementHandler& incrementHandler);
	void setPrependHandler(const PrependHandler& prependHandler);
	void setNoopHandler(const NoopHandler& noopHandler);
	void setQuitHandler(const QuitHandler& quitHandler);
	void setReplaceHandler(const ReplaceHander& replaceHandler);
	void setSetHandler(const SetHandler& setHandler);
	void setVersionHandler(const VersionHandler& versionHandler);
	void setVerbosityHandler(const VerbosityHandler& verbosityHandler);
	void setStatHandler(const StatHandler& statHandler);
	void setPreExecute(const PreExecute& preExecute);
	void setPostExecute(const PostExecute& postExecute);
	void setUnknownExecute(const UnknownExecute& unknownExecute);

	const AddHandler& getAddHandler();
	const AppendHandler& getAppendHandler();
	const DecrementHandler& getDecrementHandler();
	const DeleteHandler& getDeleteHandler();
	const FlushHandler& getFlushHandler();
	const GetHandler& getGetHandler();
	const IncrementHandler& getIncrementHandler();
	const PrependHandler& getPrependHandler();
	const NoopHandler& getNoopHandler();
	const QuitHandler& getQuitHandler();
	const ReplaceHander& getReplaceHandler();
	const SetHandler& getSetHandler();
	const VersionHandler& getVersionHandler();
	const VerbosityHandler& getVerbosityHandler();
	const StatHandler& getStatHandler();
	const PreExecute& getPreExecute();
	const PostExecute& getPostExecute();
	const UnknownExecute& getUnknownExecute();
	
private:
	AddHandler _addHandler;	
	AppendHandler _appendHandler;
	DecrementHandler _decrementHandler;
	DeleteHandler _deleteHandler;
	FlushHandler _flushHandler;
	GetHandler _getHandler;
	IncrementHandler _incrementHandler;
	PrependHandler _prependHandler;
	NoopHandler _noopHandler;
	QuitHandler _quitHandler;
	ReplaceHander _replaceHandler;
	SetHandler _setHandler;
	VersionHandler _versionHandler;
	VerbosityHandler _verbosityHandler;
	StatHandler _statHandler;
	PreExecute _preExecute;
	PostExecute _postExecute;
	UnknownExecute _unknownExecute;
};

}

/*@}*/

}
#endif
