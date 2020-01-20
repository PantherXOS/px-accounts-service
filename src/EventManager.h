//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#ifndef PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
#define PX_ACCOUNTS_SERVICE_EVENTMANAGER_H

#include <nng/nng.h>
#include <nng/protocol/pipeline0/push.h>
#include <Utils/Utils.h>

#include "Accounts/AccountParser.h"


/// @brief Class which is responsible for publishing Events to Event Service
class EventManager {

protected:
    /// @brief protected method for creating EventManager instance
    explicit EventManager();

    bool init();

    /// @brief general method for emitting events to Event Service
    bool emit(const string &event, const map<string, string> &params);

public:
    /// @brief EventManager destructor
    virtual ~EventManager();

    inline bool inited() const { return m_inited; }

    /// @brief static method to access EventManager singleton instance
    static EventManager &Instance();

    /// @brief helper method for emitting change status event
    static bool EMIT_STATUS_CHANGE(const string &accountTitle, AccountStatus from, AccountStatus to);
    static bool EMIT_CREATE_ACCOUNT(const string &accountTitle);
    static bool EMIT_MODIFY_ACCOUNT(const string &accountTitle, const string &newTitle = "");
    static bool EMIT_DELETE_ACCOUNT(const string &accountTitle);

private:
    nng_socket m_sock;      ///< @brief socket that EventManager use to connect to Event Service
    bool m_inited = false;  ///< @brief flag that indicates if EventManager is initiated or not

};

#endif //PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
