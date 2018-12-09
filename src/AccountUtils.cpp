//
// Created by Reza Alizadeh Majd on 11/6/18.
//

#include "AccountUtils.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <wordexp.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <algorithm>

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

string PXUTILS::PLUGIN::package2module(const string& title) {
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

vector<string> PXUTILS::FILE::dirfiles(const string &path, string ext) {
    vector<string> result;
    if (!ext.empty() && ext[0] == '.') {
        ext = ext.substr(1);
    }

    DIR* dirp = opendir(path.c_str());
    if (dirp != nullptr) {
        struct dirent * dp;
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
        return  string();
    }
    return string(d+1);
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

