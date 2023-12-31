//
// Created by Reza Alizadeh Majd on 2019-06-10.
//


#include <algorithm>
#include <PluginInterface.h>

class CPPTestPlugin : public IPlugin {
public:
    explicit CPPTestPlugin() : IPlugin("cpp-test") {}

    virtual VerifyResult verify(const StrStrMap &params) override;

    virtual AuthResult authenticate(const ServiceParamList &params) override;
};


VerifyResult CPPTestPlugin::verify(const StrStrMap &params) {
    StringList requiredParams{"k1", "k2"};
    StringList protectedParams{"k1"};
    StrStrMap optionalParams{std::pair<string, string>("o1", "ov1"), std::pair<string, string>("o2", "ov2")};

    VerifyResult result;
    result.verified = true;

    StringList receivedKeys;
    for (const auto &kv : params) receivedKeys.push_back(kv.first);

    for (const auto &key : requiredParams) {
        if (std::find(receivedKeys.begin(), receivedKeys.end(), key) == receivedKeys.end()) {
            result.verified = false;
            result.errors.push_back("Param '{" + key + "}' is required.");
        }
    }

    if (result.verified) {
        for (const auto &key : receivedKeys) {
            auto param = ServiceParam{
                .key = key,
                .val = params.at(key),
                .is_required = std::find(requiredParams.begin(), requiredParams.end(), key) != requiredParams.end(),
                .is_protected =
                    std::find(protectedParams.begin(), protectedParams.end(), key) != protectedParams.end()};
            result.params.push_back(param);
        }

        for (const auto &kv : optionalParams) {
            if (std::find(receivedKeys.begin(), receivedKeys.end(), kv.first) == receivedKeys.end()) {
                auto param =
                    ServiceParam{.key = kv.first, .val = kv.second, .is_required = false, .is_protected = true};
                result.params.push_back(param);
            }
        }
    }
    return result;
}

AuthResult CPPTestPlugin::authenticate(const ServiceParamList &params) {
    AuthResult result;
    SecretToken token{.label = "password", .secret = "token_value1"};
    token.attributes["username"] = "user1";
    token.attributes["schema"] = "password";
    result.tokens.push_back(token);
    result.authenticated = true;
    return result;
}

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
