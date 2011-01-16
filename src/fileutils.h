//******************************************************************************
//    Copyright (c) 2010, Christopher James Huff
//    All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//******************************************************************************

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <dirent.h>
#include <string>
#include <set>
#include <stack>
#include <vector>
#include <sys/stat.h>


// Accumulate paths of files in directory into filepaths. Does not clear filepaths,
// and can be used to gather files from multiple directories.
inline void GetFilePaths(std::string basepath,
    const std::set<std::string> & exclude,
    std::vector<std::string> & filepaths)
{
    std::stack<std::string> dirpaths;
    dirpaths.push(basepath);
    
    while(!dirpaths.empty())
    {
        basepath = dirpaths.top();
        dirpaths.pop();
        
        DIR * dir = opendir(basepath.c_str());
        if(!dir) {
//            std::cerr << "Could not read directory: " << basepath << std::endl;
            continue;
        }
        
        dirent * ent = readdir(dir);
        while(ent)
        {
            if(strcmp(ent->d_name, ".") != 0 &&
               strcmp(ent->d_name, "..") &&
               exclude.find(ent->d_name) == exclude.end())
            {
                if(ent->d_type == DT_DIR)
                    dirpaths.push(basepath + "/" + ent->d_name);
                else 
                    filepaths.push_back(basepath + "/" + ent->d_name);
            }
            ent = readdir(dir);
        }
        closedir(dir);
    }
}

inline int MakeDir(const std::string & path) {
    return mkdir(path.c_str(), 0777);
}

inline bool FileExists(const std::string & path) {
    struct stat statbuf;
    int result = stat(path.c_str(), &statbuf);
    return result == 0;
}

inline bool DirExists(const std::string & path) {
    if(!FileExists(path))
        return false;
    
    // Might be a better way to do this...
    DIR * dir = opendir(path.c_str());
    if(!dir)
        return false;
    closedir(dir);
    return true;
}

inline void GetFilePaths(const std::string & basepath,
    std::vector<std::string> & filepaths)
{
    std::set<std::string> exclude;
    GetFilePaths(basepath, exclude, filepaths);
}

#endif // FILEUTILS_H
