//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#include "cpp-test-plugin.h"

#if defined(__linux__) || defined(__APPLE__)

extern "C"
{
    CPPTestPlugin *allocator() {
        return new CPPTestPlugin();
    }

    void deleter(CPPTestPlugin* ptr) {
        delete ptr;
    }
}

#endif
