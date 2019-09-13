//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#ifndef PX_ACCOUNTS_SERVICE_ERRORREPORTER_H
#define PX_ACCOUNTS_SERVICE_ERRORREPORTER_H

#include "Utils.h"

class ErrorReporter {

public:
    StringList &getErrors() { return  _errorList; }
    void resetErrors() { _errorList.clear(); }

protected:
    inline void addError(const string &err) { _errorList.push_back(err); }
    void addErrorList(const StringList &errList) {
        for (const auto &err : errList) {
            addError(err);
        }
    }

protected:
    StringList _errorList;

};

#endif //PX_ACCOUNTS_SERVICE_ERRORREPORTER_H
