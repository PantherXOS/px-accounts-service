//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H
#define PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H

#include <string>
#include <vector>
#include <map>

using namespace std;

typedef vector<string> StringList;

/// @brief definition for string based key-value map
typedef map<string, string> StrStrMap;

/// @brief structure for holding a service parameter on a plugin
struct ServiceParam {
    string key;             ///< @brief key of parameter
    string val;             ///< @brief value of parameter
    bool is_required;       ///< @brief required flag of parameter
    bool is_protected;      ///< @brief protected flag of parameter
    string default_val;     ///< @brief default value of parameter
};
typedef vector<ServiceParam> ServiceParamList;

/// @brief result structure for plugin's verification method
struct VerifyResult {
    bool verified;              ///< @brief plugin's verification result flag
    ServiceParamList params;    ///< @brief list of processed service parameters
    StringList errors;          ///< @brief list of plugin error messages

    VerifyResult() : verified(false) {
    }
};

/// @brief result structure for plugin's authenticate method
struct AuthResult {
    bool authenticated;     ///< @brief plugin's authentication result flag
    StrStrMap tokens;       ///< @brief list of generated tokens during plugin authentication
    StringList errors;      ///< @brief list of plugin error messages
};


/// @brief Interface class that all plugins are inherited from
class IPlugin {
public:
    explicit IPlugin() = default;

    explicit IPlugin(const string &title) : title(title) {}

    virtual ~IPlugin() = default;

    /**
     * @brief abstract method for service parameters verification
     *
     * @param  params String based key-value mapping for
     * @return VerifyResult structure as result of plugin's verify method
     */
    virtual VerifyResult verify(const StrStrMap &params) = 0;

    /**
     * @brief abstract method for service authentication
     *
     * @param params list of parameters needs for service to be authenticated
     * @return AuthResult structure as result of plugin's authenticate method
     */
    virtual AuthResult authenticate(const ServiceParamList &params) = 0;

public:
    string title;
};

#endif //PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H