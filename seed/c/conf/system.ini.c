[system]
appid=0
macid=0
daemon=no
pidFile=./@ADINF_PROJECT_NAME@.pid

//@IF @logging
[logging]
logsDir=logs/
; 日志分卷大小
logRollSize=52428800
; 1: LOG_TRACE 2: LOG_DEBUG 3: LOG_INFO 
logLevel=1
isAsync=yes

//@ENDIF
//@IF @adserver
[adserver]
; 是否启动 mc server
mc=yes
; 是否启动 http server
http=yes
; 是否启动 head server
//@IF @grpc
grpc=yes
//@ENDIF

[mc]
host=0.0.0.0
port=10011
threadNum=4
serverName=mc-server

[http]
host=0.0.0.0
port=10010
timeout=3
threadNum=4
serverName=adinf-adserver
accessLogDir=logs/
accesslogRollSize=52428800
defaultController=index
defaultAction=index

//@IF @grpc
[grpc]
host=0.0.0.0
port=10012
//@ENDIF
//@ENDIF
//@IF @kafkac
//@FOR @kafka_consumers,@kafka_consumers_topics,@kafka_consumers_groups
[kafkac_@REPLACE0|lower@]
;是否是 v0.9.x 新协议获取
isNewConsumer@REPLACE0@=yes
topicName@REPLACE0@=@REPLACE1@
groupId@REPLACE0@=@REPLACE2@
brokerList@REPLACE0@=127.0.0.1:9092
kafkaDebug@REPLACE0@=none
offsetPath@REPLACE0@=./
statInterval@REPLACE0@=60000
//@ENDFOR
//@ENDIF
//@IF @kafkap
//@FOR @kafka_producers,@kafka_producers_topics
[kafkap_@REPLACE0|lower@]
topicName@REPLACE0@=@REPLACE1@
brokerList@REPLACE0@=127.0.0.1:9092
debug@REPLACE0@=none
queueLength@REPLACE0@=10000
//@ENDFOR
//@ENDIF

//@IF @timer
[timer]
; 单位ms
//@FOR @timers
interval@REPLACE0|ucfirst@ = 2000
//@ENDFOR
//@ENDIF
