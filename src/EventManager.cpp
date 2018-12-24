//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#include "EventManager.h"
#include <capnp/serialize-packed.h>
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

    string ipcSock = string("ipc://") + PXUTILS::FILE::abspath(IPC_PATH);
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
        auto evParams = evt.initParams(params.size());
        int i = 0;
        for (const auto &kv : params) {
            evParams[i].setKey(kv.first);
            evParams[i].setValue(kv.second);
            i++;
        }
        auto data = capnp::messageToFlatArray(message).asBytes();
        string strData(data.begin(), data.end());
        if (nng_send(sock, (void*)strData.c_str(), strData.size() + 1, 0) != 0) {
            perror("ng_send");
        }
    }
}

EventManager &EventManager::Instance() {
    return _instance;
}

void EventManager::EMIT_STATUS_CHANGE(string src, AccountStatus from, AccountStatus to) {
    map<string, string> params;
    params["old"] = AccounrStatusString[from];
    params["new"] = AccounrStatusString[to];
    _instance.emit(src, ACCOUNT_STATUS_CHANGE, params);
}
