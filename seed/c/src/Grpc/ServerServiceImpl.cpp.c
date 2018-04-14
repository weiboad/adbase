#include <grpc++/grpc++.h>
#include "adbase.grpc.pb.h"
#include "ServerServiceImpl.hpp"

namespace adserver {
namespace grpc {
// {{{ ServerServiceImpl::ServerServiceImpl()

ServerServiceImpl::ServerServiceImpl(AdServerContext* context) : _context(context) {
	_configure = _context->config;
}

// }}}
// {{{ ServerServiceImpl::status()

Status ServerServiceImpl::status(ServerContext* context, const StatusRequest* request, StatusReply* reply) {
	return Status::OK;
}

// }}}
}
}
