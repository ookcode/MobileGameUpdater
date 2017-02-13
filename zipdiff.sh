#!/bin/sh
PROJECT_ROOT=$(pwd) # 请将本脚本放入git or svn管理的工程目录
ZIPFILE=~/Documents/webserver/update.zip # TODO:填入你的Web服务器地址
COMPARE_VERSION=HEAD
if [ $# == 1 ]; then
    COMPARE_VERSION=$1
fi
echo "adding files to zip file: $ZIPFILE, compare with $COMPARE_VERSION"
[ -f $ZIPFILE ] && rm $ZIPFILE
cd $PROJECT_ROOT
if [ -d .git ]; then
    git diff --name-only $COMPARE_VERSION . | awk '{ print $1 }' | xargs zip $ZIPFILE
elif [ -d .svn ]; then
    svn status . | awk '{ print $2 }' | xargs zip $ZIPFILE
else
    echo "no version control system"
fi