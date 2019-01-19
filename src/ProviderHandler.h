//
// Created by Reza Alizadeh Majd on 2019-01-16.
//

#ifndef PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H
#define PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H


#include "AccountDefinitions.h"

#ifdef __linux__
#define PROVIDER_STORE_PATH "~/.guix-profile/etc/accounts/providers/"
#define PROVIDER_USER_PATH  "~/.userdata/accounts/providers/"
#else
#define PROVIDER_STORE_PATH "./providers/"
#define PROVIDER_USER_PATH  "~/.userdata/accounts/providers/"
#endif


struct ProviderStruct {
    string title;
    map < string, StrStrMap > plugins;
};

class ProviderHandler {

protected:
    explicit ProviderHandler();
    bool init();
    bool initProvider(string providerPath);

public:
    static ProviderHandler &Instance();
    ProviderStruct &operator[](const string &title);
    bool exists(const string &title);
    map<string, ProviderStruct> &providers();


protected:
    static ProviderHandler _instance;
    map<string, ProviderStruct> _providers;
    StringList _errorList;

protected:
    inline void addError(string err) { _errorList.push_back(err); }
    inline void resetErrors() { _errorList.clear(); }

public:
    inline static StringList &LastErrors() { return _instance._errorList; }

};

#endif //PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H
