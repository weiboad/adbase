#include <sys/time.h>
#include <adbase/Kafka.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Metrics.hpp>

namespace adbase {
namespace kafka {
// {{{ Consumer::Consumer()

Consumer::Consumer(const std::string& topicName, const std::string& groupId,
                             const std::string& brokerList) :
    _topic(topicName),
    _groupId(groupId),
    _brokerList(brokerList),
    _kafkaCommitInterval("1000"),
    _kafkaQueuedMinMessages("1000"),
    _kafkaQueuedMaxSize("10240"),
    _kafkaStatInterval("1000"),
    _kafkaDebug("all"),
    _consumeTimeout(10),
    _isRuning(false) {
}

// }}}
// {{{ Consumer::~Consumer()

Consumer::~Consumer() {
}

// }}}
// {{{ void Consumer::stop()

void Consumer::stop() {
    _isRuning = false;
    int tryNum = 10;
    while (!_isClose && tryNum-- > 0) {
        LOG_ERROR << "Stopping consumer.";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// }}}
// {{{ void Consumer::start()

void Consumer::start() {
    if (!init()) {
        return;
    }
    _isRuning = true;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ThreadPtr Thread(new std::thread(std::bind(&Consumer::threadFunc, this, std::placeholders::_1), nullptr), &Consumer::deleteThread);
    Threads.push_back(std::move(Thread));
}

// }}}
// {{{ void Consumer::threadFunc()

void Consumer::threadFunc(void *data) {
    (void)data;
    rd_kafka_resp_err_t err;

	std::string metricName = "consumer." + _topic + "." + _groupId;
	adbase::metrics::Meters* consumerMeter = adbase::metrics::Metrics::buildMeters("adbase.kafkac", metricName);
    while (_isRuning) {
        rd_kafka_message_t *rkmessage;
		bool ret;
        if (_isNewConsumer) {
            rkmessage = rd_kafka_consumer_poll(_kt, _consumeTimeout);
			if (_isPauseConsumer) {
				Buffer message;
				ret = _messageCallback(_topic, 0, 0, message);
				if (ret) { // 挂起并且 callback 返回 true 解除挂起
					rd_kafka_topic_partition_list_t *parts;
					if ((err = rd_kafka_assignment(_kt, &parts))) {
						LOG_ERROR << "Failed to assignment:" << rd_kafka_err2str(err);
					}
					if ((err = rd_kafka_resume_partitions(_kt, parts))) {
						LOG_ERROR << "Failed to pause consuming topics:" << rd_kafka_err2str(err);
					}
					rd_kafka_topic_partition_list_destroy(parts);
					_isPauseConsumer = false;
				}
			}
        } else {
            rkmessage = rd_kafka_consume_queue(_kqueue.get(), _consumeTimeout);
        }
        if (!_isNewConsumer) {
            // poll 获取统计信息
            rd_kafka_poll(_kt, _consumeTimeout);
        }
        if (!rkmessage) {
			//LOG_TRACE << "Consumer timeout " << _topic << "[" << _consumeTimeout << "]";
            continue;
        }

        if (rkmessage->err) {
            if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
                LOG_TRACE << "Consumer reached end of " << rd_kafka_topic_name(rkmessage->rkt)
                          << " [" << rkmessage->partition << "] message queue at offset "
                          << rkmessage->offset;
            } else {
                LOG_ERROR << "Consumer error for topic \"" << rd_kafka_topic_name(rkmessage->rkt)
                          <<  "\" [" << rkmessage->partition << "] offset "
                          << rkmessage->offset << ":"
                          << rd_kafka_message_errstr(rkmessage);
            }
            rd_kafka_message_destroy(rkmessage);
            continue;
        }

        if (rkmessage->len > 0 && rkmessage->payload != nullptr) {
			Buffer message;
			message.append(static_cast<const char *>(rkmessage->payload), rkmessage->len);
			if (consumerMeter != nullptr) {
				consumerMeter->mark();
			}
			if (!_isNewConsumer) {
                int partition = rkmessage->partition;
                uint64_t offset = rkmessage->offset;
                do {
                    ret = _messageCallback(_topic, partition, offset, message);
                    if (!ret) {
                        partition = 0;
                        offset = 0;
                        message.retrieveAll();
					    // poll 获取统计信息
                        rd_kafka_poll(_kt, _consumeTimeout);
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                } while(!ret);
			} else {
				ret = _messageCallback(_topic, rkmessage->partition, rkmessage->offset, message);
				if (!_isPauseConsumer && !ret) {
					rd_kafka_topic_partition_list_t *parts;
					if ((err = rd_kafka_assignment(_kt, &parts))) {
						LOG_ERROR << "Failed to assignment:" << rd_kafka_err2str(err);
					}
					if ((err = rd_kafka_pause_partitions(_kt, parts))) {
						LOG_ERROR << "Failed to pause consuming topics:" << rd_kafka_err2str(err);
					}
					rd_kafka_topic_partition_list_destroy(parts);
					_isPauseConsumer = true;
				}
			}
        }

        rd_kafka_message_destroy(rkmessage);
    }

    if (_isNewConsumer) {
        if ((err = rd_kafka_consumer_close(_kt)))    {
            LOG_ERROR << "Failed to close consumer:" << rd_kafka_err2str(err);
        }
        rd_kafka_topic_partition_list_destroy(_topics);
    } else {
		int partNum = getTopicPartitionNum();
        for (int i = 0; i < partNum; i++) {
            rd_kafka_consume_stop(_ktopic.get(), i);
            LOG_INFO << "Stop topic " << _topic << ", group " << _groupId << ", partId "
                << i << " consumer.";
        }
    }

	LOG_INFO << "Start destroy kt.";
	rd_kafka_destroy(_kt);
	LOG_INFO << "Destroy kt.";
    int waitRunNumber = 3;
    while (waitRunNumber-- > 0 && rd_kafka_wait_destroyed(1000) == -1) {
        LOG_ERROR << "Waiting for librdkafka to decommission";
    }
    _isClose = true;
}

// }}}
// {{{ bool Consumer::init()

bool Consumer::init() {
	_kconf = rd_kafka_conf_new();
    rd_kafka_conf_set_log_cb(_kconf, &Consumer::logger);
	_ktconf = rd_kafka_topic_conf_new();
    // 初始化配置
    initConf();

    _kt = rd_kafka_new(RD_KAFKA_CONSUMER, _kconf, const_cast<char *>(_error.c_str()), _error.size());
    if (_kt == nullptr) {
        LOG_ERROR << "Create kafka instance error, " << _error;
        return false;
    }
    rd_kafka_set_log_level(_kt, 7);
    if (_isNewConsumer) {
        rd_kafka_poll_set_consumer(_kt);
    }
    if (rd_kafka_brokers_add(_kt, _brokerList.c_str()) < 1) {
        LOG_ERROR << "No valid brokers specified";
        return false;
    }

    if (!_isNewConsumer) {
        Ktopic topic(rd_kafka_topic_new(_kt, _topic.c_str(), _ktconf), &rd_kafka_topic_destroy);
        _ktopic = std::move(topic);
        Kqueue queue(rd_kafka_queue_new(_kt), &rd_kafka_queue_destroy);
        _kqueue = std::move(queue);
    }

    rd_kafka_resp_err_t err;
    int partNum = 0;
    if (_isNewConsumer) {
		_topics = rd_kafka_topic_partition_list_new(partNum);
		rd_kafka_topic_partition_list_add(_topics, _topic.c_str(), -1);
        if ((err = rd_kafka_subscribe(_kt, _topics))) {
            LOG_ERROR << "Failed to start consuming topics:" << rd_kafka_err2str(err);
        }
    } else {
        partNum = getTopicPartitionNum();
        LOG_INFO << "Start consumer topic: " << _topic <<  ", groupId: " << _groupId << " partNum: " << partNum;
        for (int i = 0; i < partNum; i++) {
            if (rd_kafka_consume_start_queue(_ktopic.get(), i, RD_KAFKA_OFFSET_STORED, _kqueue.get()) == -1) {
                LOG_ERROR << "Start topic " << _topic << " group " << _groupId << " consumer fail.";
            }
        }
    }

    return true;
}

// }}}
// {{{ void Consumer::initConf()

void Consumer::initConf() {
    rd_kafka_conf_set(_kconf, "group.id", _groupId.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_topic_conf_set(_ktconf, "auto.commit.enabled", "true", const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_topic_conf_set(_ktconf, "offset.store.path", _offsetStorePath.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_topic_conf_set(_ktconf, "auto.offset.reset", _offsetReset.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_topic_conf_set(_ktconf, "offset.store.method", _offsetStoreMethod.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_topic_conf_set(_ktconf, "auto.commit.interval.ms", _kafkaCommitInterval.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_conf_set(_kconf, "queued.min.messages", _kafkaQueuedMinMessages.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_conf_set(_kconf, "queued.max.messages.kbytes", _kafkaQueuedMaxSize.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_conf_set(_kconf, "debug", _kafkaDebug.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_conf_set(_kconf, "statistics.interval.ms", _kafkaStatInterval.c_str(), const_cast<char *>(_error.c_str()), _error.size());
    rd_kafka_conf_set_stats_cb(_kconf, &Consumer::statsCallback);
    rd_kafka_conf_set_rebalance_cb(_kconf, &Consumer::rebalanceCallback);
    rd_kafka_conf_set_opaque(_kconf, this);

	if (_isNewConsumer) {
        rd_kafka_conf_set_default_topic_conf(_kconf, _ktconf);
	}

    const char **arr;
    size_t cnt;
    arr = rd_kafka_conf_dump(_kconf, &cnt);
    for (uint32_t i = 0 ; i < static_cast<uint32_t>(cnt) ; i += 2) {
        LOG_ERROR << arr[i] << "===" << arr[i+1];
    }
	rd_kafka_conf_dump_free(arr, cnt);
}

// }}}
// {{{ void Consumer::deleteThread()

void Consumer::deleteThread(std::thread *t) {
    t->join();
	LOG_ERROR << "Consumer main thread stop.";
	delete t;
}

// }}}
// {{{ void Consumer::logger()

void Consumer::logger(const rd_kafka_t *rk, int level, const char *fac, const char *buf) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (level == 7) {
        LOG_DEBUG << static_cast<uint32_t>(tv.tv_sec) << "." << static_cast<uint32_t>((tv.tv_usec / 1000))
                  << " RDKAFKA-" << level << "-" << fac << ":"
                  << rd_kafka_name(rk) << ":" << buf;
    } else if (level == 6) {
        LOG_INFO << static_cast<uint32_t>(tv.tv_sec) << "." << static_cast<uint32_t>((tv.tv_usec / 1000))
                  << " RDKAFKA-" << level << "-" << fac << ":"
                  << rd_kafka_name(rk) << ":" << buf;
    } else {
        LOG_ERROR << static_cast<uint32_t>(tv.tv_sec) << "." << static_cast<uint32_t>((tv.tv_usec / 1000))
                  << " RDKAFKA-" << level << "-" << fac << ":"
                  << rd_kafka_name(rk) << ":" << buf;
    }
}

// }}}
// {{{ void Consumer::statsCallback()

void Consumer::statCallback(Consumer* consumer, const std::string& stat) {
    _statCallback(consumer, stat);
}

// }}}
// {{{ int Consumer::statsCallback()

int Consumer::statsCallback(rd_kafka_t *rk, char *json, size_t jsonLen, void *opaque) {
    (void)rk;
    Consumer* consumer = reinterpret_cast<Consumer*>(opaque);
    std::string stat(json, jsonLen);
    consumer->statCallback(consumer, stat);
    return 0; // If the application returns 0 from the `stats_cb` then librdkafka will immediately free the 'json' pointer.
}

// }}}
// {{{ void Consumer::rebalanceCallback()

void Consumer::rebalanceCallback(rd_kafka_t *rk, rd_kafka_resp_err_t err,
                                      rd_kafka_topic_partition_list_t *partitions,
                                      void *opaque) {
    (void)opaque;
    LOG_INFO << "Consumer group rebalanced";
    switch (err) {
        case RD_KAFKA_RESP_ERR__ASSIGN_PARTITIONS:
            rd_kafka_assign(rk, partitions);
            break;
        case RD_KAFKA_RESP_ERR__REVOKE_PARTITIONS:
            rd_kafka_assign(rk, nullptr);
            break;
        default:
            LOG_ERROR << "rebalanced fail: " << rd_kafka_err2str(err);
			rd_kafka_assign(rk, nullptr);
            break;
    }
}

// }}}
// {{{ int Consumer::getTopicPartitionNum()

int Consumer::getTopicPartitionNum() {
    int num = 0;
    rd_kafka_resp_err_t err = RD_KAFKA_RESP_ERR_NO_ERROR;
    const struct rd_kafka_metadata *metadata;
    while (true) {
        err = rd_kafka_metadata(_kt, 0, _ktopic.get(), &metadata, 5000);
        if (err != RD_KAFKA_RESP_ERR_NO_ERROR) {
            LOG_ERROR << "Failed to acquire metadata: " <<  rd_kafka_err2str(err);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        if (metadata->topic_cnt != 1) {
            LOG_ERROR << "Failed to acquire metadata topic num invalid: " << metadata->topic_cnt;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        const struct rd_kafka_metadata_topic *t = &metadata->topics[0];
        num = t->partition_cnt;
        rd_kafka_metadata_destroy(metadata);
        break;
    }

    return num;
}

// }}}
}
}
