//
// Created by Reza Alizadeh Majd on 2019-01-16.
//

#ifndef PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H
#define PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H


#include "Accounts/AccountDefinitions.h"
#include <Utils/ErrorReporter.h>

#define PROVIDER_APP_PATH    "./providers"
#define PROVIDER_USER_PATH   BASE_USER_PATH   "/etc/px/accounts/providers"
#define PROVIDER_SYSTEM_PATH BASE_SYSTEM_PATH "/etc/px/accounts/providers"


/// @brief data structure that contains Provider info
struct ProviderStruct {
    string title;   ///< provider title
    map<string, StrStrMap> plugins; ///< map of defined parameters for providers
};


/// @brief Responsible for managing Provider related tasks
class ProviderHandler : public ErrorReporter {

protected:
    /// @brief protected constructor for provider handler
    explicit ProviderHandler();

    /// @brief init default location for reading provider details
    bool init(const string &path);

    /// @brief init specific provider based on it's definition file
    bool initProvider(const string &providerPath);

public:
    /// @brief static method for accessing singleton provider instance
    static ProviderHandler &Instance();

    /// @brief array item operator for accessing specified provider
    ProviderStruct &operator[](const string &title);

    /// @brief check if provider with specific title is exists
    bool exists(const string &title);

    /// @brief reference to list of existing providers
    map<string, ProviderStruct> &providers();

protected:
    map<string, ProviderStruct> _providers;  ///< @brief mapping of existing providers

public:
    /// @brief static method reports list of last occurred errors
    inline static StringList &LastErrors() { return ProviderHandler::Instance().getErrors(); }

};

#endif //PX_ACCOUNTS_SERVICE_PROVIDERHANDLER_H
