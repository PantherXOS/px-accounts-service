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

/**
 * convert account title to a file-system friendly formatted one
 *
 * @param title account title
 *
 * @return file-system friendly formatted version of account title
 */
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

/**
 * convert a python plugin package name to python module recommended format
 *
 * @param title plugin's package title
 *
 * @return converted plugin title
 */
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

/**
 * @param path relative path
 *
 * @return equivalent abolute path for input
 */
string PXUTILS::FILE::abspath(const string &path) {
    string res;
    wordexp_t w;
    if (wordexp(path.c_str(), &w, 0) == 0) {
        res = string(*w.we_wordv);
        wordfree(&w);
    }
    return res;
}

/**
 * @param path full path
 *
 * @return base directory part of input path
 */
string PXUTILS::FILE::basedir(const string &path) {
    string dir;
    size_t lastpos = path.rfind('/');
    if (lastpos != std::string::npos) {
        dir = path.substr(0, lastpos);
    }
    return dir;
}

/**
 * @param path full path for a file
 *
 * @return exteracted part of full path
 */
string PXUTILS::FILE::filename(const string &path) {
    string fname;
    size_t lastpos = path.rfind('/');
    if (lastpos != std::string::npos) {
        fname = path.substr(lastpos + 1);
    }
    return fname;
}

/**
 * @param path path to directory
 * @param ext filter for specific file extensions to retrieve their list
 *
 * @return list of directory files matching the extension filter
 */
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

/**
 * @param fname file name to extract it's extension
 *
 * @return extracted extension of file
 */
string PXUTILS::FILE::extpart(const string &fname) {
    const char *fpointer = fname.c_str();
    const char *d = strrchr(fpointer, '.');
    if (!d || d == fpointer) {
        return string();
    }
    return string(d + 1);
}

/**
 * @param fpath path we want to check it's existance
 *
 * @return path existance status
 */
bool PXUTILS::FILE::exists(const string &fpath) {
    struct stat buff{};
    return (stat(fpath.c_str(), &buff) == 0);
}

/**
 * @param path  full path for file
 *
 * @return file removal status
 */
bool PXUTILS::FILE::remove(const string &path) {
    return (::remove(path.c_str()) == 0);
}

/**
 * @param path full path for file we want to write to
 * @param data text-based data we want to write
 *
 * @return data write status
 *
 * @throws exception in case that base path is not exists
 */
bool PXUTILS::FILE::write(const string &path, const string &data) {
    ofstream of(path.c_str(), ios::out);
    of << data;
    of.close();
    return true;
}


#define UNIX_PATH_HEADER "unix:"

/**
 * check whether a path is started with 'unix:' prefix
 *
 * @param path path we want to check
 *
 * @return boolean indicates that path is unix path or not
 */
bool PXUTILS::PATH::isunix(const string &path) {
    return (path.find(UNIX_PATH_HEADER) == 0);
}

/**
 * checks if path starts with 'unix:' and remove it if so
 *
 * @param upath path to convert
 *
 * @return converted path
 */
string PXUTILS::PATH::unix2path(const string &upath) {
    string path = upath;
    if (isunix(upath)) {
        path.replace(0, strlen(UNIX_PATH_HEADER), "");
    }
    return path;
}

/**
 * @return current logged in user
 */
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

/**
 *
 * @param lvl LOG_LEVEL that we want to submit
 * @param file filename that log line located on
 * @param func function name that log line located on
 * @param line line of code the log is located
 * @param format format string to write log
 * @param ...
 */
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
