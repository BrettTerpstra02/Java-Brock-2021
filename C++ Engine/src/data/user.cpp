//
// Credit:
// https://github.com/tanpero/getUserName
//
/**
MIT License

Copyright (c) 2019 tanpero

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#include <data/user.h>

#include <iostream>
#include <string>

namespace TD {

    std::string getUserName() {
        #if defined linux
            uid_t userid;
            struct passwd *pwd;
            userid = getuid();
            pwd = getpwuid(userid);
            return pwd->pw_name;
        #elif defined _WIN32
            const int MAX_LEN = 100;
            char szBuffer[MAX_LEN];
            DWORD len = MAX_LEN;
            if (GetUserName(szBuffer, &len))
                return szBuffer;

        #else
            return "";
        #endif
    }

    std::string getUserHome() {
        #if defined linux
            const char* homedir;
            if ((homedir = getenv("HOME")) == nullptr) {
                homedir = getpwuid(getuid())->pw_dir;
            }
            return homedir;
        #elif defined _WIN32
            return "C:/Users/" + getUserName();
        #else
            return "";
        #endif
    }
}