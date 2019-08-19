//
// Created by Reza Alizadeh Majd on 11/6/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H

#include <vector>
#include <string>

using namespace std;

#define EXISTS(m, v) ((m).find((v)) != (m).end())

/// @brief namespace that contains helper functions, used in Accounts Service
namespace PXUTILS {

    /// @brief account related helper functions
    namespace ACCOUNT {
        /// @brief convert account title to a proper format for disk
        string title2name(const string &title);
    }

    /// @brief plugin related helper functions
    namespace PLUGIN {
        /// @brief convert plugin package names to python module format
        string package2module(const string& title);
    }

    /// @brief file-system related helper functions
    namespace FILE {
        /// @brief convert a relative path to an absolute one
        string abspath(const string &path);

        /// @brief extract base directory path from a full file path
        string basedir(const string &path);

        /// @brief extract filename from full file path
        string filename(const string &path);

        /// @brief get list of files inside a directory
        vector<string> dirfiles(const string &path, string ext);

        /// @brief extract extension part from a file name
        string extpart(const string &fname);

        /// @brief check if a file exists on disk
        bool exists(const string &path);

        /// @brief remove a file from disk
        bool remove(const string &path);

        /// @brief write text data to disk
        bool write(const string &path, const string &data);
    }

    /// @brief PATH related helper functions
    namespace PATH
    {
        /// @brief check whether input is a unix socket path
        bool isunix(const string &path);

        /// @brief convert a unix path to file-system path
        string unix2path(const string &upath);
    }

    /// @brief operating system related helper functions
    namespace SYSTEM {
        /// @brief get current logged in user
        string current_user();
    }
}

/// @brief Simple Logger to format Account service logs
class Logger {

public:
    /// @brief Log level enumerations
    enum LOG_LEVEL {
        LVL_ERR = 0,    ///< Error logs
        LVL_WRN = 1,    ///< Warning logs
        LVL_INF = 2     ///< Information logs
    };

public:
    explicit Logger() = default;

    /// @brief write a new log line in console
    void log(LOG_LEVEL lvl, const char* file, const char* func, int line, const char* format, ...);

    /**
     * @brief set visiblity level for logs to write
     * @details default level is WARNING
     */
    void setLevel(LOG_LEVEL lvl) { m_logLevel = lvl; }

private:
    LOG_LEVEL m_logLevel = LVL_WRN;
};

extern Logger gLogger;

#define LOG_ERR(fmt, ...) gLogger.log(Logger::LVL_ERR, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WRN(fmt, ...) gLogger.log(Logger::LVL_WRN, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INF(fmt, ...) gLogger.log(Logger::LVL_INF, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H
