//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#ifndef PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
#define PX_ACCOUNTS_SERVICE_EVENTMANAGER_H

#include <iostream>
#include <map>

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>

#include "AccountParser.h"

using  namespace std;


class EventManager {

public:
    enum EventType {
        NONE,
        ACCOUNT_STATUS_CHANGE
    };

protected:
    explicit EventManager();

    void emit(string src, EventType type, map<string, string> params);

public:
    virtual ~EventManager() = default;
    static EventManager &Instance();
    static void EMIT_STATUS_CHANGE(string src, AccountStatus from, AccountStatus to);

private:
    static EventManager _instance;
    nng_socket sock;
    bool inited = false;

};

#endif //PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
