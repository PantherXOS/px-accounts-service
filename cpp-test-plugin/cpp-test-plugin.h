//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#ifndef CPP_TEST_PLUGIN_H
#define CPP_TEST_PLUGIN_H

#include <PluginInterface.h>

class CPPTestPlugin : public IPlugin {
public:
    explicit CPPTestPlugin() : IPlugin("cpp-test") {}

    virtual VerifyResult verify(const StrStrMap &params) override;

    virtual AuthResult authenticate(const ServiceParamList &params) override;
};

#endif
