//@IF @grpc
#include "Grpc.hpp"
#include <adbase/Logging.hpp>

// {{{ void grpcLogging()

void grpcLogging(gpr_log_func_args* args) {

	if (args->severity == GPR_LOG_SEVERITY_DEBUG) {
		LOG_DEBUG << "[GRPC] [" << args->file << ":" << args->line << "] " << args->message; 		
	} else if (args->severity == GPR_LOG_SEVERITY_INFO) {
		LOG_INFO << "[GRPC] [" << args->file << ":" << args->line << "] " << args->message; 		
	} else {
		LOG_ERROR << "[GRPC] [" << args->file << ":" << args->line << "] " << args->message; 		
	}
}

// }}}
// {{{ Grpc::Grpc()

Grpc::Grpc(AdServerContext* context) :
	_context(context) {
}

// }}}
// {{{ Grpc::~Grpc()

Grpc::~Grpc() {
}

// }}}
// {{{ void Grpc::addService()

void Grpc::addService(grpc::ServerBuilder& builder) {
	ADSERVER_GRPC_ADD_SERVICE(ServerServiceImpl);
}

// }}}
// {{{ void Grpc::start()

void Grpc::start() {
	ThreadPtr startThread(new std::thread(std::bind(&Grpc::threadFunc, this, std::placeholders::_1), nullptr), &Grpc::deleteThread);
	LOG_DEBUG << "Create grpc thread success";
	Threads.push_back(std::move(startThread));
}

// }}}
// {{{ void Grpc::stop()

void Grpc::stop() {
	if (_server) {
		_server->Shutdown();
	}
}

// }}}
// {{{ void Grpc::threadFunc()

void Grpc::threadFunc(void *) {
	setGrpcLoggerLevel();
	gpr_set_log_function(grpcLogging);
	grpc::ServerBuilder builder;
	std::string grpcAddress = _context->config->grpcHost;
	grpcAddress.append(":");
   	grpcAddress.append(std::to_string(_context->config->grpcPort));
	LOG_DEBUG << "Start server:" << grpcAddress;
	builder.AddListeningPort(grpcAddress, grpc::InsecureServerCredentials());
	addService(builder);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	_server = std::move(server);
	_server->Wait();
}

// }}}
// {{{ void Grpc::setGrpcLoggerLevel()

void Grpc::setGrpcLoggerLevel() {
	if (_context->config->logLevel == 3) {
		gpr_set_log_verbosity(GPR_LOG_SEVERITY_INFO);
	} else {
		gpr_set_log_verbosity(GPR_LOG_SEVERITY_DEBUG);
	}
}

// }}}
// {{{ void Grpc::deleteThread()

void Grpc::deleteThread(std::thread *t) {
	t->join();
	delete t;
}

// }}}
//@ENDIF
