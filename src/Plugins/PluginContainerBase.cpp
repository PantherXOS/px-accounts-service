//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#include "PluginContainerBase.h"
#include <yaml-cpp/yaml.h>

#include "PluginContainerPython.h"
#include "../AccountUtils.h"


#define PLUGIN_KEY "plugin"
#define PLUGIN_NAME_KEY "name"
#define PLUGIN_VERSION_KEY "version"
#define PLUGIN_TYPE_KEY "type"
#define PLUGIN_TYPE_PYTHON_VAL "python"
#define PLUGIN_TYPE_CPP_VAL "cpp"


PluginContainerBase *PluginContainerBase::CreateContainer(const string &pluginInfoPath) {

    PluginContainerBase *result = nullptr;
    if (PXUTILS::FILE::exists(pluginInfoPath)) {
        try {
            YAML::Node plugin = YAML::LoadFile(pluginInfoPath);
            if (plugin[PLUGIN_KEY]) {
                PluginInfo inf;
                inf.name = plugin[PLUGIN_KEY][PLUGIN_NAME_KEY].as<string>();
                inf.version = plugin[PLUGIN_KEY][PLUGIN_VERSION_KEY].as<string>();
                inf.typeStr = plugin[PLUGIN_KEY][PLUGIN_TYPE_KEY].as<string>();

                if (inf.typeStr == PLUGIN_TYPE_PYTHON_VAL) {
                    inf.type = PluginTypes::PythonPlugin;
                    result = new PluginContainerPython(inf);

                } else if (inf.typeStr == PLUGIN_TYPE_CPP_VAL) {
                    inf.type = PluginTypes::CppPlugin;
                    LOG_WRN("plugin CPP not implemented yet: %s", inf.typeStr.c_str());

                } else {
                    LOG_ERR("Invalid Plugin Type: %s", inf.typeStr.c_str());
                }
            }
        }
        catch (YAML::Exception &ex) {
            LOG_ERR("Plugin load Error: %s", ex.what());
        }
    }
    return result;
}

const string &PluginContainerBase::getName() const {
    return _info.name;
}

const string &PluginContainerBase::getVersion() const {
    return _info.version;
}

PluginTypes PluginContainerBase::getType() const {
    return _info.type;
}

bool PluginContainerBase::isInited() const {
    return _inited;
}
