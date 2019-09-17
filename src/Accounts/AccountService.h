//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTSERVICE_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTSERVICE_H

#include <Utils/ErrorReporter.h>

#include "../Plugins/PluginInterface.h"
//#include "../AccountDefinitions.h"

class AccountObject;
class PluginContainerBase;

///@brief Object holds detail of an Account Service
class AccountService : public StrStrMap, public ErrorReporter {

public:
    /// @brief initiate
    bool init(AccountObject* act, const string &name);

    /// @brief indicates whether service is initiated or not
    bool inited() { return _inited; }

    /// @brief apply plugin verification results
    void applyVerification(const ServiceParamList &params);

    /// @brief check if provided key is protected
    bool isProtected(const string &key) const;

    /// @brief check if provided key is required
    bool isRequired(const string &key) const;

    /// @brief pointer to plugin which is related to service
    PluginContainerBase* plugin();

    /// @brief verify Account against provided service
    bool verify();

    /// getter to check if service is verified
    bool verified() { return _inited && _verified; }

    void clearService() {
        this->clear();
        this->_protectedDict.clear();
        this->_requiredDict.clear();
    }

    bool performCustomRead();

    bool performServiceCustomRemoval();


protected:
    /// @brief check and prepare parameters needed for a service
    VerifyResultPtr _verifyParams();

    /// @brief perform authentication task for a service
    AuthResultPtr _authenticate(VerifyResultPtr &vResult);

    /// @brief save protected params to SecretManager
    bool _saveProtectedParams(VerifyResultPtr &vResult, AuthResultPtr &aResult);

protected:
    string _name;
    AccountObject* _account = nullptr;
    PluginContainerBase* _plugin = nullptr;

    bool _inited = false;
    bool _verified = false;

    map<string, bool> _requiredDict;
    map<string, bool> _protectedDict;
};


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTSERVICE_H
