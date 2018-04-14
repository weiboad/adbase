//@IF @grpc
#ifndef @ADINF_PROJECT_NAME|upper@_GRPC_SERVER_SERVICE_IMPL_HPP_
#define @ADINF_PROJECT_NAME|upper@_GRPC_SERVER_SERVICE_IMPL_HPP_
#include <grpc++/grpc++.h>
#include "adbase.grpc.pb.h"
#include "AdbaseConfig.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using @ADINF_PROJECT_NAME@::StatusRequest;
using @ADINF_PROJECT_NAME@::StatusReply;

namespace adserver {
namespace grpc {
class ServerServiceImpl final : public @ADINF_PROJECT_NAME@::Server::Service {
public:
	ServerServiceImpl(AdServerContext* context);
	Status status(ServerContext* context, const StatusRequest* request, StatusReply* reply) override;
private:
	AdServerContext* _context;
	AdbaseConfig* _configure;
};
}
}
#endif
//@ENDIF
