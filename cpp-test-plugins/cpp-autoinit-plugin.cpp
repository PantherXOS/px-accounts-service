#include <PluginInterface.h>

class CppTestAutoInitPlugin: public IPlugin {
public:
    explicit CppTestAutoInitPlugin() : IPlugin("cpp-autoinit") {
        this->auto_init = true;
        this->max_count = 1;
    }

    virtual VerifyResult verify(const StrStrMap &params) override;

    virtual AuthResult authenticate(const ServiceParamList &params) override;
};

VerifyResult CppTestAutoInitPlugin::verify(const StrStrMap &params) {
    VerifyResult result;
    auto param = ServiceParam{.key = "test-key", .val = "test-val", .is_required = false};
    result.params.push_back(param);
    result.verified = true;
    return result;
}

AuthResult CppTestAutoInitPlugin::authenticate(const ServiceParamList &params) {
    AuthResult result;
    result.authenticated = true;
    return result;
}


#if defined(__linux__) || defined(__APPLE__)

extern "C"
{
    CppTestAutoInitPlugin *allocator() {
        return new CppTestAutoInitPlugin();
    }

    void deleter(CppTestAutoInitPlugin* ptr) {
        delete ptr;
    }
}

#endif
