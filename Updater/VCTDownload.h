//
//  VCTDownload.h
//  cocos2d-lua-proj
//
//  Created by Vincent Yao on 2017/1/24.
//
//

#ifndef VCTDownload_h
#define VCTDownload_h
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <curl/easy.h>
#include <sys/stat.h>
#include <thread>
namespace VCT {

class Download
{
public:
    Download(std::string downloadPath, std::string storagePath);
    ~Download();
    bool doDownload();
    void setStopFlag() {
        _stop = true;
    }
    std::function<void(float, float, float)> progressingFunc;
    std::function<void(std::string)> successFunc;
    std::function<void(int, std::string)> errorFunc;
    
private:
    void doRetry(int errorCode);
    static size_t dataReceiving(void *ptr, size_t size, size_t nmemb, void *userdata);
    static int dataProgressing(void *userdata, double total, double curr, double totalUpload, double currUpload);
    std::string _storagePath;
    std::string _downloadPath;
    std::ofstream _fileHandler;
    CURL *_curl;
    bool _stop = false;
    int _retryCount = 0;
};

}
#endif /* VCTDownload_h */
