//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#ifndef PX_ACCOUNTS_SERVICE_UTILS_H
#define PX_ACCOUNTS_SERVICE_UTILS_H

#include <uuid/uuid.h>

#include <iostream>
#include <map>
#include <vector>

using namespace std;

typedef map<string, string> StrStrMap;
typedef vector<string> StringList;

inline string uuid_as_string(const uuid_t id) {
    char strId[UUID_STR_LEN] = {0x00};
    uuid_unparse_lower(id, strId);
    return string(strId);
}

inline bool uuid_from_string(const string strId, uuid_t id) {
    uuid_clear(id);
    if (strId.empty()) {
        uuid_generate(id);
        return true;
    }
    return uuid_parse(strId.c_str(), id) == 0;
}

#endif  // PX_ACCOUNTS_SERVICE_UTILS_H
