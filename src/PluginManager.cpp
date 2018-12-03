//
// Created by Reza Alizadeh Majd on 11/28/18.
//


#include "PluginManager.h"

#ifdef __linux__
#define PLUGIN_CMD "guix package --list-installed | grep -o '^[a-zA-Z0-9\\-]*'"
#else
//#define PLUGIN_CMD "echo 'px-accounts-service-plugin-imap\n'"
#define PLUGIN_CMD "echo 'testmodule\n'"
#endif


PluginManager::PluginManager() {
    py::initialize_interpreter();
    init();
}

PluginManager::~PluginManager() {
    _plugins.clear();
    py::finalize_interpreter();
}

PluginManager &PluginManager::Instance() {
    static PluginManager instance;
    return instance;
}

PluginContainer &PluginManager::operator[](const std::string &title) {
    return  _plugins[title];
}

map<string, PluginContainer> &PluginManager::plugins() {
    return _plugins;
}

bool PluginManager::init() {

    FILE *pipe = popen(PLUGIN_CMD, "r");
    if (!pipe) {
        return false;
    }

    std::array<char, 128> buffer{};
    std::stringstream data;

    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            data << buffer.data();
    }
    if (pclose(pipe) != EXIT_SUCCESS) {
        return false;
    }

    _plugins.clear();

    std::string line;
    while (std::getline(data, line)) {
        if (!line.empty()) {
            PluginContainer p(line);
            _plugins[p.getTitle()] = p;
        }
    }

    return true;
}
