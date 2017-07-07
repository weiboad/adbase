#!/bin/bash


#{{{ options
echo "Start cmake configure....."
echo -e "\033[31m\033[05mCompile debug level [Debug(D)|Release(R)]: \033[0m\c"
read makelevel
case $makelevel in
	Debug | D )
		CMAKE_BUILD_TYPE="Debug";;
	Release | R )
		CMAKE_BUILD_TYPE="Release";;
	*)
		CMAKE_BUILD_TYPE="Debug";;
esac
echo -e "\033[31m\033[05mWith adbase_kafka module [Y|N]: \033[0m\c"
read usekafka
case $usekafka in
	Y | y )
		USEKAFKA=ON;;
	N | n )
		USEKAFKA=0;;
	*)
		USEKAFKA=ON;;
esac
echo -e "\033[31m\033[05mWith adbase_lua module [Y|N]: \033[0m\c"
read uselua
case $uselua in
	Y | y )
		USELUA=ON;;
	N | n )
		USELUA=0;;
	*)
		USELUA=0;;
esac

# }}}

CMD="cmake -H. -B_build -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DINSTALL_KAFKA=$USEKAFKA  -DINSTALL_LUA=$USELUA -DCMAKE_CXX_COMPILER=g++ "

echo $CMD
eval $CMD
