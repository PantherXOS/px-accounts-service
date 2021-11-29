//
// Created by Reza Alizadeh Majd on 11/28/18.
//

#include "PluginManager.h"

#include <pybind11/embed.h>

#include <cstdlib>
#include <iostream>

#include "../Accounts/AccountUtils.h"

namespace py = pybind11;

PluginManager *PluginManager::_mgrPtr = nullptr;

/**
 * init python plugins interpreter, and init manager
 * for different paths of plugins
 */
PluginManager::PluginManager(vector<string> pluginPaths) {
    py::initialize_interpreter();

    const char *customPaths = std::getenv("PLUGIN_PATH");
    if (customPaths) {
        auto token = std::string(customPaths);
        size_t pos = 0;
        while ((pos = token.find(":")) != std::string::npos) {
            pluginPaths.push_back(PXUTILS::FILE::abspath(token.substr(0, pos)));
            token.erase(0, pos + 1);
        }
        pluginPaths.push_back(PXUTILS::FILE::abspath(token));
    }
    PluginContainerPtrMap plugins;
    for (const auto &path : pluginPaths) {
        auto pathPlugins = readPathPlugins(path);
        for (auto p : pathPlugins) {
            plugins[p->getName()] = p;
        }
    }
    initPlugins(plugins);
}

/**
 * delete allocated plugins and clear plugin list.
 * close python plugins interpreter
 */
PluginManager::~PluginManager() {
    for (const auto &kv : _plugins) {
        delete kv.second;
    }
    _plugins.clear();
    py::finalize_interpreter();
}

bool PluginManager::Init(vector<string> pluginPaths) {
    if (!PluginManager::Initiated()) {
        PluginManager::_mgrPtr = new PluginManager(pluginPaths);
    }
    return PluginManager::Initiated();
}

PluginManager &PluginManager::Instance() {
    if (!PluginManager::Initiated()) {
        throw std::logic_error("PluginManager is not initiated");
    }
    return *PluginManager::_mgrPtr;
}

/**
 * check if a plugin with specified title exists and return pointer to it.
 * otherwise returns nullptr.
 *
 * @param title plugin title we want to get it's reference
 * @return reference to specified plugin
 */
PluginContainerBase *PluginManager::operator[](const std::string &pluginName) {
    for (auto &kv : _plugins) {
        if (kv.second && kv.second->isInited()) {
            if (kv.second->getTitle() == pluginName) {
                return kv.second;
            }
        }
    }
    GLOG_WRN("Could not find plugin: ", pluginName);
    return nullptr;
}

/**
 * @return reference to list of registered plugins
 */
PluginContainerPtrMap &PluginManager::registeredPlugins() { return _plugins; }

/**
 * @param path path to plugin's information file
 * @return plugin initiation status
 */
PluginContainerPtrList PluginManager::readPathPlugins(const std::string &path) {
    PluginContainerPtrList plugins;
    for (const auto &pluginFile : PXUTILS::FILE::dirfiles(path, ".yaml")) {
        auto infoPath = path + "/" + pluginFile;
        auto *plugin = PluginContainerBase::CreateContainer(infoPath);
        if (plugin) {
            plugins.push_back(plugin);
        } else {
            GLOG_WRN("Unable to load plugin from: ", infoPath);
        }
    }
    return plugins;
}

bool PluginManager::initPlugins(PluginContainerPtrMap plugins) {
    for (auto &kv : plugins) {
        auto pluginName = kv.first;
        auto plugin = kv.second;
        if (plugin && plugin->init()) {
            GLOG_INF("   - new plugin loaded: [", PluginTypesStr[plugin->getType()], "]\t: ", plugin->getTitle(), "[",
                     plugin->getVersion(), "]"
                     // , " - ", plugin->infoPath()
                     , "[ max-count:", plugin->maxInstanceCount(), "]"
                     , "[ auto-init:", plugin->autoInitialize(), "]"
            );
            _plugins[plugin->getTitle()] = plugin;
        } else {
            GLOG_WRN("Plugin initiation failed: ", pluginName);
        }
    }
    return true;
}
