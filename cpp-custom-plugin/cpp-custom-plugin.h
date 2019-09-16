//
// Created by Reza Alizadeh Majd on 9/15/19.
//

#ifndef PX_ACCOUNTS_SERVICE_CPP_CUSTOM_PLUGIN_H
#define PX_ACCOUNTS_SERVICE_CPP_CUSTOM_PLUGIN_H

#include <PluginInterface.h>

class CPPCustomPlugin : public IPlugin {

public:
    explicit CPPCustomPlugin();

    VerifyResult verify(const StrStrMap &params) override;

    AuthResult authenticate(const ServiceParamList &params) override;

    StrStrMap read(const string &id) override;

    string write(VerifyResult &vResult, AuthResult &aResult) override;

    bool remove(const string &id) override;

};


#endif //PX_ACCOUNTS_SERVICE_CPP_CUSTOM_PLUGIN_H
