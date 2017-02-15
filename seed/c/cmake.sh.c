#!/bin/bash
#{{{ options

echo "Start cmake configure....."
echo -e "\033[31m\033[05m编译 Debug 级别 [Debug(D)|Release(R)]: \033[0m\c"
read makelevel
case $makelevel in
	Debug | D )
		CMAKE_BUILD_TYPE="Debug";;
	Release | R )
		CMAKE_BUILD_TYPE="Release";;
	*)
		CMAKE_BUILD_TYPE="Debug";;
esac
# }}}
# {{{ function opversion()

############################
# 操作系统版本 #############
############################
function opversion()
{
    MAIN_VERSION=`lsb_release -a |grep 'Release' | cut -f2 | cut -d '.' -f 1`
    SUB_VERSION=`lsb_release -a |grep 'Release' | cut -f2 | cut -d '.' -f 2`
    case $1 in
        "main")
            echo $MAIN_VERSION
            ;;
        "sub")
            echo $SUB_VERSION
            ;;
        "int")
            echo ${MAIN_VERSION}${SUB_VERSION}
            ;;
        *)
            echo ${MAIN_VERSION}.${SUB_VERSION}
            ;;
    esac
}

# }}}

# 计算版本号
LAST_TAG=`git tag|tail -n 1`
VERSION=${LAST_TAG/v/}

GIT_SHA1=`(git show-ref --head --hash=8 2> /dev/null || echo 00000000) | head -n1`
GIT_DIRTY=`git diff --no-ext-diff 2> /dev/null | wc -l`
BUILD_ID=`uname -n`"-"`date +%s`
OPVERSION="el"`opversion`

CMD="cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DADINFVERSION=$VERSION -DGIT_SHA1=$GIT_SHA1 -DGIT_DIRTY=$GIT_DIRTY -DBUILD_ID=$BUILD_ID  -DOPVERSION=$OPVERSION .."

if [ -d ./build ]; then
	echo "mkdir build.";
else
	mkdir ./build;
fi
cd ./build;
echo $CMD
eval $CMD
