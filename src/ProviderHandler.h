//
// Created by Reza Alizadeh Majd on 2019-01-16.
//

#ifndef PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H
#define PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H


#include "AccountDefinitions.h"


struct ProviderStruct {
    string name;
    map < string, StrStrMap > plugins;
};

class ProviderHandler {

protected:
    explicit ProviderHandler() = default;
    bool init();

public:
    static ProviderHandler &Instance();
    ProviderStruct &operator[](const string &title);
    bool exists(const string &title);
    map<string, ProviderStruct> &providers();


protected:
    static ProviderHandler _instance;
    map<string, ProviderStruct> _providers;

};

#endif //PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H
