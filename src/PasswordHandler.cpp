//
// Created by Reza Alizadeh Majd on 2019-01-20.
//

#include <thread>
#include "PasswordHandler.h"
#include "AccountUtils.h"

PasswordHandler PasswordHandler::_instance;
bool PasswordHandler::_inited = false;


PasswordHandler &PasswordHandler::Instance() {
    return PasswordHandler::_instance;
}

bool PasswordHandler::Init(const string &addr, const string &userPass) {
    _instance.m_rpcClient = new RPCClient<PasswordInterface, PasswordInterface::Client>(addr);
    if (!_instance.isRegistered()) {
        _instance.registerToPassService(userPass);
    } else {
        std::cout << "> account service is already registered" << std::endl;
    }
    _inited = true;
    return true;
}

PasswordHandler::~PasswordHandler() {
    if (m_rpcClient != nullptr) {
        delete m_rpcClient;
        m_rpcClient = nullptr;
    }
}

bool PasswordHandler::isRegistered() {
    // accIsRegisterd @14 ()->(isRegisterd : Bool, errorCode : Int16, errorText : Text);

    bool registered = false;
    int errCode = -1;
    string errText;

    m_rpcClient->performRequest([&](kj::AsyncIoContext &ctx, PasswordInterface::Client &client) {
        auto req = client.accIsRegisterdRequest();
        auto resp = req.send().wait(ctx.waitScope);
        registered = resp.getIsRegisterd();
        errCode = resp.getErrorCode();
        errText = resp.getErrorText();
    });
    std::cout << "[" << __func__ << "] " << registered << " - " << errCode << " - " << errText << std::endl;
    if (!registered) {
        addError(errText);
    }
    return registered;
}

bool PasswordHandler::registerToPassService(string userPass) {
    // registerPxAccountService @7(password: Text)->(errorCode : Int16, errorText : Text);

    int errCode = -1;
    string errText;

    m_rpcClient->performRequest([&](kj::AsyncIoContext &ctx, PasswordInterface::Client &client) {
        auto req = client.registerPxAccountServiceRequest();
        req.setPassword(userPass);

        auto resp = req.send().wait(ctx.waitScope);
        errCode = resp.getErrorCode();
        errText = resp.getErrorText();
    });
    std::cout << "[" << __func__ << "] " << errCode << " - " << errText << std::endl;
    if (errCode != 0) {
        addError(errText);
        return false;
    }
    return true;
}


bool PasswordHandler::set(string act, string svc, string key, string val) {
    // accCreateNewEntry @9 (pack:Text, page: Text, line : PasswordBook.Line)->(errorCode : Int16, errorText : Text);
    // accSetPassword @13 (page: Text, userName: Text, oldPassword: Text, newPassword: Text)->(errorCode : Int16, errorText : Text);

    int errCode = -1;
    std::string errText;

    m_rpcClient->performRequest([&](kj::AsyncIoContext &ctx, PasswordInterface::Client &client) {
        auto req = client.accCreateNewEntryRequest();
//        req.setPack("");
        req.setPage(act);
        auto line = req.initLine();
        line.setUserName(PasswordHandler::MAKE_USERNAME(svc, key));
        line.setPassword(val);

        auto resp = req.send().wait(ctx.waitScope);
        errCode   = resp.getErrorCode();
        errText   = resp.getErrorText().cStr();
    });
    std::cout << "[" << __func__ << "] " << errCode << " - " << errText << std::endl;

    if (errCode != 0) {
        addError(errText);
        return false;
    }
    return true;
}

PasswordStruct PasswordHandler::get(string act, string svc, string key) {
    // accGetPassword @12 (page : Text, userName: Text)->(password: Text, errorCode : Int16, errorText : Text);
    PasswordStruct result = {
            .inited = false,
            .act = act,
            .svc = svc,
            .key = key
    };

    int errCode = -1;
    string errText;
    string password;

    m_rpcClient->performRequest([&](kj::AsyncIoContext &ctx, PasswordInterface::Client &client) {
        auto req = client.accGetPasswordRequest();
        req.setPage(act);
        req.setUserName(PasswordHandler::MAKE_USERNAME(svc, key));

        auto resp = req.send().wait(ctx.waitScope);
        errCode = resp.getErrorCode();
        errText = resp.getErrorText().cStr();
        password = resp.getPassword();
    });

    if (errCode == 0) {
        result.inited = true;
        result.val = password;

    } else {
        cout << errText << endl;
        addError(errText);
    }
    return result;
}