## Updater模块
#### 简介
脚本游戏真机运行时可以不重新编译   
实时保证真机运行代码和本地同步  
#### 使用说明
* 将zipdiff.sh放入在svn或git管理下的游戏目录中
* 修改zipdiff.sh, 将ZIPFILE修改为一个Web服务器的地址
* 修改Demo中的main.cpp，将downloadPath改为上面的Web服务器地址
* 执行./zipdiff.sh（可指定对比哪次commit，默认对比head）
* 上述操作会对比本地和git上的代码并将差异打包成一个update.zip
* 游戏运行时会自动下载解压该zip，故每次运行游戏都是最新的代码，实现了热更新
#### 引用
* minizip
* libcurl
* libz

## Logger模块
	TODO  