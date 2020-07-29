//
// Created by Reza Alizadeh Majd on 2019-06-11.
//

#ifndef PX_ACCOUNTS_SERVICE_DLLOADER_H
#define PX_ACCOUNTS_SERVICE_DLLOADER_H

#include <iostream>
#include <memory>
#include <dlfcn.h>

using namespace std;

template<class T>
class IDLLoader {

public:
    virtual ~IDLLoader() = default;

    virtual bool DLOpenLib() = 0;

    virtual void DLCloseLib() = 0;

    virtual shared_ptr<T> DLGetInstance() = 0;

};


#if defined(__linux__) || defined(__APPLE__)

template<class T>
class DLLoader : public IDLLoader<T> {

private:
    void *_handle;
    std::string _libPath;
    std::string _allocClassSumbol;
    std::string _deleteClassSymbol;

public:
    explicit DLLoader(const std::string &libPath,
                      const std::string &allocSymbol = "allocator",
                      const std::string &deleteSymbol = "deleter") :
            _handle(nullptr),
            _libPath(libPath),
            _allocClassSumbol(allocSymbol),
            _deleteClassSymbol(deleteSymbol) {}

    bool DLOpenLib() override {
        if (!(_handle = dlopen(_libPath.c_str(), RTLD_NOW | RTLD_LAZY))) {
            GLOG_ERR("plugin open failed: ", dlerror());
            return false;
        }
        return true;
    }

    void DLCloseLib() override {
        if (dlclose(_handle) != 0) {
            GLOG_ERR("plugin close failed: ", dlerror());
        }
    }

    shared_ptr<T> DLGetInstance() override {
        if (_handle == nullptr) {
            GLOG_WRN("invalid handle");
            return nullptr;
        }
        using allocClass = T *(*)();
        using deleteClass = void (*)(T *);

        auto allocFunc = reinterpret_cast<allocClass>(dlsym(_handle, _allocClassSumbol.c_str()));
        auto deleteFunc = reinterpret_cast<deleteClass>(dlsym(_handle, _deleteClassSymbol.c_str()));

        if (!allocFunc || !deleteFunc) {
            DLCloseLib();
            GLOG_ERR("get plugin instance failed: ", dlerror());
            return nullptr;
        }

        return shared_ptr<T>(allocFunc(), [deleteFunc](T *p) { deleteFunc(p); });
    }
};

#endif

#endif //PX_ACCOUNTS_SERVICE_DLLOADER_H
