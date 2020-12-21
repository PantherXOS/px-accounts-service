#include "test_common.h"

#include <Accounts/AccountManager.h>
#include <Accounts/AccountParser.h>

bool TESTCOMMON::ACCOUNTS::cleanup(const uuid_t &id) {
    auto *parser = AccountManager::Instance().findParser(id, true);
    if (!parser) {
        return true;
    }
    return parser->remove(id);
}

bool TESTCOMMON::ACCOUNTS::cleanup(const string &title) {
    auto accountList = AccountManager::Instance().listAccounts();
    for (auto &act : accountList) {
        if (!cleanup(act.id)) {
            return false;
        }
    }
    return true;
}