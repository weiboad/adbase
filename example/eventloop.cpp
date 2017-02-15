#include <signal.h>
#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

adbase::EventLoop* gloop = nullptr;
adbase::TcpServer* gserver = nullptr;

void test(void*) {
	LOG_INFO << "test timer" << " close.";
}

// {{{ static void killSignal()

static void killSignal(const int sig) {
	(void)sig;
	if (gloop != nullptr) {
		gloop->stop();
		delete gloop;
		gloop = nullptr;
	}
	if (gserver != nullptr) {
		gserver->stop();
		delete gserver;
		gserver = nullptr;
	}

	exit(0);
}

// }}}
// {{{ static void reloadConf()

static void reloadConf(const int sig) {
	(void)sig;
}

// }}}
// {{{ static void registerSignal()

static void registerSignal() {
	/* 忽略Broken Pipe信号 */
	signal(SIGPIPE, SIG_IGN);
	/* 处理kill信号 */
	signal(SIGINT,  killSignal);
	signal(SIGKILL, killSignal);
	signal(SIGQUIT, killSignal);
	signal(SIGTERM, killSignal);
	signal(SIGHUP,  killSignal);
	signal(SIGSEGV, killSignal);
	signal(SIGUSR1, reloadConf);
}

// }}}

void onConnection(const adbase::TcpConnectionPtr& conn) {
	LOG_INFO << conn->getTcpInfoString();
}

void onClose(const adbase::TcpConnectionPtr& conn) {
	LOG_INFO << conn->getCookie() << " close.";
}

void onMessage(const adbase::TcpConnectionPtr& conn, evbuffer* evbuf, adbase::Timestamp reciveTime) {
	char buf[1];
	int n;
	while((n = evbuffer_remove(evbuf, buf, sizeof buf)) > 0) {
		conn->send(buf, sizeof buf);
	}
	LOG_DEBUG << "Recive time:" << reciveTime.toString();
}

int main(void) {
	registerSignal();
	adbase::EventLoop* eventloop = new adbase::EventLoop();
	gloop = eventloop;
	adbase::Timer timer(eventloop->getBase());
	//timer.runEvery(3000, std::bind(test, std::placeholders::_1), nullptr);
	adbase::InetAddress addr("127.0.0.1", 10086);
	adbase::TcpServer* server = new adbase::TcpServer(eventloop->getBase(), addr, "test");
	gserver = server;
	server->setConnectionCallback(std::bind(onConnection, std::placeholders::_1));
	server->setMessageCallback(std::bind(onMessage, std::placeholders::_1,
				std::placeholders::_2, std::placeholders::_3));
	server->setCloseCallback(std::bind(onClose, std::placeholders::_1));

	server->start(4);

	eventloop->start();
	return 0;
}
