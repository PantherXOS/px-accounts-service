//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINCONTAINERBASE_H
#define PX_ACCOUNTS_SERVICE_PLUGINCONTAINERBASE_H

#include "../AccountDefinitions.h"

struct VerifyResult {
    bool             verified;
    ServiceParamList params;
    StringList       errors;

    VerifyResult(): verified(false) {
    }
};

struct AuthResult {
    bool       authenticated;
    StrStrMap  tokens;
    StringList errors;
};


enum class PluginTypes {
    PythonPlugin,
    CppPlugin,
    UnknownPlugin
};

struct PluginInfo {
    string name;
    string version;
    string typeStr;
    PluginTypes type = PluginTypes::UnknownPlugin;
    string path;
};

class PluginContainerBase {

protected:
    PluginContainerBase() = default;

public:
    virtual ~PluginContainerBase() = default;

public:
    virtual string getTitle() = 0;

    virtual VerifyResult verify(const StrStrMap &params) = 0;

    virtual AuthResult authenticate(const ServiceParamList &params) = 0;

public:
    static PluginContainerBase *CreateContainer(const string &pluginInfoPath);

public:
    const string &getName() const;

    const string &getVersion() const;

    PluginTypes getType() const;

    bool isInited() const;

protected:
    PluginInfo _info;
    bool _inited = false;
};


#endif //PX_ACCOUNTS_SERVICE_PLUGINCONTAINERBASE_H
