//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#include "EventManager.h"
#include <capnp/serialize-packed.h>
#include <kj/io.h>
#include <interface/Account.capnp.h>

#define IPC_DIR  "~/.userdata/events"
#define IPC_PATH IPC_DIR "/accounts"
#define IPC_MKDIR_CMD "mkdir -p " IPC_DIR

EventManager EventManager::_instance;

EventManager::EventManager() {

    system(IPC_MKDIR_CMD);

    int rv;
    if ((rv = nng_pub0_open(&sock)) != 0) {
        perror("nng_pub0_open");
        exit(EXIT_FAILURE);
    }

    string ipcFullPath = PXUTILS::FILE::abspath(IPC_PATH);
    if (PXUTILS::FILE::exists(ipcFullPath)) {
        PXUTILS::FILE::remove(ipcFullPath);
    }

    string ipcSock = string("ipc://") + ipcFullPath;

//    string ipcSock = string("ipc://") + PXUTILS::FILE::abspath(IPC_PATH);
    if ((rv = nng_listen(sock, ipcSock.c_str(), NULL, 0)) != 0) {
        perror("nng_listen");
        perror(ipcSock.c_str());
        exit(EXIT_FAILURE);
    }
    inited = true;
}


void EventManager::emit(string src, EventManager::EventType type, map<string, string> params) {

    if (inited) {
        capnp::MallocMessageBuilder message;
        Account::AccountEvent::Builder evt = message.initRoot<Account::AccountEvent>();
        evt.setSource(src);
        evt.setType((Account::EventType)type);
        auto evParams = evt.initParams(static_cast<unsigned int>(params.size()));
        int i = 0;
        for (const auto &kv : params) {
            evParams[i].setKey(kv.first);
            evParams[i].setValue(kv.second);
            i++;
        }
        kj::Array<capnp::word> words = capnp::messageToFlatArray(message);
        kj::ArrayPtr<kj::byte> data = words.asBytes();
        if (nng_send(sock, data.begin(), data.size(), 0) != 0) {
            perror("ng_send");
        }
    }
}

EventManager &EventManager::Instance() {
    return _instance;
}

void EventManager::EMIT_STATUS_CHANGE(string src, AccountStatus from, AccountStatus to) {
    map<string, string> params;
    params["old"] = AccountStatusString[from];
    params["new"] = AccountStatusString[to];
    _instance.emit(src, ACCOUNT_STATUS_CHANGE, params);
}
