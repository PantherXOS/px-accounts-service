//
// Created by Reza Alizadeh Majd on 2019-01-20.
//

#include <thread>
#include "PasswordHandler.h"
#include "AccountUtils.h"

#define RPC_PASSWORD_SOCKET     "~/.userdata/rpc/password"

PasswordHandler PasswordHandler::_instance;


PasswordHandler::PasswordHandler() : _rpcClient(string("unix:") + PXUTILS::FILE::abspath(RPC_PASSWORD_SOCKET)) {

    if (!isRegistered()) {
        registerToPassService("123");
    }

}

PasswordHandler &PasswordHandler::Instance() {
    return PasswordHandler::_instance;
}

bool PasswordHandler::isRegistered() {
    // accIsRegisterd @14 ()->(isRegisterd : Bool, errorCode : Int16, errorText : Text);

    bool registered = false;
    int errCode = -1;
    string errText;

    _rpcClient.performRequest([&](kj::AsyncIoContext &ctx, PasswordInterface::Client &client) {
        auto req = client.accIsRegisterdRequest();
        auto resp = req.send().wait(ctx.waitScope);
        registered = resp.getIsRegisterd();
        errCode = resp.getErrorCode();
        errText = resp.getErrorText();
    });
    if (!registered) {
        addError(errText);
    }
    return registered;
}

bool PasswordHandler::registerToPassService(string userPass) {
    // registerPxAccountService @7(password: Text)->(errorCode : Int16, errorText : Text);

    int errCode = -1;
    string errText;

    _rpcClient.performRequest([&](kj::AsyncIoContext &ctx, PasswordInterface::Client &client) {
        auto req = client.registerPxAccountServiceRequest();
        req.setPassword(userPass);

        auto resp = req.send().wait(ctx.waitScope);
        errCode = resp.getErrorCode();
        errText = resp.getErrorText();
    });
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

    _rpcClient.performRequest([&](kj::AsyncIoContext &ctx, PasswordInterface::Client &client) {
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

    _rpcClient.performRequest([&](kj::AsyncIoContext &ctx, PasswordInterface::Client &client) {
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

