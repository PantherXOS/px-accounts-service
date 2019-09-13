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
