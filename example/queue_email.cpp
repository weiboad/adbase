#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <thread>
#include <atomic>

typedef enum {
	MESSAGE_TEXT = 0x01,
	MESSAGE_STOP = 0xff // 当判断发现消息类型是 stop 类型，发送邮件的线程退出
} MessageType;
typedef struct sendMessage {
	MessageType type;
	std::string data;
} Message;
void deleteThread(std::thread *t) {
	t->join();
	LOG_ERROR << "Consumer main thread stop.";
	delete t;
}
typedef std::unique_ptr<std::thread, decltype(&deleteThread)> ThreadPtr;

void threadFunc(void *data) {
	adbase::Queue<Message>* queue = reinterpret_cast<adbase::Queue<Message>*>(data);
	while(true) {
		Message message;
		queue->waitPop(message);
		if (message.type == MESSAGE_STOP) { // 如果是 stop 类型程序退出
			break;	
		}

		// send mail....
		LOG_INFO << "send email data:" << message.data;
	}
}

int main(void) {
	adbase::Queue<Message> queue;
    ThreadPtr emailThread(new std::thread(std::bind(&threadFunc, std::placeholders::_1), &queue), &deleteThread);

	Message message;
	message.type = MESSAGE_TEXT;
	message.data = "text 1";
	queue.push(message);

    std::this_thread::sleep_for(std::chrono::seconds(1));
	message.type = MESSAGE_TEXT;
	message.data = "text 2";
	queue.push(message);
    std::this_thread::sleep_for(std::chrono::seconds(1));
	message.type = MESSAGE_TEXT;
	message.data = "text 3";
	queue.push(message);

    std::this_thread::sleep_for(std::chrono::seconds(1));
	message.type = MESSAGE_STOP;
	queue.push(message);

	emailThread.reset();
}
