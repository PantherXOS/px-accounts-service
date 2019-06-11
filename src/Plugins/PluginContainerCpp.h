//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINCONTAINERCPP_H
#define PX_ACCOUNTS_SERVICE_PLUGINCONTAINERCPP_H

#include "PluginContainerBase.h"
#include "DLLoader.h"

class PluginContainerCpp : public PluginContainerBase {

public:
    explicit PluginContainerCpp(const PluginInfo &info);

    ~PluginContainerCpp() override;

    string getTitle() override;

    VerifyResult verify(const StrStrMap &params) override;

    AuthResult authenticate(const ServiceParamList &params) override;

private:
    DLLoader<IPlugin> _loader;
};

#endif //PX_ACCOUNTS_SERVICE_PLUGINCONTAINERCPP_H
