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
#include <pwd.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>


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
    cout << " - PATH: " << path << " - POS: " << lastpos << endl;
    if (lastpos > 0) {
        dir = path.substr(0, lastpos);
    } else if (lastpos == 0) {
        // base directory is "/"
        dir = "/";
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

/**
 * create directory path
 *
 * @param path: path to create
 * @return creation succeed or not
 */
bool PXUTILS::FILE::mkpath(const string &path) {
    auto fullPath = abspath(path);
    cout << "create: " << fullPath << endl;

    if (PXUTILS::FILE::exists(fullPath)) {
        cout << "path exists: " << fullPath << endl;
        return true;
    }
    auto parent = PXUTILS::FILE::basedir(fullPath);
    if (PXUTILS::FILE::mkpath(parent)) {
        int status = ::mkdir(fullPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        if (status != 0) {
            cout << "[" << errno << "] " << strerror(errno) << endl;
            return false;
        }
        return true;
    }
    return false;
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

vector<string> PXUTILS::PATH::extract_path_str(const string &pathStr) {
    vector<string> pathList;
    auto token = pathStr;
    size_t pos = 0;
    while ((pos = token.find(":")) != std::string::npos) {
        pathList.push_back(PXUTILS::FILE::abspath(token.substr(0, pos)));
        token.erase(0, pos + 1);
    }
    pathList.push_back(PXUTILS::FILE::abspath(token));
    return pathList;
}

/**
 * @return current logged in user
 */
string PXUTILS::SYSTEM::current_user() {
    uid_t uid = geteuid();
    struct passwd *pwd = getpwuid(uid);
    if (pwd) {
        return string(pwd->pw_name);
    }
    return string();
}


// ============================================================================
