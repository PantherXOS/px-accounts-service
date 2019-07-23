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

PluginContainerBase &PluginManager::operator[](const std::string &title) {
    return *_plugins[title];
}

bool PluginManager::exists(const string &title) {
    return _plugins.find(title) != _plugins.end();
}

map<string, PluginContainerBase *> &PluginManager::plugins() {
    return _plugins;
}

bool PluginManager::init(const std::string &path) {
    LOG_INF("========================================================================================================");
    LOG_INF("search registered plugins for: %s", path.c_str());
    for (const string &pluginFile: PXUTILS::FILE::dirfiles(path, ".yaml")) {
        PluginContainerBase *plugin = PluginContainerBase::CreateContainer(path + "/" + pluginFile);
        if (plugin != nullptr && plugin->isInited()) {
            LOG_INF("   - new plugin loaded: [%s]: %s",
                    PluginTypesStr[plugin->getType()].c_str(),
                    plugin->getTitle().c_str());
            _plugins[plugin->getTitle()] = plugin;
        }
    }
    return true;
}
