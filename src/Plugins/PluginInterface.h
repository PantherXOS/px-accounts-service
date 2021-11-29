//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H
#define PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H

#include <Utils/Utils.h>

#include <memory>

/// @brief structure for holding a service parameter on a plugin
struct ServiceParam {
    string key;          ///< @brief key of parameter
    string val;          ///< @brief value of parameter
    bool is_required;    ///< @brief required flag of parameter
    bool is_protected;   ///< @brief protected flag of parameter
    string default_val;  ///< @brief default value of parameter
};
typedef vector<ServiceParam> ServiceParamList;

/// @brief result structure for plugin's verification method
struct VerifyResult {
    bool verified;            ///< @brief plugin's verification result flag
    ServiceParamList params;  ///< @brief list of processed service parameters
    StringList errors;        ///< @brief list of plugin error messages

    VerifyResult() : verified(false) {}
};
typedef std::shared_ptr<VerifyResult> VerifyResultPtr;

struct SecretToken {
    string label;
    string secret;
    StrStrMap attributes;
};
typedef std::shared_ptr<SecretToken> SecretTokenPtr;
typedef vector<SecretToken> SecretTokenList;

/// @brief result structure for plugin's authenticate method
struct AuthResult {
    bool authenticated;      ///< @brief plugin's authentication result flag
    SecretTokenList tokens;  ///< @brief list of generated tokens during plugin authentication
    StringList errors;       ///< @brief list of plugin error messages
};
typedef std::shared_ptr<AuthResult> AuthResultPtr;

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

    /**
     * virtual method for a plugin to read it's parameters from a custom source
     * with it's custom format.
     *
     * @param id unique identifier that plugin previously generated during `write` procedure
     * @return string based key-value map of plugin parameters
     */
    virtual StrStrMap read(const string &id) { throw std::logic_error("not implemented"); }

    /**
     * virtual method for plugin to allow it to write plugin data in it's custom format.
     *
     * @param vResult result of plugin's verify method.
     * @param aResult result of plugin's authenticate method.
     * @return unique identifier about current write that used to allow us to access this write details
     */
    virtual string write(VerifyResult &vResult, AuthResult &aResult) { throw std::logic_error("not implemented"); }

    /**
     * virtual method that calls during account removal, to cleanup plugin generated data
     *
     * @param id unique identifier to referenced pluign details that needs to be removed
     * @return plugin removal status
     */
    virtual bool remove(const string &id) { throw std::logic_error("not implemented"); }

   public:
    string title;           ///< @brief plugin's title
    bool auto_init = false; ///< @brief indicate that if account service creates an account if plugin installed
};

#endif  // PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H