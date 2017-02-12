//
//  VCTDownloadThread.h
//  cocos2d-lua-proj
//
//  Created by Vincent Yao on 2017/2/12.
//
//

#ifndef VCTDownloadThread_h
#define VCTDownloadThread_h

#include "VCTDownload.h"

namespace VCT {
    
class DownloadThread
{
public:
    DownloadThread(std::string downloadPath, std::string storagePath);
    ~DownloadThread();
    void start();
    void stop();
    void onProgressing(std::function<void(float, float, float)> progressingFunc);
    void onSuccess(std::function<void(std::string)> successFunc);
    void onError(std::function<void(int, std::string)> errorFunc);
    
private:
    void run();
    std::thread *_thread;
    Download _download;
};

}

#endif /* VCTDownloadThread_h */
