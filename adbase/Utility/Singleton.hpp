#if !defined ADBASE_UTILITY_HPP_
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_SINGLETON_HPP_
#define ADBASE_UTILITY_SINGLETON_HPP_

#include <mutex>
#include <thread>

namespace adbase {
    
namespace wrapper {
    template <class T>
    class SingletonWrapper : public T {
        public:
            static bool _destroyed;
            ~SingletonWrapper() { _destroyed = true; }
    };

    template <class T>
    bool SingletonWrapper<T>::_destroyed = false;
}

template <typename T>
class Singleton {
    private:
        Singleton(const Singleton &) = delete;
        Singleton& operator=(const Singleton &) = delete;

    protected:
        Singleton() {}

    public:
        typedef T selfType;
        typedef std::shared_ptr<selfType> ptrT;

        static T &getInstance() { 
            return *me(); 
        }

        static const T &getConstInstance() {
            return getInstance(); 
        }

        static selfType *instance() { return me().get(); }

        static ptrT &me() {
            static ptrT inst;
            static std::mutex _mut;
            if (inst == nullptr) {
                std::lock_guard<std::mutex> lk(_mut);
                if (inst == nullptr) {
                    ptrT newData = std::make_shared<wrapper::SingletonWrapper<selfType>>();
                    inst = newData;
                }
            }
            return inst;
        }

        static bool isInstanceDestroyed() { 
            return wrapper::SingletonWrapper<T>::_destroyed;
        }
};

}
#endif
