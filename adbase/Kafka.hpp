#ifndef ADBASE_KAFKA_HPP_
#define ADBASE_KAFKA_HPP_

/** 
 * @defgroup kafka adbase::Kafka
 *
 * @brief adbase Kafka Sdk(使用方法见该模块的描述信息)
 *
 * @par 使用方法
 *
 * @note 请勿直接引用对应头文件，adbase::Kafka 模块统一引用<adbase/Kafka.hpp> 文件
 *
 * @par Example for producer
 *
 * @code
 * #include <adbase/Kafka.hpp>
 * #include <adbase/Net.hpp>
 * #include <adbase/Logging.hpp>
 * #include <signal.h>
 * 
 * adbase::EventLoop* gEventLoop = nullptr;
 * adbase::kafka::Producer* gProducer = nullptr;
 * 
 * // {{{ class SendMessage
 * 
 * class SendMessage {
 * public:
 *     SendMessage() :
 *         _index(0) {
 *     }
 *     ~SendMessage() {}
 * 
 *     // 如果有数据要发送 返回 true, 否则返回false
 *     bool send(std::string& topicName, int* partId, adbase::Buffer& message, uint64_t* ackCode) {
 *         topicName = "test";
 *         message.append("TEST" + std::to_string(_index));
 * 
 *         *partId = rand() % 5;
 *         *ackCode = _index;
 *         if (_index == 10) {
 *             return false;
 *         }
 *         _index++;
 *         std::this_thread::sleep_for(std::chrono::seconds(1));
 *         LOG_INFO << "Send message";
 *         return true;
 *     }
 * 
 *     void ackCallback(uint64_t ackCode) {
 *         LOG_INFO << "Ack send ackCode: " << ackCode;
 *     }
 * 
 *     void errorCallback(uint64_t ackCode) {
 *         LOG_ERROR << "Error send ackCode: " << ackCode;
 *     }
 * 
 * private:
 *     uint64_t _index;
 * };
 * 
 * // }}}
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (gEventLoop != nullptr) {
 *         gEventLoop->stop();
 *         delete gEventLoop;
 *     }
 *     if (gProducer != nullptr) {
 *         gProducer->stop();
 *         delete gProducer;
 *     }
 *     exit(0);
 * }
 * 
 * // }}}
 * // {{{ static void reloadConf()
 * 
 * static void reloadConf(const int sig) {
 *     (void)sig;
 * }
 * 
 * // }}}
 * // {{{ static void registerSignal()
 * 
 * static void registerSignal() {
 *     /// 忽略Broken Pipe信号
 *     signal(SIGPIPE, SIG_IGN);
 *     /// 处理kill信号 
 *     signal(SIGINT,  killSignal);
 *     signal(SIGKILL, killSignal);
 *     signal(SIGQUIT, killSignal);
 *     signal(SIGTERM, killSignal);
 *     signal(SIGHUP,  killSignal);
 *     signal(SIGSEGV, killSignal);
 *     signal(SIGUSR1, reloadConf);
 * }
 * 
 * // }}}
 * 
 * int main(void) {
 *     registerSignal();
 *     adbase::TimeZone tz(8*3600, "CST");
 *     adbase::Logger::setTimeZone(tz);
 *     gEventLoop = new adbase::EventLoop();
 * 
 *     SendMessage sendMessage;
 *     gProducer = new adbase::kafka::Producer("10.13.4.161:9192", 1000, "none");
 *     gProducer->setSendHandler(std::bind(&SendMessage::send, &sendMessage, std::placeholders::_1,
 *                                        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
 *     gProducer->setAckHandler(std::bind(&SendMessage::ackCallback, &sendMessage, std::placeholders::_1));
 *     gProducer->setErrorHandler(std::bind(&SendMessage::errorCallback, &sendMessage, std::placeholders::_1));
 *     gProducer->start();
 * 
 *     gEventLoop->start();
 * 
 *     return 0;
 * }
 * 
 * @endcode
 *
 * @par Example for consumer
 * 
 * @code
 * #include <adbase/Kafka.hpp>
 * #include <adbase/Net.hpp>
 * #include <adbase/Logging.hpp>
 * #include <signal.h>
 * 
 * adbase::EventLoop* gEventLoop = nullptr;
 * adbase::kafka::Consumer* gConsumer = nullptr;
 * 
 * // {{{ class PullMessage
 * 
 * class PullMessage {
 * public:
 *     PullMessage() {}
 *     ~PullMessage() {}
 * 
 *     bool pull(const std::string& topicName, int partId, uint64_t offset, const adbase::Buffer& message) {
 *         (void)topicName;
 *         LOG_ERROR << "Size:" << message.readableBytes();
 *         LOG_ERROR << partId << "-Offset:" << offset;
 *         //LOG_INFO << "Topic:" << topicName << " partId:" << partId << " Msg:" << message;
 *         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 *         return true;
 *     }
 * 
 *     void stat(adbase::kafka::Consumer* consumer, const std::string& stats) {
 *         (void)consumer;
 *         LOG_INFO << "Stats:" << stats.substr(0, 1024);
 *     }
 * };
 * 
 * // }}}
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (gEventLoop != nullptr) {
 *         gEventLoop->stop();
 *         delete gEventLoop;
 *     }
 *     if (gConsumer != nullptr) {
 *         gConsumer->stop();
 *         delete gConsumer;
 *     }
 *     exit(0);
 * }
 * 
 * // }}}
 * // {{{ static void reloadConf()
 * 
 * static void reloadConf(const int sig) {
 *     (void)sig;
 * }
 * 
 * // }}}
 * // {{{ static void registerSignal()
 * 
 * static void registerSignal() {
 *     // 忽略Broken Pipe信号 
 *     signal(SIGPIPE, SIG_IGN);
 *     /// 处理kill信号 
 *     signal(SIGINT,  killSignal);
 *     signal(SIGKILL, killSignal);
 *     signal(SIGQUIT, killSignal);
 *     signal(SIGTERM, killSignal);
 *     signal(SIGHUP,  killSignal);
 *     signal(SIGSEGV, killSignal);
 *     signal(SIGUSR1, reloadConf);
 * }
 * 
 * // }}}
 * 
 * int main(void) {
 *     registerSignal();
 *     adbase::TimeZone tz(8*3600, "CST");
 *     adbase::Logger::setTimeZone(tz);
 *     gEventLoop = new adbase::EventLoop();
 * 
 *     PullMessage pullMessage;
 *     gConsumer = new adbase::kafka::Consumer("test", "testgroupid", "10.13.4.159:9192");
 *     gConsumer->setMessageHandler(std::bind(&PullMessage::pull, &pullMessage, std::placeholders::_1,
 *                                            std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
 *     gConsumer->setStatCallback(std::bind(&PullMessage::stat, &pullMessage, std::placeholders::_1,
 *                                          std::placeholders::_2));
 *     gConsumer->setKafkaStatInterval("1000");
 *     gConsumer->setKafkaDebug("msg");
 *     gConsumer->setIsNewConsumer(true);
 *     gConsumer->setOffsetStoreMethod("broker");
 *     gConsumer->start();
 * 
 *     gEventLoop->start();
 * 
 *     return 0;
 * }
 * 
 * @endcode
 */
/*@{*/ 

#include <adbase/Kafka/Producer.hpp>
#include <adbase/Kafka/Consumer.hpp>

/*@}*/ 

#endif
