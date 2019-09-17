//
// Created by Reza Alizadeh Majd on 9/16/19.
//

#include "cpp-custom-plugin.h"

#if defined(__linux__) || defined(__APPLE__)

extern "C"
{
    CPPCustomPlugin *allocator() {
        return new CPPCustomPlugin();
    }

    void deleter(CPPCustomPlugin* ptr) {
        delete ptr;
    }
}

#endif