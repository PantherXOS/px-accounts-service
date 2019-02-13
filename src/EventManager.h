//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#ifndef PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
#define PX_ACCOUNTS_SERVICE_EVENTMANAGER_H

#include <iostream>
#include <map>

#include <nng/nng.h>
#include <nng/protocol/pipeline0/push.h>

#include "AccountParser.h"

using  namespace std;


class EventManager {

public:

protected:
    explicit EventManager();

    void emit(string topic, const map<string, string> &params);

public:
    virtual ~EventManager();
    static EventManager &Instance();
    static void EMIT_STATUS_CHANGE(string act, AccountStatus from, AccountStatus to);

private:
    static EventManager _instance;
    nng_socket m_sock;
    bool m_inited = false;

};

#endif //PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
