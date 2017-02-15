//@IF @adserver
#include "HeadProcessor.hpp"
#include "Version.hpp"

// {{{ HeadProcessor::HeadProcessor()

HeadProcessor::HeadProcessor(AdServerContext* context) :
	_context(context) {
	_configure = _context->config;
}

// }}}
// {{{ HeadProcessor::~HeadProcessor()

HeadProcessor::~HeadProcessor() {
}

// }}}
// {{{ adbase::head::ProtocolBinaryResponseStatus HeadProcessor::readHandler()	

adbase::head::ProtocolBinaryResponseStatus HeadProcessor::readHandler(adbase::head::ProtocolBinaryDataType datatype,
		const void *data,
		ssize_t datalen,
		adbase::head::ProtocolBinaryDataType* resDataType,
		adbase::Buffer* responseData) {
	(void)data;
	(void)datatype;
	(void)datalen;
	*resDataType = adbase::head::PROTOCOL_BINARY_TYPE_JSON;		
	// 如下 json 手动拼接为了减少对 json 库的依赖，在实际项目中推荐用 rapidjson
	std::unordered_map<std::string, std::string> procs = adbase::procStats();
	procs["version"]    = VERSION;
	procs["git_sha1"]   = GIT_SHA1;
	procs["git_dirty"]  = GIT_DIRTY;
	procs["build_id"]   = BUILD_ID;
	procs["build_type"] = BUILD_TYPE;
	std::string result = "{";
	for (auto &t : procs) {
		result +="\"" + t.first + "\":\"" + t.second + "\",";
	}
	result = adbase::rightTrim(result, ",");
	result += "}";
	responseData->append(result);
	return adbase::head::PROTOCOL_BINARY_RESPONSE_SUCCESS;
}

// }}}
//@ENDIF
