//
// Created by Reza Alizadeh Majd on 2018-12-22.
//

#ifndef PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
#define PX_ACCOUNTS_SERVICE_EVENTMANAGER_H

#include <nng/nng.h>
#include <uuid/uuid.h>
#include <nng/protocol/pipeline0/push.h>
#include <Utils/Utils.h>

#include "Accounts/AccountObject.h"
#include "Accounts/AccountParser.h"


/// @brief Class which is responsible for publishing Events to Event Service
class EventManager {

protected:
    /// @brief protected method for creating EventManager instance
    explicit EventManager(const string &path);

    /// @brief general method for emitting events to Event Service
    bool emit(const string &event, const map<string, string> &params);

    static string MAKE_SERVICES_PARAM(const AccountObject &account);

public:
    /// @brief EventManager destructor
    virtual ~EventManager();

    static bool Init(const string &path);

    inline bool inited() const { return m_instancePtr != nullptr; }

    /// @brief connect to remote event manager service
    bool connect();

    /// @brief static method to access EventManager singleton instance
    static EventManager &Instance();

    /// @brief helper method for emitting change status event
    static bool EMIT_STATUS_CHANGE(const AccountObject &account, AccountStatus from, AccountStatus to);
    static bool EMIT_CREATE_ACCOUNT(const AccountObject &account);
    static bool EMIT_MODIFY_ACCOUNT(const AccountObject &account);
    static bool EMIT_DELETE_ACCOUNT(const AccountObject &account);

private:
    static EventManager *m_instancePtr; ///< @brief static pointer to EventManager instance
    nng_socket m_sock;                  ///< @brief socket that EventManager use to connect to Event Service
    string m_socketPath;                ///< @brief remote path that receiver connects to
};

#endif //PX_ACCOUNTS_SERVICE_EVENTMANAGER_H
