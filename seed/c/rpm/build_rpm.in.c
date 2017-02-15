#!/bin/sh
# vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker: #
# adbase-common 出包工具

############################
# 相关配置项   #############
############################
CUR_PWD=`pwd`

# 编译后的临时目录
PATH_DEV_SWAN=/usr/local/adinf/@ADINF_PROJECT_NAME@

# build rpm 包的目录
BUILD_ROOT="/usr/home/zhongxiu/rpmbuild"
SWANSOFT_NAME="@ADINF_PROJECT_NAME@"
SWANSOFT_VERSION="@ADINFVERSION@"
BUILD_TMP=/tmp
BUILD_BALL=$BUILD_TMP/$SWANSOFT_NAME-$SWANSOFT_VERSION

############################
#          函数        #####
############################

# {{{ check_return_exit()

check_return_exit ()
{
    if ! test 0 = $?; then
        echo -e "\n!!!!!!!!!! Error, this script will exit !!!!!!!!!!\n"
        exit 1
    fi
}

# }}}
# {{{ function sw_mkdir()

############################
# 创建目录 #################
############################
function sw_mkdir()
{
	echo "create $1 directory ...";
	if [ -d $1 ] ;then
		echo "directory $1 already exists. " 
	else
		mkdir -p $1
		check_return_exit
	fi
}

# }}} 

###############################
#copy opt , web code to /tmp        #
###############################
# {{{ function ad_copy_opt()

function ad_copy_opt()
{
	echo "copy opt, to $BUILD_BALL ..."
	echo "cp -r $PATH_DEV_SWAN/* $BUILD_BALL/ ..."
	sw_mkdir $BUILD_BALL/
	cp -r $PATH_DEV_SWAN/* $BUILD_BALL/
	check_return_exit
}

# }}}
###############################
# 制作tar包                   #
###############################
# {{{ function ad_make_tar()

function ad_make_tar()
{
	echo "start make smeta tar $BUILD_BALL.tar ..."
	echo "cd $BUILD_TMP ..."
	cd $BUILD_TMP
	CMD="tar -zcf $BUILD_BALL.tar $SWANSOFT_NAME-$SWANSOFT_VERSION"
	echo $CMD
	eval $CMD
	check_return_exit

	echo "remove dir $BUILD_BALL ..."
	rm -rf $BUILD_BALL
	check_return_exit
}

# }}}
###############################
# 建立打包环境                #
###############################
# {{{ function ad_set_rpm_env()

function ad_set_rpm_env()
{
	echo "cd current $CUR_PWD ..."
	cd $CUR_PWD
	check_return_exit

	echo "mkdir rpmbuild ..."
	mkdir -p $BUILD_ROOT
	mkdir -p $BUILD_ROOT/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
	check_return_exit

	echo "cd current $CUR_PWD ..."
	cd $CUR_PWD
	check_return_exit

	echo "mv spec to buildroot ..."
	cp $SWANSOFT_NAME.spec  $BUILD_ROOT/SPECS
	check_return_exit

	echo "mv tar file to SOURCES ..."
	mv $BUILD_BALL.tar $BUILD_ROOT/SOURCES
	check_return_exit
}

# }}}
###############################
# 执行打包                    #
###############################
# {{{ function ad_rpmbuild()

function ad_rpmbuild()
{
	echo "start rpmbuild ... $BUILD_ROOT"
	rpmbuild -bb $BUILD_ROOT/SPECS/$SWANSOFT_NAME.spec
	check_return_exit
}

# }}}
###############################
# 预处理                      #
###############################
# {{{ function ad_pre_install()

function ad_pre_install()
{
	if [ -d $BUILD_ROOT ]; then
		echo "remove $BUILD_ROOT ..."
		rm -rf $BUILD_ROOT
		check_return_exit
	fi
	if [ -d $BUILD_TMP/$SWANSOFT_NAME-$SWANSOFT_VERSION ]; then
		echo "remove /tmp/$SWANSOFT_NAME-$SWANSOFT_VERSION ..."
		rm -rf $BUILD_TMP/$SWANSOFT_NAME-$SWANSOFT_VERSION
		check_return_exit
	fi
}

# }}}

ad_pre_install

###############################
# 打包全过程                  #
###############################

ad_copy_opt
ad_make_tar
ad_set_rpm_env
ad_rpmbuild
