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

//@IF @kafkap || @kafkac
[kafka]
brokerList=127.0.0.1:9092
debug=none
statInterval=60000
//@IF @kafkap
queueLength=10000
//@ENDIF
//@IF @kafkac
topicName=test
groupId=test
autoCommitEnabled=true
offsetReset=largest
consumeCallbackMaxMessages=0
commitInterval=1000
enabledEvents=10000
queuedMinMessages=1000
queuedMaxSize=10240
//@ENDIF
securityProtocol=plaintext
saslMechanisms=PLAIN
saslUsername=
saslPassword=
kerberosServiceName=
kerberosPrincipal=
kerberosCmd=
kerberosKeytab=
kerberosMinTime=
//@ENDIF

//@IF @timer
[timer]
; 单位ms
//@FOR @timers
interval@REPLACE0|ucfirst@ = 2000
//@ENDFOR
//@ENDIF
