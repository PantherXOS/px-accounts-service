//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#ifndef PX_ACCOUNTS_SERVICE_ERRORREPORTER_H
#define PX_ACCOUNTS_SERVICE_ERRORREPORTER_H

#include "Utils.h"

/// @brief utility class to add error reporting facilities to child classes
class ErrorReporter {

public:
    /// @brief get last reported errors
    StringList &getErrors() { return  _errorList; }

    /// @brief reset list of reported errors
    void resetErrors() { _errorList.clear(); }

protected:
    /// @brief add new error message to  error list
    inline void addError(const string &err) { _errorList.push_back(err); }

    /// @brief add a list of messages to error list
    void addErrorList(const StringList &errList) {
        for (const auto &err : errList) {
            addError(err);
        }
    }

protected:
    StringList _errorList;  ///< #brief list of reported error messages

};

#endif //PX_ACCOUNTS_SERVICE_ERRORREPORTER_H
