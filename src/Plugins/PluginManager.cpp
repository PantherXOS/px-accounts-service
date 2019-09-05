//
// Created by Reza Alizadeh Majd on 11/28/18.
//


#include "PluginManager.h"
#include "../AccountUtils.h"

#include <pybind11/embed.h>

namespace py = pybind11;


#define APP_PLUGIN_PATH "./plugins"
#define USER_PLUGIN_PATH "~/.guix-profile/etc/px/accounts/plugins"
#define SYSTEM_PLUGIN_PATH "/run/current-system/profile/etc/px/accounts/plugins"


PluginManager::PluginManager() {
    py::initialize_interpreter();

    init(PXUTILS::FILE::abspath(SYSTEM_PLUGIN_PATH));
    init(PXUTILS::FILE::abspath(USER_PLUGIN_PATH));
    init(PXUTILS::FILE::abspath(APP_PLUGIN_PATH));
}

PluginManager::~PluginManager() {
    for (const auto &plugin : _plugins) {
        delete plugin.second;
    }
    _plugins.clear();
    py::finalize_interpreter();
}

PluginManager &PluginManager::Instance() {
    static PluginManager instance;
    return instance;
}

/**
 * @param title plugin title we want to get it's reference
 * @return reference to specified plugin
 */
PluginContainerBase *PluginManager::operator[](const std::string &title) {
    if (_plugins.find(title) == _plugins.end()) {
        return nullptr;
    }
    return _plugins[title];
}

map<string, PluginContainerBase *> &PluginManager::plugins() {
    return _plugins;
}

/**
 * @param path path to plugin's information file
 * @return plugin initiation status
 */
bool PluginManager::init(const std::string &path) {
    GLOG_INF("===============================================================================");
    GLOG_INF("search for registered plugins on: ", path);
    for (const string &pluginFile: PXUTILS::FILE::dirfiles(path, ".yaml")) {
        PluginContainerBase *plugin = PluginContainerBase::CreateContainer(path + "/" + pluginFile);
        if (plugin != nullptr && plugin->isInited()) {
            GLOG_INF("   - new plugin loaded: [", PluginTypesStr[plugin->getType()], "]\t: ", plugin->getTitle());
            _plugins[plugin->getTitle()] = plugin;
        }
    }
    return true;
}
