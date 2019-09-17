//
// Created by Reza Alizadeh Majd on 11/7/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H

#define BASE_ACCOUNT_PATH    "~/.userdata/accounts/"

#include "AccountDefinitions.h"
#include "AccountUtils.h"


/// @brief namespace used for low-level operations on account files
namespace PXParser {

    /// @brief low-level function to read an account from disk
    bool read(const string &acName, AccountObject *ac);

    /// @brief low-level function for write an account to disk
    bool write(const string &acName, const AccountObject &ac);

    /// @brief low-level delete an account from disk
    bool remove(const string &acName);

    /// @brief helper method to get base storage path for account files
    inline string accountsPath() {
        string acPath = PXUTILS::FILE::abspath(BASE_ACCOUNT_PATH);
        if (!PXUTILS::FILE::exists(acPath)) {
            system((string("mkdir -p ") + acPath).c_str());
        }
        return acPath;
    }

    /// @brief helper function to retreive full path for an account
    inline string fullPath(const string &acName) {
        return accountsPath() + acName + string(".yaml");
    }

    /// @brief helper function to print an account details
    void print_account(const AccountObject &act);
}

#endif //PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
