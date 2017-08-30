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
# }}}
# 计算版本号
GIT_SHA1=`(git show-ref --head --hash=8 2> /dev/null || echo 00000000) | head -n1`
GIT_DIRTY=`git diff --no-ext-diff 2> /dev/null | wc -l`
BUILD_ID=`uname -n`"-"`date +%s`

CMD="cmake -H. -B_build -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DGIT_SHA1=$GIT_SHA1 -DGIT_DIRTY=$GIT_DIRTY -DBUILD_ID=$BUILD_ID -DSTATICCOMPILE=ON  "

echo $CMD
eval $CMD
