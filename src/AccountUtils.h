//
// Created by Reza Alizadeh Majd on 11/6/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H

#include <vector>
#include <string>

using namespace std;

#define EXISTS(m, v) ((m).find((v)) != (m).end())

namespace PXUTILS {

    namespace ACCOUNT {
        string title2name(const string &title);
    }

    namespace PLUGIN {
        string package2module(const string& title);
    }

    namespace FILE
    {
        string abspath(const string &path);

        string basedir(const string &path);

        string filename(const string &path);

        vector<string> dirfiles(const string &path, string ext);

        string extpart(const string &fname);

        bool exists(const string &path);

        bool remove(const string &path);

        bool write(const string &path, const string &data);
    }

    namespace PATH
    {
        bool isunix(const string &path);

        string unix2path(const string &upath);
    }

    namespace SYSTEM {
        string current_user();
    }
}


class Logger {

public:
    enum LOG_LEVEL {
        LVL_ERR = 0,
        LVL_WRN = 1,
        LVL_INF = 2
    };

public:
    explicit Logger() = default;
    void log(LOG_LEVEL lvl, const char* file, const char* func, int line, const char* format, ...);

    void setLevel(LOG_LEVEL lvl) { m_logLevel = lvl; }

private:
    LOG_LEVEL m_logLevel = LVL_WRN;
};

extern Logger gLogger;
#define LOG_ERR(fmt, ...) gLogger.log(Logger::LVL_ERR, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WRN(fmt, ...) gLogger.log(Logger::LVL_WRN, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INF(fmt, ...) gLogger.log(Logger::LVL_INF, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H
