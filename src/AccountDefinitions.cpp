//
// Created by Reza Alizadeh Majd on 2018-12-23.
//

#include "AccountDefinitions.h"

map<AccountStatus, string> AccountStatusString = {
        {AC_NONE,    "NONE"},
        {AC_ONLINE,  "ONLINE"},
        {AC_OFFLINE, "OFFLINE"},
        {AC_ERROR,   "ERROR"}
};

/**
 * apply results received from plugin's `verify` method to service
 *
 * @param params: List of ServiceParam details processed inside plugin
 */
void AccountService::applyVerification(const ServiceParamList &params) {
    _verified = false;
    _requiredDict.clear();
    _protectedDict.clear();
    for (const auto &param : params) {
        this->operator[](param.key) = param.val;
        _requiredDict[param.key] = param.is_required;
        _protectedDict[param.key] = param.is_protected;
    }
    _verified = true;
}

/**
 * check whether provided key is protected parameter or not
 *
 * @param key name of parameter to check if is protected or not
 *
 * @return protected flag for provided key
 */
bool AccountService::isProtected(const string &key) const {
    if (_verified && _protectedDict.find(key) != _protectedDict.end()) {
        return _protectedDict.find(key)->second;
    }
    return false;
}

/**
 * check whether provided key is a required parameter or not
 *
 * @param key name of parameter to check if is required or not
 *
 * @return required flag for provided key
 */
bool AccountService::isRequired(const string &key) const {
    if (_verified && _requiredDict.find(key) != _requiredDict.end()) {
        return _requiredDict.find(key)->second;
    }
    return false;
}
