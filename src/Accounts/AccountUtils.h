//
// Created by Reza Alizadeh Majd on 11/6/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H

#include <vector>
#include <string>
#include <Utils/Logger.h>

using namespace std;

#define EXISTS(m, v) ((m).find((v)) != (m).end())

/// @brief namespace that contains helper functions, used in Accounts Service
namespace PXUTILS {

    /// @brief account related helper functions
    namespace ACCOUNT {
        /// @brief convert account title to a proper format for disk
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

        /// @brief split colon separated path string to a vector of paths
        vector<string> extract_path_str(const string &pathStr);
    }

    /// @brief operating system related helper functions
    namespace SYSTEM {
        /// @brief get current logged in user
        string current_user();
    }
}

#endif //PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H
