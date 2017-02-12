//
//  VCTDownloadThread.cpp
//  cocos2d-lua-proj
//
//  Created by Vincent Yao on 2017/2/12.
//
//

#include "VCTDownloadThread.h"

namespace VCT {

DownloadThread::DownloadThread(std::string downloadPath, std::string storagePath)
:_download(std::move(downloadPath), std::move(storagePath))
{
}
    
DownloadThread::~DownloadThread()
{
    
}
    
void DownloadThread::start()
{
    if(_thread) {
        this->stop();
    }
    _thread = new std::thread(std::bind(&DownloadThread::run, this));
}

void DownloadThread::stop()
{
    if(_thread) {
        _download.setStopFlag();
        _thread->join();
        delete _thread;
        _thread = nullptr;
    }
}

void DownloadThread::run()
{
    _download.doDownload();
}

void DownloadThread::onProgressing(std::function<void(float, float, float)> progressingFunc)
{
    _download.progressingFunc = progressingFunc;
}
    
void DownloadThread::onSuccess(std::function<void(std::string)> successFunc)
{
    _download.successFunc = successFunc;
}
    
void DownloadThread::onError(std::function<void(int, std::string)> errorFunc)
{
    _download.errorFunc = errorFunc;
}
    
}


