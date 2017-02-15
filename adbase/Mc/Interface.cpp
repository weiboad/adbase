#include <adbase/Mc.hpp>

namespace adbase {
namespace mc {
// {{{ Interface::Interface()

Interface::Interface() {
	
}

// }}}
// {{{ Interface::~Interface()

Interface::~Interface() {
	
}

// }}}
// {{{ void Interface::setAddHandler()

void Interface::setAddHandler(const AddHandler& addHandler) {
	_addHandler = addHandler;	
}
 
// }}}
// {{{ void Interface::setAppendHandler()

void Interface::setAppendHandler(const AppendHandler& appendHandler) {
	_appendHandler = appendHandler;	
}
 
// }}}
// {{{ void Interface::setDecrementHandler()

void Interface::setDecrementHandler(const DecrementHandler& decrementHandler) {
	_decrementHandler = decrementHandler;	
}
 
// }}}
// {{{ void Interface::setDeleteHandler()

void Interface::setDeleteHandler(const DeleteHandler& deleteHandler) {
	_deleteHandler = deleteHandler;	
}
 
// }}}
// {{{ void Interface::setFlushHandler()

void Interface::setFlushHandler(const FlushHandler& flushHandler) {
	_flushHandler = flushHandler;	
}
 
// }}}
// {{{ void Interface::setGetHandler()

void Interface::setGetHandler(const GetHandler& getHandler) {
	_getHandler = getHandler;	
}
 
// }}}
// {{{ void Interface::setIncrementHandler()

void Interface::setIncrementHandler(const IncrementHandler& incrementHandler) {
	_incrementHandler = incrementHandler;	
}
 
// }}}
// {{{ void Interface::setPrependHandler()

void Interface::setPrependHandler(const PrependHandler& prependHandler) {
	_prependHandler = prependHandler;	
}
 
// }}}
// {{{ void Interface::setNoopHandler()

void Interface::setNoopHandler(const NoopHandler& noopHandler) {
	_noopHandler = noopHandler;	
}
 
// }}}
// {{{ void Interface::setQuitHandler()

void Interface::setQuitHandler(const QuitHandler& quitHandler) {
	_quitHandler = quitHandler;	
}
 
// }}}
// {{{ void Interface::setReplaceHandler()

void Interface::setReplaceHandler(const ReplaceHander& replaceHandler) {
	_replaceHandler = replaceHandler;	
}
 
// }}}
// {{{ void Interface::setSetHandler()

void Interface::setSetHandler(const SetHandler& setHandler) {
	_setHandler = setHandler;	
}
 
// }}}
// {{{ void Interface::setVersionHandler()

void Interface::setVersionHandler(const VersionHandler& versionHandler) {
	_versionHandler = versionHandler;	
}
 
// }}}
// {{{ void Interface::setVerbosityHandler()

void Interface::setVerbosityHandler(const VerbosityHandler& verbosityHandler) {
	_verbosityHandler = verbosityHandler;	
}
 
// }}}
// {{{ void Interface::setStatHandler()

void Interface::setStatHandler(const StatHandler& statHandler) {
	_statHandler = statHandler;	
}
 
// }}}
// {{{ void Interface::setPreExecute()

void Interface::setPreExecute(const PreExecute& preExecute) {
	_preExecute = preExecute;	
}
 
// }}}
// {{{ void Interface::setPostExecute()

void Interface::setPostExecute(const PostExecute& postExecute) {
	_postExecute = postExecute;	
}
 
// }}}
// {{{ void Interface::setUnknownExecute()

void Interface::setUnknownExecute(const UnknownExecute& unknownExecute) {
	_unknownExecute = unknownExecute;	
}
 
// }}}
// {{{ const AddHandler& Interface::getAddHandler()

const AddHandler& Interface::getAddHandler() {
	return _addHandler;	
}

// }}}
// {{{ const AppendHandler& Interface::getAppendHandler()

const AppendHandler& Interface::getAppendHandler() {
	return _appendHandler;	
}

// }}}
// {{{ const DecrementHandler& Interface::getDecrementHandler()

const DecrementHandler& Interface::getDecrementHandler() {
	return _decrementHandler;	
}

// }}}
// {{{ const DeleteHandler& Interface::getDeleteHandler()

const DeleteHandler& Interface::getDeleteHandler() {
	return _deleteHandler;	
}

// }}}
// {{{ const FlushHandler& Interface::getFlushHandler()

const FlushHandler& Interface::getFlushHandler() {
	return _flushHandler;	
}

// }}}
// {{{ const GetHandler& Interface::getGetHandler()

const GetHandler& Interface::getGetHandler() {
	return _getHandler;	
}

// }}}
// {{{ const IncrementHandler& Interface::getIncrementHandler()

const IncrementHandler& Interface::getIncrementHandler() {
	return _incrementHandler;	
}

// }}}
// {{{ const PrependHandler& Interface::getPrependHandler()

const PrependHandler& Interface::getPrependHandler() {
	return _prependHandler;	
}

// }}}
// {{{ const NoopHandler& Interface::getNoopHandler()

const NoopHandler& Interface::getNoopHandler() {
	return _noopHandler;	
}

// }}}
// {{{ const QuitHandler& Interface::getQuitHandler()

const QuitHandler& Interface::getQuitHandler() {
	return _quitHandler;	
}

// }}}
// {{{ const ReplaceHander& Interface::getReplaceHandler()

const ReplaceHander& Interface::getReplaceHandler() {
	return _replaceHandler;	
}

// }}}
// {{{ const SetHandler& Interface::getSetHandler()

const SetHandler& Interface::getSetHandler() {
	return _setHandler;	
}

// }}}
// {{{ const VersionHandler& Interface::getVersionHandler()

const VersionHandler& Interface::getVersionHandler() {
	return _versionHandler;	
}

// }}}
// {{{ const VerbosityHandler& Interface::getVerbosityHandler()

const VerbosityHandler& Interface::getVerbosityHandler() {
	return _verbosityHandler;	
}

// }}}
// {{{ const StatHandler& Interface::getStatHandler()

const StatHandler& Interface::getStatHandler() {
	return _statHandler;	
}

// }}}
// {{{ const PreExecute& Interface::getPreExecute()

const PreExecute& Interface::getPreExecute() {
	return _preExecute;	
}

// }}}
// {{{ const PostExecute& Interface::getPostExecute()

const PostExecute& Interface::getPostExecute() {
	return _postExecute;	
}

// }}}
// {{{ const UnknownExecute& Interface::getUnknownExecute()

const UnknownExecute& Interface::getUnknownExecute() {
	return _unknownExecute;	
}

// }}}
}
}
