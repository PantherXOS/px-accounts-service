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

    /// @brief read list of plugins for in a path
    PluginContainerPtrList readPathPlugins(const std::string &path);

    /// @brief init plugin logic
    bool initPlugins(PluginContainerPtrMap plugins);

   public:
    virtual ~PluginManager();

    /// @brief method to access
    static PluginManager &Instance();

    /// @brief array item access operator
    PluginContainerBase *operator[](const string &pluginName);

    /// @brief get reference to map of registered plugins
    PluginContainerPtrMap &registeredPlugins();

   protected:
    PluginContainerPtrMap _plugins;
};

#endif  // PX_ACCOUNTS_SERVICE_PLUGINMANAGER_H
