//
// Created by Reza Alizadeh Majd on 2019-01-28.
//

#ifndef PX_ACCOUNTS_SERVICE_PASSWORDSIMULATOR_H
#define PX_ACCOUNTS_SERVICE_PASSWORDSIMULATOR_H

#include <iostream>
#include <map>

using namespace std;

#include <interface/password/interface.capnp.h>
#include <Accounts/AccountDefinitions.h>



class PasswordSimulator final : public PasswordInterface::Server {

public:
    kj::Promise<void> accCreateNewEntry(AccCreateNewEntryContext ctx) override;

    kj::Promise<void> accIsRegisterd(AccIsRegisterdContext ctx) override;

    kj::Promise<void> registerPxAccountService(RegisterPxAccountServiceContext ctx) override;

    kj::Promise<void> accGetPassword(AccGetPasswordContext ctx) override;

protected:
    map < string, StrStrMap > m_passDict;     // m_passDict[page][user name] = password.
    bool m_registered = false;


};

#endif //PX_ACCOUNTS_SERVICE_PASSWORDSIMULATOR_H
