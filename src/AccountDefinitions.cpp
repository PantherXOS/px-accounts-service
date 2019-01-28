//
// Created by Reza Alizadeh Majd on 2018-12-23.
//

#include "AccountDefinitions.h"

map<AccountStatus, string> AccountStatusString = {
        { AC_NONE,    "NONE" },
        { AC_ONLINE,  "ONLINE" },
        { AC_OFFLINE, "OFFLINE" },
        { AC_ERROR,   "ERROR" }
};


void AccountService::applyVerification(const ServiceParamList &params) {

    _verified = false;
    _requiredDict.clear();
    _protectedDict.clear();

    for (const auto & param : params) {
        this->operator[](param.key) = param.val;
        _requiredDict[param.key] = param.is_required;
        _protectedDict[param.key] = param.is_protected;
    }

    _verified = true;
}

bool AccountService::isProtected(string key) const {

    if (_verified && _protectedDict.find(key) != _protectedDict.end()) {
        return _protectedDict.find(key)->second;
    }
    return false;
}

bool AccountService::isRequired(string key) const {
    if (_verified && _requiredDict.find(key) != _requiredDict.end()) {
        return _requiredDict.find(key)->second;
    }
    return false;
}
