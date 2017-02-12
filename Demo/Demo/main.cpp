//
//  main.cpp
//  Demo
//
//  Created by Vincent Yao on 2017/2/12.
//  Copyright © 2017年 ookcode. All rights reserved.
//

#include <iostream>
#include "VCTDownloadThread.h"

int main(int argc, const char * argv[]) {
    // 下载路径
    std::string downloadPath = "http://127.0.0.1:8080/update.zip";
    // TODO:替换为自己的存储目录
    std::string storagePath = "/Users/vincent/Desktop/update.zip";
    auto thread = new VCT::DownloadThread(downloadPath, storagePath);
    thread->start();
    thread->onSuccess([](std::string url) {
        // TODO:此处开始运行游戏脚本
    });
    thread->onError([](int errorCode, std::string url) {
        printf("thread over, download %s fail, error code = %d\n", url.c_str(), errorCode);
    });
    while (true) {
        // 阻塞
    }
    return 0;
}
