//
// Created by Reza Alizadeh Majd on 11/7/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H

#ifndef ACCOUNT_PATHS
#define ACCOUNT_PATHS "~/.userdata/accounts/"
#endif
#ifndef READONLY_ACCOUNT_PATHS
#define READONLY_ACCOUNT_PATHS ""
#endif


#include "AccountDefinitions.h"
#include "AccountUtils.h"
#include <tuple>

/// @brief namespace used for low-level operations on account files
namespace PXParser {

    /// @brief low-level function to read an account from disk
    bool read(const string &acName, AccountObject *ac);

    /// @brief low-level function for write an account to disk
    bool write(const string &acName, const AccountObject &ac);

    /// @brief low-level delete an account from disk
    bool remove(const string &acName);


    vector<AccountObject> list();

    /// @brief helper function to print an account details
    void print_account(const AccountObject &act);
}

#endif //PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
