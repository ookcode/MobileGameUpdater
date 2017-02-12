//
//  VCTUtils.h
//  cocos2d-lua-proj
//
//  Created by Vincent Yao on 2017/2/12.
//
//

#ifndef VCTUtils_h
#define VCTUtils_h
#include <string>
namespace VCT {
    
class Utils {
public:
    static bool unzip(std::string path);
    static bool isDirectoryExist(std::string dirPath);
    static bool createDirectory(std::string path);
};

}
#endif /* VCTUtils_h */
