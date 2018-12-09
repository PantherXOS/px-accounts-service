//
// Created by Reza Alizadeh Majd on 11/6/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H

#include <vector>
#include <string>

using namespace std;

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

        vector<string> dirfiles(const string &path, string ext);

        string extpart(const string &fname);

        bool exists(const string &path);

        bool remove(const string &path);

        bool write(const string &path, const string &data);
    }


}


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTUTILS_H
