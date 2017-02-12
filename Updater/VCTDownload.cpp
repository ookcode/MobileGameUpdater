//
//  VCTDownload.cpp
//  cocos2d-lua-proj
//
//  Created by Vincent Yao on 2017/1/24.
//
//

#include "VCTDownload.h"
#include "VCTUtils.h"

namespace VCT {

//网络访问错误标识码
enum  ErrorCode  {
    CURL_INIT_ERROR = 0,
    CREATE_FILE_ERROR = 1,
    NETWORK_ERROR = 2,
    UNCOMPRESS_ERROR = 3,
};
    
#define TIME_OUT 10 // 超时秒数
#define MAX_RETRY 5 // 最大重试次数
#define RETRY_SLEEP 1000 // 重试间隔，毫秒
    
Download::Download(std::string downloadPath, std::string storagePath)
:_storagePath(storagePath)
,_downloadPath(downloadPath)
{
}

Download::~Download()
{
    
}

size_t Download::dataReceiving(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    Download *_this = reinterpret_cast<Download *>(userdata);
    _this->_fileHandler.write(reinterpret_cast<const char *>(ptr), size * nmemb);
    return size * nmemb;
}

int Download::dataProgressing(void *userdata, double total, double curr, double totalUpload, double currUpload)
{
    Download *_this = reinterpret_cast<Download *>(userdata);
    
    // 获取下载速度
    double speed(0);
    if (nullptr != _this->_curl)
    {
        curl_easy_getinfo(_this->_curl, CURLINFO_SPEED_DOWNLOAD, &speed);
    }
    
    // 通知当前的进度
    if(total > 0) {
        printf("curl dataProgressing: %1.f KB/s, %1.f %%\n", speed / 1024, (curr / total) * 100);
        if(nullptr != _this->progressingFunc) {
            _this->progressingFunc(speed, curr, total); // byte
        }
    }
    if(_this->_stop) {
        // 中断下载
        return 1;
    } else {
        return 0;
    }
}

void Download::doRetry(int errorCode)
{
    if(_retryCount++ < MAX_RETRY) {
        printf("waiting for %d millisecond\n", RETRY_SLEEP);
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_SLEEP)); //休眠
        this->doDownload();
    } else {
        this->errorFunc(errorCode, _downloadPath);
    }
}

bool Download::doDownload()
{
    printf("start download %s\n", _downloadPath.c_str());
    _fileHandler.open(_storagePath, std::fstream::binary);
    
    _curl = curl_easy_init();
    if (_curl == nullptr) {
        printf("curl init fail");
        doRetry(ErrorCode::CURL_INIT_ERROR);
        return false;
    }

    curl_easy_setopt(_curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(_curl, CURLOPT_URL, _downloadPath.c_str());
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &Download::dataReceiving);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(_curl, CURLOPT_FAILONERROR, true);
    curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, &Download::dataProgressing);
    curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1);
    
    // 下载完成状态
    bool downloadSuccess = true;
    int errorCode = -1;
    
    // 开始下载
    CURLcode code = curl_easy_perform(_curl);
    if (code != CURLE_OK) {
        // failed to download
        downloadSuccess = false;
        printf("curl error: %s\n", curl_easy_strerror(code));
        errorCode = ErrorCode::NETWORK_ERROR;
    } else {
        // check http result?
        long httpCode;
        code = curl_easy_getinfo(_curl, CURLINFO_HTTP_CODE, &httpCode);
        if (code != CURLE_OK) {
            downloadSuccess = false;
            printf("curl cannot retrieve http: %s\n", curl_easy_strerror(code));
            errorCode = ErrorCode::NETWORK_ERROR;
        } else {
            if (httpCode != 200 && httpCode != 206) {
                downloadSuccess = false;
                printf("curl http error, code = %ld\n", httpCode);
                errorCode = ErrorCode::NETWORK_ERROR;
            }
        }
    }
    
    // 关闭curl
    curl_easy_cleanup(_curl);
    
    // 关闭文件
    _fileHandler.close();
    
    // 检查状态并且回调
    if (downloadSuccess) {
        printf("curl download success\n");
        if(Utils::unzip(_storagePath)) {
            successFunc(_downloadPath);
        }
        return true;
    } else {
        this->doRetry(errorCode);
    }
    return false;
}
    
}
