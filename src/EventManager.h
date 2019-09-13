//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#ifndef PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
#define PX_ACCOUNTS_SERVICE_EVENTMANAGER_H

#include <iostream>
#include <map>

#include <nng/nng.h>
#include <nng/protocol/pipeline0/push.h>

#include "Accounts/AccountParser.h"

using namespace std;

/// @brief Class which is responsible for publishing Events to Event Service
class EventManager {

protected:
    /// @brief protected method for creating EventManager instance
    explicit EventManager();

    /// @brief method for emitting events to Event Service
    void emit(const string &event, const map<string, string> &params);

public:
    virtual ~EventManager();

    /// @brief static method to access EventManager singleton instance
    static EventManager &Instance();

    /// @brief helper method for emitting change status event
    static void EMIT_STATUS_CHANGE(const string &act, AccountStatus from, AccountStatus to);

private:
    nng_socket m_sock;
    bool m_inited = false;

};

#endif //PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
