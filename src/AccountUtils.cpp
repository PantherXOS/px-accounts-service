//
// Created by Reza Alizadeh Majd on 11/6/18.
//

#include "AccountUtils.h"

#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <wordexp.h>
#include <dirent.h>
#include <stdarg.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

string PXUTILS::ACCOUNT::title2name(const string &title) {
    string result = title;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        if (c == ' ') {
            return (int) '_';
        }
        return std::tolower(c);
    });
    return result;
}

string PXUTILS::PLUGIN::package2module(const string &title) {
    string result = title;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        if (c == '-') {
            return (int) '_';
        }
        return std::tolower(c);
    });
    return result;
}

string PXUTILS::FILE::abspath(const string &path) {
    string res;
    wordexp_t w;
    if (wordexp(path.c_str(), &w, 0) == 0) {
        res = string(*w.we_wordv);
        wordfree(&w);
    }
    return res;
}

string PXUTILS::FILE::basedir(const string &path) {
    string dir;
    size_t lastpos = path.rfind('/');
    if (lastpos != std::string::npos) {
        dir = path.substr(0, lastpos);
    }
    return dir;
}

string PXUTILS::FILE::filename(const string &path) {
    string fname;
    size_t lastpos = path.rfind('/');
    if (lastpos != std::string::npos) {
        fname = path.substr(lastpos + 1);
    }
    return fname;
}

vector<string> PXUTILS::FILE::dirfiles(const string &path, string ext) {
    vector<string> result;
    if (!ext.empty() && ext[0] == '.') {
        ext = ext.substr(1);
    }

    DIR *dirp = opendir(path.c_str());
    if (dirp != nullptr) {
        struct dirent *dp;
        while ((dp = readdir(dirp)) != nullptr) {
            if (strcmp(dp->d_name, ".") == 0)
                continue;
            if (strcmp(dp->d_name, "..") == 0)
                continue;

            if (ext.empty() || PXUTILS::FILE::extpart(dp->d_name) == ext) {
                result.emplace_back(dp->d_name);
            }
        }
        closedir(dirp);
    }
    return result;
}

string PXUTILS::FILE::extpart(const string &fname) {
    const char *fpointer = fname.c_str();
    const char *d = strrchr(fpointer, '.');
    if (!d || d == fpointer) {
        return string();
    }
    return string(d + 1);
}

bool PXUTILS::FILE::exists(const string &fpath) {
    struct stat buff{};
    return (stat(fpath.c_str(), &buff) == 0);
}

bool PXUTILS::FILE::remove(const string &path) {
    return (::remove(path.c_str()) == 0);
}

bool PXUTILS::FILE::write(const string &path, const string &data) {
    ofstream of(path.c_str(), ios::out);
    of << data;
    of.close();
    return true;
}


#define UNIX_PATH_HEADER "unix:"

bool PXUTILS::PATH::isunix(const string &path) {
    return (path.find(UNIX_PATH_HEADER) == 0);
}

string PXUTILS::PATH::unix2path(const string &upath) {
    string path = upath;
    if (isunix(upath)) {
        path.replace(0, strlen(UNIX_PATH_HEADER), "");
    }
    return path;
}

string PXUTILS::SYSTEM::current_user() {
    const size_t buffLen = 100;
    char buff[buffLen] = {0x00};
    int res = getlogin_r(buff, buffLen);
    if (res == 0) {
        return string(buff);
    }
    return string();
}


// ============================================================================
void Logger::log(Logger::LOG_LEVEL lvl, const char *file, const char *func, int line, const char *format, ...) {

    static map<Logger::LOG_LEVEL, std::string> lvlNames;
    lvlNames[Logger::LVL_ERR] = "ERR";
    lvlNames[Logger::LVL_WRN] = "WRN";
    lvlNames[Logger::LVL_INF] = "INF";

    if (lvl <= m_logLevel) {
        char buffer[1024];
        va_list args;
        va_start (args, format);
        vsprintf(buffer, format, args);
        va_end (args);

        string fname = PXUTILS::FILE::filename(file);
        std::cout << "[" << lvlNames[lvl] << "] - " << "[" << fname << "][" << func << "](" << line << "): "
                  << buffer << std::endl;
    }
}

Logger gLogger;
// ============================================================================
