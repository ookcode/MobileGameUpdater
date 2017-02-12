//
//  VCTUtils.cpp
//  cocos2d-lua-proj
//
//  Created by Vincent Yao on 2017/2/12.
//
//

#include "VCTUtils.h"
#include <assert.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <errno.h>
#include <vector>
#include "unzip/unzip.h"

namespace VCT {
    
#define BUFFER_SIZE 8192 // buffer最大大小
#define MAX_FILENAME 512 // 文件名最大长度

bool Utils::unzip(std::string path)
{
    // 打开压缩文件
    std::string outFileName = path;
    unzFile zipfile = unzOpen(path.c_str());
    if (!zipfile) {
        printf("can not open %s\n", path.c_str());
        return false;
    }
    
    // 获取文件信息
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK) {
        printf("can not read file global info %s\n", path.c_str());
        unzClose(zipfile);
        return false;
    }
    
    // 开始解压
    char readBuffer[BUFFER_SIZE];
    printf("start unzip %s\n", path.c_str());
    
    uLong i;
    for (i = 0; i < global_info.number_entry; ++i) {
        unz_file_info fileInfo;
        char fileName[MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile, &fileInfo, fileName, MAX_FILENAME, nullptr, 0, nullptr, 0) != UNZ_OK) {
            printf("can not read file info\n");
            unzClose(zipfile);
            return false;
        }
        
        const std::string fullPath = path + fileName;
        const size_t filenameLength = strlen(fileName);
        if (fileName[filenameLength - 1] == '/') {
            // 创建目录
            if (!createDirectory(fullPath)) {
                printf("can not create directory %s\n", fullPath.c_str());
                unzClose(zipfile);
                return false;
            }
        } else {
            const std::string fileNameStr(fileName);
            size_t startIndex = 0;
            size_t index=fileNameStr.find("/", startIndex);
            
            while(index != std::string::npos) {
                const std::string dir = path + fileNameStr.substr(0, index);
                FILE *out = fopen(dir.c_str(), "r");
                if(!out) {
                    // 创建目录
                    if (!createDirectory(dir)) {
                        printf("can not create directory %s\n", dir.c_str());
                        unzClose(zipfile);
                        return false;
                    } else {
                        printf("create directory %s\n", dir.c_str());
                    }
                } else {
                    fclose(out);
                }
                
                startIndex = index+1;
                index = fileNameStr.find("/", startIndex);
            }
            
            // 解压文件
            if (unzOpenCurrentFile(zipfile) != UNZ_OK)
            {
                printf("can not open file %s\n", fileName);
                unzClose(zipfile);
                return false;
            }
            
            FILE *out = fopen(fullPath.c_str(), "wb");
            if (!out) {
                printf("can not open destination file %s\n", fullPath.c_str());
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }
            
            int error = UNZ_OK;
            do {
                error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
                if (error < 0) {
                    printf("can not read zip file %s, error code is %d\n", fileName, error);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    return false;
                }
                
                if (error > 0) {
                    fwrite(readBuffer, error, 1, out);
                }
            } while(error > 0);
            fclose(out);
        }
        unzCloseCurrentFile(zipfile);
        // 判断下一项是否存在
        if ((i + 1) < global_info.number_entry)
        {
            if (unzGoToNextFile(zipfile) != UNZ_OK)
            {
                printf("can not read next file\n");
                unzClose(zipfile);
                return false;
            }
        }
    }
    
    printf("end unzip\n");
    unzClose(zipfile);
    
    return true;
}

bool Utils::isDirectoryExist(std::string dirPath)
{
    assert(!dirPath.empty());
    struct stat st;
    if (stat(dirPath.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool Utils::createDirectory(std::string path)
{
    assert(!path.empty());
    
    if (isDirectoryExist(path)) {
        return true;
    }
    
    // 分割路径
    size_t start = 0;
    size_t found = path.find_first_of("/\\", start);
    std::string subpath;
    std::vector<std::string> dirs;
    
    if (found != std::string::npos) {
        while (true) {
            subpath = path.substr(start, found - start + 1);
            if (!subpath.empty()) {
                dirs.push_back(subpath);
            }
            start = found + 1;
            found = path.find_first_of("/\\", start);
            if (found == std::string::npos) {
                if (start < path.length()) {
                    dirs.push_back(path.substr(start));
                }
                break;
            }
        }
    }
    
    DIR *dir = NULL;
    
    // 循环创建目录
    subpath = "";
    for (int i = 0; i < dirs.size(); ++i) {
        subpath += dirs[i];
        dir = opendir(subpath.c_str());
        
        if (!dir) {
            int ret = mkdir(subpath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            if (ret != 0 && (errno != EEXIST)) {
                // 无法创建目录
                return false;
            }
        } else {
            closedir(dir);
        }
    }
    return true;
}

}
