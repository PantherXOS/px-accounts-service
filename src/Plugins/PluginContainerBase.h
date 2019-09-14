//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINCONTAINERBASE_H
#define PX_ACCOUNTS_SERVICE_PLUGINCONTAINERBASE_H

#include "../Accounts/AccountDefinitions.h"
#include "../Accounts/AccountUtils.h"
#include "PluginInterface.h"

/// @brief enumeration for Plugin Types
enum class PluginTypes {
    PythonPlugin,   ///< Python plugin type
    CppPlugin,      ///< CPP plugin type
    UnknownPlugin   ///< Unknown plugin type
};
/// @brief String representation of PluginTypes
extern map<PluginTypes, string> PluginTypesStr;

/// @brief data structure to store plugin information file
struct PluginInfo {
    string name;            ///< plugin name
    string version;         ///< plugin version
    string typeStr;         ///< plugin type string
    PluginTypes type = PluginTypes::UnknownPlugin;      ///< plugin type in PluginTypes format
    string path;            ///< path to plugin location
};

/// @brief abstract base class for plugin containers
class PluginContainerBase {

protected:
    PluginContainerBase() = default;

public:
    virtual ~PluginContainerBase() = default;

public:
    /// @brief pure virtual method to get plugin title
    virtual string getTitle() = 0;

    /// @brief pure virtual method to call plugin's verify method
    virtual VerifyResult verify(const StrStrMap &params) = 0;

    /// @brief pure virtual method to call plugin's authenticate method
    virtual AuthResult authenticate(const ServiceParamList &params) = 0;

    virtual StrStrMap read(const string &id) = 0;

    virtual string write(const ServiceParamList &params) = 0;

public:
    /// @brief static method used to initiate a new PluginContainer based on provided path
    static PluginContainerBase *CreateContainer(const string &pluginInfoPath);

public:

    /// @brief getter method for plugin name
    const string &getName() const;

    /// @brief getter method for plugin version
    const string &getVersion() const;

    /// @brief getter method for plugin type
    PluginTypes getType() const;

    /// @brief getter method to show if plugin container is initiated
    bool isInited() const;

protected:
    PluginInfo _info;
    bool _inited = false;
};


#endif //PX_ACCOUNTS_SERVICE_PLUGINCONTAINERBASE_H
