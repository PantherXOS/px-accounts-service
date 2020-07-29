//
// Created by Reza Alizadeh Majd on 11/28/18.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H
#define PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H

#include "PluginContainerBase.h"

/// @brief Base class for managing plugin related classes
class PluginManager {

protected:
    explicit PluginManager();

    /// @brief init a plugin based on it's path
    bool initPlugins(const std::string &path);

public:
    virtual ~PluginManager();

    /// @brief method to access
    static PluginManager &Instance();

    /// @brief array item access operator
    PluginContainerBase *operator[](const std::string &);

    /// @brief get reference to map of registered plugins
    map<string, PluginContainerBase *> &plugins();

protected:
    map<string, PluginContainerBase *> _plugins;
};


#endif //PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H
