#include <signal.h>
#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

adbase::EventLoop* gloop = nullptr;
// {{{ callback
void onConnection(const adbase::TcpConnectionPtr& conn) {
	LOG_INFO << conn->getTcpInfoString();
	std::string test = "hello";
	conn->send(test.c_str(), test.size());
}

void onClose(const adbase::TcpConnectionPtr& conn) {
	LOG_INFO << conn->getCookie() << " close.";
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void onMessage(const adbase::TcpConnectionPtr& conn, evbuffer* evbuf, adbase::Timestamp reciveTime) {
	char buf[1];
	int n;
	while((n = evbuffer_remove(evbuf, buf, sizeof buf)) > 0) {
		conn->send(buf, sizeof buf);
	}
	std::string test = "hello";
	conn->send(test.c_str(), test.size());
	LOG_DEBUG << "Recive time:" << reciveTime.toString();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
// }}}
// {{{ Client
class Client {
public:
	Client() {}
	~Client(){}
	void start() {
		ThreadPtr t1(new std::thread(std::bind(&Client::threadFunc, this, std::placeholders::_1), nullptr), &Client::deleteThread);
		Threads.push_back(std::move(t1));
	}
	void stop() {
		if (_client != nullptr) {
			//_client->stop();
			//delete _client;
			//_client = nullptr;
		}

		if (_loop != nullptr) {
			_loop->stop();
		}	
	}
	void threadFunc(void*) {
		EventLoop eventLoop(new adbase::EventLoop()); 
		_loop = std::move(eventLoop);	
		adbase::Timer timer(_loop->getBase());
		_client = new adbase::TcpClient(_loop->getBase(), "127.0.0.1", 10086, "MC-CLIENT", &timer);
		_client->enableRetry();	
		_client->setConnectionCallback(std::bind(::onConnection, std::placeholders::_1));
		_client->setMessageCallback(std::bind(::onMessage, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));
		_client->connect();
		_loop->start();
	}
	static void deleteThread(std::thread *t) {
		t->join();
		delete t;
		LOG_INFO << "delete thread";
	}

private:
	typedef std::unique_ptr<std::thread, decltype(&Client::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	typedef std::shared_ptr<adbase::EventLoop> EventLoop;
	EventLoop _loop;
	ThreadPool Threads;
	adbase::TcpClient* _client;
};

// }}}
Client* client = nullptr;

void test(void*) {
	if (client != nullptr) {
		client->stop();
		delete client;
		client = nullptr;
	}
	client = new Client;
	client->start();
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
	if (client != nullptr) {
		client->stop();
		delete client;
		client = nullptr;
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
int main(void) {
	registerSignal();
	adbase::EventLoop* eventloop = new adbase::EventLoop();
	gloop = eventloop;
	adbase::Timer timer(eventloop->getBase());
	timer.runEvery(20, std::bind(test, std::placeholders::_1), client);

	//client = new Client;
	//client->start();
	//int i = 2;
	//while (1) {
    //    std::this_thread::sleep_for(std::chrono::milliseconds(1000));	
	//	if (i == 0) {
	//		if (client != nullptr) {
	//			client->stop();
	//			delete client;
	//			client = nullptr;
	//		}
	//	}
	//	i--;
	//}
	eventloop->start();
	return 0;
}
