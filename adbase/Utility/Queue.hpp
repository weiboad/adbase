#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_QUEUE_HPP_
#define ADBASE_UTILITY_QUEUE_HPP_

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace adbase {

/** 
 * @addtogroup utility adbase::Utility
 */
/*@{*/ 

template <typename T>
class Queue {
private:
	mutable std::mutex mut;
	std::queue<T> data;
	size_t size;
	std::condition_variable dataCond;
public:
	Queue() : size(0) {}

	/// 复制构造函数
	Queue(const Queue& other) {
		std::lock_guard<std::mutex> lk(mut);	
		data = other.data;
		size = other.size;
	}

	/// 由于赋值构造存在要释放原对象中的成员变量值，对于队列在多线程下处理为了方便赋值构造函数禁用
	Queue& operator=(const Queue&) = delete;

	/// 入队列操作
	void push(T value) {
		std::lock_guard<std::mutex> lk(mut);	
		data.push(value);
		size++;
		dataCond.notify_one();
	}

	/// 入队列操作
	bool tryPop(T &value) {		
		std::lock_guard<std::mutex> lk(mut);	
		if (data.empty()) {
			return false;	
		}
		value = data.front();
		data.pop();
		size--;
		return true;
	}

	/// 通过 shared 智能指针返回
	std::shared_ptr<T> tryPop() {
		std::lock_guard<std::mutex> lk(mut);	
		if (data.empty()) {
			return std::shared_ptr<T>();	
		}	
		std::shared_ptr<T> res = std::make_shared<T>(data.front());
		data.pop();
		size--;
		return res;
	}

	/// 当队列为空时阻塞等待数据
	void waitPop(T &value) {
		std::unique_lock<std::mutex> lk(mut);
		dataCond.wait(lk, [this]{return !data.empty();});
		value = data.front();
		data.pop();
		size--;
	}

	std::shared_ptr<T> waitPop() {
		std::unique_lock<std::mutex> lk(mut);
		dataCond.wait(lk, [this]{return !data.empty();});
		std::shared_ptr<T> res = std::make_shared<T>(data.front());
		data.pop();
		size--;
		return res;
	}

	bool empty() const {
		std::lock_guard<std::mutex> lk(mut);	
		return data.empty();	
	}

	size_t getSize() const {
		return size;	
	}
};

/*@}*/ 

}
#endif
