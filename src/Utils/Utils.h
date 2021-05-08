//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#ifndef PX_ACCOUNTS_SERVICE_UTILS_H
#define PX_ACCOUNTS_SERVICE_UTILS_H

#include <uuid/uuid.h>

#include <iostream>
#include <sstream>
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
    return uuid_parse(strId.c_str(), id) == 0;
}

inline string mask_string(const string &input, bool showFirstChar = true, char maskCharacter = 'X') {
    stringstream maskedStream;
    if (input.length() == 0) {
        maskedStream << maskCharacter << maskCharacter << maskCharacter;
    } else {
        maskedStream << (showFirstChar ? input.at(0) : maskCharacter);
        for (int i = 0; i < input.length() - 1; i++) {
            maskedStream << maskCharacter;
        }
    }
    return maskedStream.str();
}

#endif  // PX_ACCOUNTS_SERVICE_UTILS_H
