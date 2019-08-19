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

    /// @brief get plugin title
    string getTitle() override;

    /// @brief verify service params
    VerifyResult verify(const StrStrMap &params) override;

    /// @brief authenticate service based on provided params
    AuthResult authenticate(const ServiceParamList &params) override;

private:
    DLLoader<IPlugin> _loader;
};

#endif //PX_ACCOUNTS_SERVICE_PLUGINCONTAINERCPP_H
