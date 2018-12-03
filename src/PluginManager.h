//
// Created by Reza Alizadeh Majd on 11/28/18.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H
#define PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H

#include "PluginContainer.h"

class PluginManager {

protected:
    explicit PluginManager();
    bool init();

public:
    virtual ~PluginManager();
    static PluginManager &Instance();

    PluginContainer &operator[](const std::string &);
    map<string, PluginContainer> &plugins();

protected:
    map<string, PluginContainer> _plugins;
};



#endif //PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H
