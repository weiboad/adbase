//@IF @grpc
#ifndef @ADINF_PROJECT_NAME|upper@_GRPC_HPP_
#define @ADINF_PROJECT_NAME|upper@_GRPC_HPP_

#include "AdbaseConfig.hpp"
#include "Grpc/ServerServiceImpl.hpp"
#include <grpc++/grpc++.h>
#include <grpc/support/log.h>

// {{{ macros

#ifndef DECLARE_GRPC_SERVICE
#define DECLARE_GRPC_SERVICE(name) \
	std::shared_ptr<adserver::grpc:: name> _grpc##name;
#endif
#ifndef ADSERVER_GRPC_ADD_SERVICE
#define ADSERVER_GRPC_ADD_SERVICE(name) do {\
	_grpc##name = std::shared_ptr<adserver::grpc:: name>(new adserver::grpc:: name(_context)); \
	builder.RegisterService(_grpc##name.get()); \
} while(0)
#endif

// }}}
class Grpc {
public:
	Grpc(AdServerContext* context);
	void start();
	void stop();
	void threadFunc(void *data);
	void setGrpcLoggerLevel();
	~Grpc();
	static void deleteThread(std::thread *t);
private:
	void addService(grpc::ServerBuilder& builder);
	AdServerContext* _context;
	std::unique_ptr<grpc::Server> _server;
	typedef std::unique_ptr<std::thread, decltype(&Grpc::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool Threads;

	DECLARE_GRPC_SERVICE(ServerServiceImpl);
};

#endif
//@ENDIF
