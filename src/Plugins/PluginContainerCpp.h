//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINCONTAINERCPP_H
#define PX_ACCOUNTS_SERVICE_PLUGINCONTAINERCPP_H

#include "PluginContainerBase.h"
#include "DLLoader.h"

/// @brief Container for loading CPP plugins
class PluginContainerCpp : public PluginContainerBase {

public:
    explicit PluginContainerCpp(const PluginInfo &info);

    ~PluginContainerCpp() override;

    bool init() override;

    /// @brief get plugin title
    string getTitle() override;

    bool autoInitialize() override;

    int maxInstanceCount() override;

    /// @brief verify service params
    VerifyResult verify(const StrStrMap &params) override;

    /// @brief authenticate service based on provided params
    AuthResult authenticate(const ServiceParamList &params) override;

    StrStrMap read(const string &id) override;

    string write(VerifyResult &vResult, AuthResult &aResult) override;

    bool remove(const string &id) override;

private:
    DLLoader<IPlugin> _loader;
};

#endif //PX_ACCOUNTS_SERVICE_PLUGINCONTAINERCPP_H
