//
// Created by Reza Alizadeh Majd on 2019-01-20.
//

#include <thread>
#include "PasswordHandler.h"
#include "AccountUtils.h"

#define RPC_PASSWORD_SOCKET     "~/.userdata/rpc/password"

PasswordHandler PasswordHandler::_instance;

//PasswordHandler::PasswordHandler() : _client(nullptr) {

//    if (ctx == nullptr) {
//        ctx = kj::heap(kj::setupAsyncIo());
//    }
//    this->_ctx = ctx;
//
//    string rpcPath = string("unix:") + PXUTILS::FILE::abspath(RPC_PASSWORD_SOCKET);
//    kj::WaitScope &waitScope = _ctx->waitScope;
//    auto netAddr = _ctx->provider->getNetwork().parseAddress(rpcPath).wait(waitScope);
//    auto stream = netAddr->connect().wait(waitScope);
//    _rpcClient = kj::heap<capnp::TwoPartyClient>(*stream);
//    _client = _rpcClient->bootstrap().castAs<PasswordInterface>();
//}

PasswordHandler::PasswordHandler() {
    _rpcPath = string("unix:") + PXUTILS::FILE::abspath(RPC_PASSWORD_SOCKET);
}

PasswordHandler &PasswordHandler::Instance() {
    return PasswordHandler::_instance;
}

bool PasswordHandler::set(string act, string svc, string key, string val) {

    // accCreateNewEntry @9 (pack:Text, page: Text, line : PasswordBook.Line)->(errorCode : Int16, errorText : Text);
    // accSetPassword @13 (page: Text, userName: Text, oldPassword: Text, newPassword: Text)->(errorCode : Int16, errorText : Text);

    int errCode = -1;
    std::string errText;

    std::thread thClient = std::thread([&]() {
        auto ctx = kj::setupAsyncIo();
        auto netAddr = ctx.provider->getNetwork().parseAddress(_rpcPath).wait(ctx.waitScope);
        auto stream = netAddr->connect().wait(ctx.waitScope);
        auto rpcClient = kj::heap<capnp::TwoPartyClient>(*stream);
        auto client = rpcClient->bootstrap().castAs<PasswordInterface>();

        auto req = client.accCreateNewEntryRequest();
//        req.setPack("");
        req.setPage(act);
        auto line = req.initLine();
        line.setUserName(PasswordHandler::MAKE_USERNAME(svc, key));
        line.setPassword(val);

        auto resp = req.send().wait(ctx.waitScope);
        errCode = resp.getErrorCode();
        errText = resp.getErrorText().cStr();

        free(rpcClient);
    });

    thClient.join();
    if (errCode != 0) {
        cout << errText << endl;
        addError(errText);
        return false;
    }
    return true;

//    auto req = _client.accCreateNewEntryRequest();
//    req.setPack("");
//    req.setPage(act);
//    auto line = req.initLine();
//    line.setUserName(PasswordHandler::MAKE_USERNAME(svc, key));
//    line.setPassword(val);
//
//    auto resp = req.send().wait(_ctx->waitScope);
//
//    if (resp.getErrorCode() != 0) {
//        cout << __func__ << ": " << resp.getErrorText().cStr() << endl;
//        return false;
//    }
//    return true;
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
    string errText = "";
    string password = "";

    std::thread thClient = std::thread([&]() {
        auto ctx = kj::setupAsyncIo();
        auto netAddr = ctx.provider->getNetwork().parseAddress(_rpcPath).wait(ctx.waitScope);
        auto stream = netAddr->connect().wait(ctx.waitScope);
        auto rpc = kj::heap<capnp::TwoPartyClient>(*stream);
        auto client = rpc->bootstrap().castAs<PasswordInterface>();

        auto req = client.accGetPasswordRequest();
        req.setPage(act);
        req.setUserName(PasswordHandler::MAKE_USERNAME(svc, key));

        auto resp = req.send().wait(ctx.waitScope);
        errCode = resp.getErrorCode();
        errText = resp.getErrorText();
        password= resp.getPassword();
    });

    thClient.join();
    if (errCode == 0) {
        result.inited = true;
        result.val = password;

    } else {
        cout << errText << endl;
        addError(errText);
    }
    return result;

//    auto req = _client.accGetPasswordRequest();
//    req.setPage(act);
//    req.setUserName(PasswordHandler::MAKE_USERNAME(svc, key));
//
//    auto resp = req.send().wait(_ctx->waitScope);
//    if (resp.getErrorCode() != 0) {
//        result.val = resp.getPassword().cStr();
//        result.inited = true;
//    } else {
//        cout << __func__ << ": " << resp.getErrorText().cStr() << endl;
//    }
//    return result;
}
