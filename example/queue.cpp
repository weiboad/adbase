#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
	
	adbase::Queue<int> queue;
	LOG_INFO << "queue size: " << queue.getSize();
	queue.push(1);
	queue.push(2);
	LOG_INFO << "queue size: " << queue.getSize();

	// tryPop 引用方式
	int value;
	bool isRet = queue.tryPop(value);
	if (isRet) {
		LOG_INFO << "Pop value:" << value;	
	}
	LOG_INFO << "queue size: " << queue.getSize();

	std::shared_ptr<int> ptr = queue.tryPop();
	if (ptr) {
		LOG_INFO << "Pop value:" << *ptr;	
	}
	LOG_INFO << "queue size: " << queue.getSize();
	LOG_INFO << "ptr before count:" << ptr.use_count();

	std::shared_ptr<int> p1 = ptr;
	LOG_INFO << "ptr after count:" << ptr.use_count();
	LOG_INFO << "p1 count:" << p1.use_count();

	ptr.reset();
	LOG_INFO << "ptr reset count:" << ptr.use_count();
	LOG_INFO << "p1 reset count:" << p1.use_count();

	p1.reset();
	LOG_INFO << "ptr reset count:" << ptr.use_count();
	LOG_INFO << "p1 reset count:" << p1.use_count();
	return 0;
}
