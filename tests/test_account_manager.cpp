//
// Created by Reza Alizadeh Majd on 11/12/18.
//

//#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <iostream>

#include <Accounts/AccountManager.h>
#include "test_common.h"


TEST_CASE("Account Management Tasks", "[AccountManager]") {


    std::string title1 = "My Test Account";
    std::string title2 = "My Edited Title";
    std::string title3 = "Test Provider Account";

    AccountObject newAccount, account;
    newAccount.title = title1;
    newAccount.is_active = false;

    newAccount.settings["first key"] = "first value";
    newAccount.settings["second key"] = "second value";

    newAccount.services["python-test"].init(&newAccount, "python-test");
    newAccount.services["python-test"]["k1"] = "v1";
    newAccount.services["python-test"]["k2"] = "v2";


    uuid_t accountId;


    SECTION("Cleanup Old Test files") {
        REQUIRE(TESTCOMMON::ACCOUNTS::cleanup(title1));
        REQUIRE(TESTCOMMON::ACCOUNTS::cleanup(title2));
        REQUIRE(TESTCOMMON::ACCOUNTS::cleanup(title3));
    }

    SECTION("Create New Account") {
        GLOG_INF("TEST STARTED: Create New Account");
        bool createResult = AccountManager::Instance().createAccount(newAccount);
        if (!createResult) {
            for (const auto &err : AccountManager::LastErrors()) {
                WARN(err);
            }
        } else {
            if (!AccountManager::LastErrors().empty()) {
                REQUIRE(AccountManager::LastErrors().size() == 1);
                REQUIRE(AccountManager::LastErrors()[0] == "sample warning");
            }
        }
        REQUIRE(createResult);
        REQUIRE_FALSE(uuid_is_null(newAccount.id));
        uuid_copy(accountId, newAccount.id);
    }

    SECTION("Read Created Account") {
        GLOG_INF("TEST STARTED: Read Created Account");
        REQUIRE(AccountManager::Instance().readAccount(accountId, &account));
        REQUIRE(newAccount.title == account.title);
        REQUIRE(newAccount.provider == account.provider);
        REQUIRE(newAccount.is_active == account.is_active);
        REQUIRE(newAccount.settings.size() == 2);
        REQUIRE(newAccount.settings.find("first key") != newAccount.settings.end());
        REQUIRE(newAccount.settings["first key"] == "first value");
        REQUIRE(newAccount.settings.find("second key") != newAccount.settings.end());
        REQUIRE(newAccount.settings["second key"] == "second value");
        REQUIRE(newAccount.services.size() == 1);
        REQUIRE(newAccount.services.find("python-test") != newAccount.services.end());
        REQUIRE(newAccount.services["python-test"].size() == 2);
    }

    SECTION("Change Account active status") {
        GLOG_INF("TEST STARTED: Change Account active status");
        REQUIRE(AccountManager::Instance().readAccount(accountId, &account));

        account.is_active = true;
        bool modifyResult = AccountManager::Instance().modifyAccount(accountId, account);
        if (!modifyResult) {
            for (const auto &err : AccountManager::LastErrors()) {
                WARN(err);
            }
        } else {
            if (!AccountManager::LastErrors().empty()) {
                REQUIRE(AccountManager::LastErrors().size() == 1);
                REQUIRE(AccountManager::LastErrors()[0] == "sample warning");
            }
        }
        REQUIRE(modifyResult);

        REQUIRE(AccountManager::Instance().readAccount(accountId, &account));
        REQUIRE(account.is_active);
    }

    SECTION("Change Account Title") {
        GLOG_INF("TEST STARTED: Change Account Title");
        REQUIRE(AccountManager::Instance().readAccount(accountId, &account));

        account.title = title2;
        bool modifyResult = AccountManager::Instance().modifyAccount(accountId, account);
        if (!modifyResult) {
            for (const auto &err : AccountManager::LastErrors()) {
                WARN(err);
            }
        } else {
            if (!AccountManager::LastErrors().empty()) {
                REQUIRE(AccountManager::LastErrors().size() == 1);
                REQUIRE(AccountManager::LastErrors()[0] == "sample warning");
            }
        }
        REQUIRE(modifyResult);

        REQUIRE(AccountManager::Instance().readAccount(accountId, &account));
    }

    SECTION("Delete Account") {
        GLOG_INF("TEST STARTED: Delete Account");
        REQUIRE(AccountManager::Instance().deleteAccount(accountId));
    }

    SECTION("Create Account with Provider") {
        GLOG_INF("TEST STARTED: Create Account with Provider");
        AccountObject providerAccount;
        providerAccount.title = title3;
        providerAccount.provider = "test_provider";
        providerAccount.services["python-test"].init(&providerAccount, "python-test");
        providerAccount.services["python-test"]["k2"] = "v2";

        bool createResult = AccountManager::Instance().createAccount(providerAccount);
        if (!createResult) {
            for (const auto &err : AccountManager::LastErrors()) {
                WARN(err);
            }
        } else {
            if (!AccountManager::LastErrors().empty()) {
                REQUIRE(AccountManager::LastErrors().size() == 1);
                REQUIRE(AccountManager::LastErrors()[0] == "sample warning");
            }
        }
        REQUIRE(createResult);
        REQUIRE_FALSE(uuid_is_null(providerAccount.id));
        REQUIRE(AccountManager::Instance().readAccount(providerAccount.id, &account));
        REQUIRE(providerAccount.provider == account.provider);
        REQUIRE(EXISTS(providerAccount.services, "python-test"));
        REQUIRE(EXISTS(providerAccount.services["python-test"], "k1"));
        REQUIRE(EXISTS(providerAccount.services["python-test"], "k2"));
        REQUIRE(EXISTS(providerAccount.services["python-test"], "k3"));

        REQUIRE(providerAccount.services["python-test"]["k1"] == "provider_val1");
        REQUIRE(providerAccount.services["python-test"]["k2"] == "v2");
        REQUIRE(providerAccount.services["python-test"]["k3"] == "provider_val3");

        REQUIRE(AccountManager::Instance().deleteAccount(providerAccount.id));
    }

    SECTION("Modify Account with Public Plugin") {
        GLOG_INF("TEST STARTED: Modify Account with Public Plugin");
        REQUIRE(TESTCOMMON::ACCOUNTS::cleanup("my_public_account"));
        REQUIRE(TESTCOMMON::ACCOUNTS::cleanup("modified_public_act"));

        AccountObject act;
        act.title = "my_public_account";
        act.services["public-test"].init(&act, "public-test");
        bool createRes = AccountManager::Instance().createAccount(act);
        (AccountManager::LastErrors());
        REQUIRE(createRes);
        REQUIRE_FALSE(uuid_is_null(act.id));

        AccountObject savedAct;
        REQUIRE(AccountManager::Instance().readAccount(act.id, &savedAct));
        REQUIRE(savedAct.services.find("public-test") != savedAct.services.end());
        savedAct.title = "modified_public_act";
        bool modifyRes = AccountManager::Instance().modifyAccount(act.id, savedAct);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(modifyRes);

    }

    SECTION("Python plugin with custom read/write") {
        GLOG_INF("TEST STARTED: Python plugin with custom read/write");
        REQUIRE(TESTCOMMON::ACCOUNTS::cleanup("my_json_account"));

        AccountObject act;
        act.title = "my json account";
        act.services["python-json"].init(&act, "python-json");
        act.services["python-json"]["k1"] = "pv1";
        act.services["python-json"]["k2"] = "v2";
        bool createRes = AccountManager::Instance().createAccount(act);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(createRes);
        REQUIRE_FALSE(uuid_is_null(act.id));

        AccountObject receivedAct;
        bool readResult = AccountManager::Instance().readAccount(act.id, &receivedAct);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(readResult);

        REQUIRE(receivedAct.services.size() == 1);
        REQUIRE(receivedAct.services.find("python-json") != receivedAct.services.end());

        REQUIRE(receivedAct.services["python-json"]["k1"] == "pv1");
        REQUIRE(receivedAct.services["python-json"]["k2"] == "v2");
        REQUIRE(receivedAct.services["python-json"].find("o1") != receivedAct.services["python-json"].end());
        REQUIRE(receivedAct.services["python-json"].find("o2") != receivedAct.services["python-json"].end());


        auto removeResult = AccountManager::Instance().deleteAccount(act.id);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(removeResult);
    }

    SECTION("CPP Plugin with custom read/write") {
        GLOG_INF("TEST STARTED: CPP Plugin with custom read/write");
        REQUIRE(TESTCOMMON::ACCOUNTS::cleanup("my_custom_cpp_account"));

        AccountObject act;
        act.title = "my custom cpp account";
        act.services["cpp-custom"].init(&act, "cpp-custom");
        act.services["cpp-custom"]["k1"] = "v1";
        act.services["cpp-custom"]["k2"] = "v2";
        bool createRes = AccountManager::Instance().createAccount(act);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(createRes);
        REQUIRE_FALSE(uuid_is_null(act.id));

        AccountObject receivedAct;
        bool readResult = AccountManager::Instance().readAccount(act.id, &receivedAct);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(readResult);

        REQUIRE(receivedAct.services.size() == 1);
        REQUIRE(receivedAct.services.find("cpp-custom") != receivedAct.services.end());

        REQUIRE(receivedAct.services["cpp-custom"].find("k1") != receivedAct.services["cpp-custom"].end());
        CHECK(receivedAct.services["cpp-custom"]["k1"] == "v1");
        CHECK(receivedAct.services["cpp-custom"]["k2"] == "v2");


        auto removeResult = AccountManager::Instance().deleteAccount(act.id);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(removeResult);
    }

    SECTION("retrieve account only with protected params") {
        GLOG_INF("TEST STARTED: retrieve account only with protected params");

        AccountObject act;
        act.title = "my protected account";
        REQUIRE(act.services["protected-test"].init(&act, "protected-test"));
        act.services["protected-test"]["param1"] = "value1";
        act.services["protected-test"]["param2"] = "value2";

        REQUIRE(TESTCOMMON::ACCOUNTS::cleanup(act.title));
        // string actName = PXUTILS::ACCOUNT::title2name(act.title);
        bool createResult = AccountManager::Instance().createAccount(act);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(createResult);
        REQUIRE_FALSE(uuid_is_null(act.id));

        AccountObject receivedAct;
        bool readResult = AccountManager::Instance().readAccount(act.id, &receivedAct);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(readResult);

        REQUIRE(receivedAct.services.size() == 1);
        REQUIRE(receivedAct.services.find("protected-test") != receivedAct.services.end());
    }
}
