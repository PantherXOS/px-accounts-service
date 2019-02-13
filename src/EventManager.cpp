//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#include "EventManager.h"
#include <capnp/serialize-packed.h>
#include <kj/io.h>
#include <interface/event.capnp.h>

#define RPC_DIR "~/.userdata/rpc"
#define RPC_PATH RPC_DIR "/events"
#define RPC_MKPATH_CMD "mkdir -p " RPC_DIR

EventManager EventManager::_instance;

EventManager::EventManager() {

    system(RPC_MKPATH_CMD);

    int rv;
    if ((rv = nng_push0_open(&m_sock)) != 0) {
        LOG_ERR("unable to open socket", rv);
        exit(EXIT_FAILURE);
    }

    string ipcSock = string("ipc://") + PXUTILS::FILE::abspath(RPC_PATH);
    if ((rv = nng_dial(m_sock, ipcSock.c_str(), nullptr, 0)) != 0) {
        LOG_ERR("connect error", rv);
        exit(EXIT_FAILURE);
    }
    m_inited = true;
}

EventManager::~EventManager() {
    if (m_inited) {
        nng_close(m_sock);
        m_inited = false;
    }
}

void EventManager::emit(string topic, const map<string, string> &params) {

    if (m_inited) {
        uint64_t now_secs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        ).count());

        capnp::MallocMessageBuilder message;
        EventData::Builder evt = message.initRoot<EventData>();
        evt.setTopic(topic);
        evt.setSource("px-accounts-service");
        evt.setTime(now_secs);
        evt.setEvent("account");

        auto evParams = evt.initParams(static_cast<unsigned int>(params.size()));
        int i = 0;
        for (const auto &kv : params) {
            evParams[i].setKey(kv.first);
            evParams[i].setValue(kv.second);
            i++;
        }
        kj::Array<capnp::word> words = capnp::messageToFlatArray(message);
        kj::ArrayPtr<kj::byte> data = words.asBytes();
        int ret;
        if ((ret = nng_send(m_sock, data.begin(), data.size(), 0)) != 0) {
            LOG_WRN("send error", ret);
        }
    }
}

EventManager &EventManager::Instance() {
    return _instance;
}

void EventManager::EMIT_STATUS_CHANGE(string act, AccountStatus from, AccountStatus to) {
    map<string, string> params;
    params["account"] = act;
    params["old"] = AccountStatusString[from];
    params["new"] = AccountStatusString[to];
    _instance.emit("account_status_change", params);
}
