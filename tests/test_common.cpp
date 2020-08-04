#include "test_common.h"

#include <Accounts/AccountParser.h>
#include <Accounts/AccountManager.h>

bool TESTCOMMON::ACCOUNTS::cleanup(const string &title) {
    auto actName = PXUTILS::ACCOUNT::title2name(title);
    auto *parser = AccountManager::Instance().findParser(actName, true);
    if (!parser) {
        return true;
    }
    return parser->remove(actName);
}