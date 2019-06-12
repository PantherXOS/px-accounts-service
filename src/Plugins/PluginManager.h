//
// Created by Reza Alizadeh Majd on 11/28/18.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H
#define PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H

#include "PluginContainerBase.h"


class PluginManager {

protected:
    explicit PluginManager();

    bool init(const std::string &path);

public:
    virtual ~PluginManager();

    static PluginManager &Instance();

    PluginContainerBase &operator[](const std::string &);

    bool exists(const string &);

    map<string, PluginContainerBase *> &plugins();

protected:
    map<string, PluginContainerBase *> _plugins;
};


#endif //PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H
