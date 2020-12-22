//
// Created by Reza Alizadeh Majd on 9/15/19.
//

#include "cpp-custom-plugin.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <uuid/uuid.h>

using namespace std;

template<char delimiter>
class WordDelimitedBy : public std::string {
};

CPPCustomPlugin::CPPCustomPlugin() : IPlugin("cpp-custom") {}

VerifyResult CPPCustomPlugin::verify(const StrStrMap &params) {
    VerifyResult result;
    for (const auto &kv : params) {
        ServiceParam p;
        p.key = kv.first;
        p.val = kv.second;
        result.params.push_back(p);
    }
    result.verified = true;
    return result;
}

AuthResult CPPCustomPlugin::authenticate(const ServiceParamList &params) {
    AuthResult result;
    SecretToken token{.label = "token1", .secret = "token_value_1"};
    result.tokens.push_back(token);
    result.authenticated = true;
    return result;
}

StrStrMap CPPCustomPlugin::read(const string &id) {
    StrStrMap result;
    string fName = id + ".txt";
    ifstream stream(fName);
    if (stream.is_open()) {
        string line;
        while (getline(stream, line)) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(line);
            while (std::getline(tokenStream, token, '=')) {
                tokens.push_back(token);
            }
            if (tokens.size() == 2) {
                result[tokens[0]] = tokens[1];
            }
        }
    }
    return result;
}

string CPPCustomPlugin::write(VerifyResult &vResult, AuthResult &aResult) {
    uuid_t id;
    char strId[37] = {0x00};
    uuid_generate(id);
    uuid_unparse_lower(id, strId);
    string fName = std::string(strId) + ".txt";
    ofstream stream;
    stream.open(fName);
    for (const auto &param : vResult.params) {
        stream << param.key << "=" << param.val << endl;
    }
    stream.close();
    return string(strId);
}

bool CPPCustomPlugin::remove(const string &id) {
    string fName = id + ".txt";
    return (::remove(fName.c_str()) == 0);
}
