//
// Created by Reza Alizadeh Majd on 2019-01-16.
//

#ifndef PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H
#define PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H


#include "Accounts/AccountDefinitions.h"

#define PROVIDER_APP_PATH    "./providers"
#define PROVIDER_USER_PATH   BASE_USER_PATH   "/etc/px/accounts/providers"
#define PROVIDER_SYSTEM_PATH BASE_SYSTEM_PATH "/etc/px/accounts/providers"


/// @brief data structure that contains Provider info
struct ProviderStruct {
    string title;   ///< provider title
    map<string, StrStrMap> plugins; ///< map of defined parameters for providers
};


/// @brief Responsible for managing Provider related tasks
class ProviderHandler {

protected:
    explicit ProviderHandler();

    /// @brief init default location for reading provider details
    bool init(const string& path);

    /// @brief init specific provider based on it's definition file
    bool initProvider(const string &providerPath);

public:
    /// @brief static method for accessing singleton provider instance
    static ProviderHandler &Instance();

    /// @brief array item operator for accessing specified provider
    ProviderStruct &operator[](const string &title);

    /// @brief check if provider with specific title is exists
    bool exists(const string &title);

    map<string, ProviderStruct> &providers();


protected:
    map<string, ProviderStruct> _providers;
    StringList _errorList;

protected:
    inline void addError(string err) { _errorList.push_back(err); }

    inline void resetErrors() { _errorList.clear(); }

public:
    inline static StringList &LastErrors() { return ProviderHandler::Instance()._errorList; }

};

#endif //PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H
