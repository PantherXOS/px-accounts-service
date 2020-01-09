//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#include "EventManager.h"
#include <chrono>
#include <capnp/serialize-packed.h>
#include <kj/io.h>
#include <interface/event.capnp.h>

#define RPC_DIR "~/.userdata/rpc"
#define RPC_PATH RPC_DIR "/events"
#define RPC_MKPATH_CMD "mkdir -p " RPC_DIR

#define ACCOUNT_STATUS_CHANGE_EVENT "account_status_change"
#define ACCOUNT_CREATE_EVENT        "account_create"
#define ACCOUNT_MODIFY_EVENT        "account_modify"
#define ACCOUNT_DELETE_EVENT        "account_delete"

/**
 * Initiates NNG socket and connect to Event Service
 */
EventManager::EventManager() {

    system(RPC_MKPATH_CMD);
    this->init();
}

bool EventManager::init() {
    m_inited = false;
    int rv;
    if ((rv = nng_push0_open(&m_sock)) != 0) {
        GLOG_ERR("unable to open socket: ", rv);
        return false;
    }

    string ipcSock = string("ipc://") + PXUTILS::FILE::abspath(RPC_PATH);
    if ((rv = nng_dial(m_sock, ipcSock.c_str(), nullptr, 0)) != 0) {
        GLOG_ERR("connect error: ", rv);
        return false;
    }
    m_inited = true;
    return false;
}

/**
 * close nng socket which was initiated for communication with Event Service
 */
EventManager::~EventManager() {
    if (m_inited) {
        nng_close(m_sock);
        m_inited = false;
    }
}

/**
 * Create and serialize a CapnProto message and send it to Event Service
 * using initiated NNG socket
 *
 * @param event event string that we want to emit to event service
 * @param params string-based key value map about event params
 */
bool EventManager::emit(const string &event, const map<string, string> &params) {

    if (!m_inited) {
        GLOG_WRN("EventManager is not inited.");
        return false;
    }
    uint64_t now_secs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    ).count());

    capnp::MallocMessageBuilder message;
    EventData::Builder evt = message.initRoot<EventData>();
    evt.setTopic("account");
    evt.setSource("px-accounts-service");
    evt.setTime(now_secs);
    evt.setEvent(event);

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
        GLOG_WRN("send error: ", ret);
        return false;
    }
    GLOG_INF("new event sent: ", event);
    return true;
}

/**
 * @return reference for EventManager instance
 */
EventManager &EventManager::Instance() {
    static EventManager instance;
    if (!instance.inited()) {
        instance.init();
    }
    return instance;
}

/**
 * initiate list of parameters that needs to emit a status-change event
 * to Event Service
 *
 * @param actName account that it's status is changed
 * @param from old status of account
 * @param to new status of account
 */
bool EventManager::EMIT_STATUS_CHANGE(const string &actName, AccountStatus from, AccountStatus to) {
    map<string, string> params;
    params["account"] = actName;
    params["old"] = AccountStatusString[from];
    params["new"] = AccountStatusString[to];
    return EventManager::Instance().emit(ACCOUNT_STATUS_CHANGE_EVENT, params);
}

bool EventManager::EMIT_CREATE_ACCOUNT(const string &actName) {
    map<string, string> params;
    params["account"] = actName;
    return EventManager::Instance().emit(ACCOUNT_CREATE_EVENT, params);
}

bool EventManager::EMIT_MODIFY_ACCOUNT(const string &actName, const string &newName) {
    map<string, string> params;
    params["account"] = actName;
    if (!newName.empty()) {
        params["new_title"] = newName;
    }
    return EventManager::Instance().emit(ACCOUNT_MODIFY_EVENT, params);
}

bool EventManager::EMIT_DELETE_ACCOUNT(const string &actName) {
    map<string, string> params;
    params["account"] = actName;
    return EventManager::Instance().emit(ACCOUNT_DELETE_EVENT, params);
}
