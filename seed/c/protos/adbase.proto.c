//@IF @grpc
syntax = "proto3";

option java_multiple_files = true;
option java_package = "com.weibo.ad.@ADINF_PROJECT_NAME@";
option java_outer_classname = "@ADINF_PROJECT_NAME|ucfirst@";
option objc_class_prefix = "HLW";

package @ADINF_PROJECT_NAME@;

service Server {
	rpc status (StatusRequest) returns (StatusReply) {}
}

message StatusRequest {
	string name = 1;
}
message StatusReply {
	string status = 1;
}
//@ENDIF
