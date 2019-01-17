//
// Created by Reza Alizadeh Majd on 2019-01-16.
//

#include "ProviderHandler.h"

ProviderHandler ProviderHandler::_instance;

bool ProviderHandler::init() {
    //todo: not implemented yet
    return false;
}

ProviderHandler &ProviderHandler::Instance() {
    return _instance;
}

ProviderStruct &ProviderHandler::operator[](const string &title) {
    return _providers[title];
}

bool ProviderHandler::exists(const string &title) {
    //todo: not implemented yet
    return false;
}

map<string, ProviderStruct> &ProviderHandler::providers() {
    return _providers;
}
