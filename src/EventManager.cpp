//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#include "EventManager.h"

#include <capnp/serialize-packed.h>
#include <interface/event.capnp.h>
#include <kj/io.h>

#include <chrono>
#include <sstream>


#define ACCOUNT_STATUS_CHANGE_EVENT "account_status_change"
#define ACCOUNT_CREATE_EVENT "account_create"
#define ACCOUNT_MODIFY_EVENT "account_modify"
#define ACCOUNT_DELETE_EVENT "account_delete"

EventManager *EventManager::m_instancePtr = nullptr;

/**
 * Initiates NNG socket and connect to Event Service
 */
EventManager::EventManager(const string &socketPath) : m_socketPath(socketPath) {
    int rv;
    if ((rv = nng_push0_open(&m_sock)) != 0) {
        stringstream errStream;
        errStream << "unable to open socket: " << rv;
        throw std::logic_error(errStream.str());
    }
}

/**
 * close nng socket which was initiated for communication with Event Service
 */
EventManager::~EventManager() {
    nng_close(m_sock);
}

bool EventManager::Init(const string &path) {
    try {
        if (m_instancePtr != nullptr) {
            delete m_instancePtr;
            m_instancePtr = nullptr;
        }
        string rpcPath;
        if (path.find("/") == 0) {       // absolute path to unix socket
            rpcPath = "ipc://" + path;
        } else {
            rpcPath = path;
        }
        m_instancePtr = new EventManager(rpcPath);
        GLOG_INF("EventManager inited on: ", rpcPath);
    } catch (std::exception &ex) {
        GLOG_ERR(ex.what());
        m_instancePtr = nullptr;
    }
    return m_instancePtr != nullptr;
}

bool EventManager::connect() {
    int rv;
    if ((rv = nng_dial(m_sock, m_socketPath.c_str(), nullptr, 0)) != 0) {
        GLOG_ERR("connection failed: ", rv);
        return false;
    }
    return true;
}

/**
 * Create and serialize a CapnProto message and send it to Event Service
 * using initiated NNG socket
 *
 * @param event event string that we want to emit to event service
 * @param params string-based key value map about event params
 */
bool EventManager::emit(const string &event, const map<string, string> &params) {
    if (!this->inited()) {
        GLOG_WRN("EventManager is not initiated.");
        return false;
    }
    if (!this->connect()) {
        return false;
    }
    uint64_t now_secs = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count());

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
        GLOG_WRN("Send error: ", ret);
        return false;
    }
    GLOG_INF("New event sent: ", event);
    return true;
}

string EventManager::MAKE_SERVICES_PARAM(const AccountObject &account) {
    stringstream ss;
    for (const auto &kv : account.services) {
        ss << kv.first;
    }
    return ss.str();
}

/**
 * @return reference for EventManager instance
 */
EventManager &EventManager::Instance() {
    if (m_instancePtr == nullptr) {
        throw std::logic_error("EventManager is not inited yet!");
    }
    return *m_instancePtr;
}

/**
 * initiate list of parameters that needs to emit a status-change event
 * to Event Service
 *
 * @param accountTitle account that it's status is changed
 * @param from old status of account
 * @param to new status of account
 */
bool EventManager::EMIT_STATUS_CHANGE(const AccountObject &account, AccountStatus from, AccountStatus to) {
    map<string, string> params;
    params["account"] = account.idAsString();
    params["services"] = EventManager::MAKE_SERVICES_PARAM(account);
    params["old"] = AccountStatusString[from];
    params["new"] = AccountStatusString[to];
    return EventManager::Instance().emit(ACCOUNT_STATUS_CHANGE_EVENT, params);
}

bool EventManager::EMIT_CREATE_ACCOUNT(const AccountObject &account) {
    map<string, string> params;
    params["account"] = account.idAsString();
    params["services"] = EventManager::MAKE_SERVICES_PARAM(account);
    return EventManager::Instance().emit(ACCOUNT_CREATE_EVENT, params);
}

bool EventManager::EMIT_MODIFY_ACCOUNT(const AccountObject &account) {
    map<string, string> params;
    params["account"] = account.idAsString();
    params["services"] = EventManager::MAKE_SERVICES_PARAM(account);
    return EventManager::Instance().emit(ACCOUNT_MODIFY_EVENT, params);
}

bool EventManager::EMIT_DELETE_ACCOUNT(const AccountObject &account) {
    map<string, string> params;
    params["account"] = account.idAsString();
    params["services"] = EventManager::MAKE_SERVICES_PARAM(account);
    return EventManager::Instance().emit(ACCOUNT_DELETE_EVENT, params);
}
