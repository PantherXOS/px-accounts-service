//
// Created by Reza Alizadeh Majd on 11/12/18.
//

//#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <iostream>

#include <AccountManager.h>
#include <AccountParser.h>


TEST_CASE("Account Management Tasks", "[AccountManager]") {


    std::string title1 = "My Test Account";
    std::string title2 = "My Edited Title";
    std::string title3 = "Test Provider Account";

    AccountObject newAccount, account;
    newAccount.title = title1;
    newAccount.is_active = false;

    newAccount.settings["first key"] = "first value";
    newAccount.settings["second key"] = "second value";

    newAccount.services["python-test"]["k1"] = "v1";
    newAccount.services["python-test"]["k2"] = "v2";


    string accountName = PXUTILS::ACCOUNT::title2name(newAccount.title);

    SECTION("Cleanup Old Test files") {
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name(title1)));
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name(title2)));
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name(title3)));
    }

    SECTION("Create New Account") {
        bool createResult = AccountManager::Instance().createAccount(newAccount);
        if (!createResult) {
            for (const auto &err : AccountManager::LastErrors()) {
                WARN(err);
            }
        } else {
            if (AccountManager::LastErrors().size() > 0) {
                REQUIRE(AccountManager::LastErrors().size() == 1);
                REQUIRE(AccountManager::LastErrors()[0] == "sample warning");
            }
        }
        REQUIRE(createResult);
    }

    SECTION("Read Created Account") {
        REQUIRE(AccountManager::Instance().readAccount(accountName, &account));
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
        REQUIRE(AccountManager::Instance().readAccount(accountName, &account));

        account.is_active = true;
        bool modifyResult = AccountManager::Instance().modifyAccount(accountName, account);
        if (!modifyResult) {
            for (const auto &err : AccountManager::LastErrors()) {
                WARN(err);
            }
        } else {
            if (AccountManager::LastErrors().size() > 0) {
                REQUIRE(AccountManager::LastErrors().size() == 1);
                REQUIRE(AccountManager::LastErrors()[0] == "sample warning");
            }
        }
        REQUIRE(modifyResult);

        REQUIRE(AccountManager::Instance().readAccount(accountName, &account));
        REQUIRE(account.is_active);
    }

    SECTION("Change Account Title") {
        REQUIRE(AccountManager::Instance().readAccount(accountName, &account));

        account.title = title2;
        bool modifyResult = AccountManager::Instance().modifyAccount(accountName, account);
        if (!modifyResult) {
            for (const auto &err : AccountManager::LastErrors()) {
                WARN(err);
            }
        } else {
            if (AccountManager::LastErrors().size() > 0) {
                REQUIRE(AccountManager::LastErrors().size() == 1);
                REQUIRE(AccountManager::LastErrors()[0] == "sample warning");
            }
        }
        REQUIRE(modifyResult);

        string newAccountName = PXUTILS::ACCOUNT::title2name(account.title);
        REQUIRE_FALSE(AccountManager::Instance().readAccount(accountName, &account));
        REQUIRE(AccountManager::Instance().readAccount(newAccountName, &account));
    }

    SECTION("Delete Account") {
        accountName = PXUTILS::ACCOUNT::title2name(title2);
        REQUIRE(AccountManager::Instance().deleteAccount(accountName));
    }

    SECTION("Create Account with Provider") {
        AccountObject providerAccount;
        providerAccount.title = title3;
        providerAccount.provider = "test_provider";
        providerAccount.services["python-test"]["k2"] = "v2";

        bool createResult = AccountManager::Instance().createAccount(providerAccount);
        if (!createResult) {
            for (const auto &err : AccountManager::LastErrors()) {
                WARN(err);
            }
        } else {
            if (AccountManager::LastErrors().size() > 0) {
                REQUIRE(AccountManager::LastErrors().size() == 1);
                REQUIRE(AccountManager::LastErrors()[0] == "sample warning");
            }
        }
        REQUIRE(createResult);

        string providerActName = PXUTILS::ACCOUNT::title2name(title3);
        REQUIRE(AccountManager::Instance().readAccount(providerActName, &account));
        REQUIRE(providerAccount.provider == account.provider);
        REQUIRE(EXISTS(providerAccount.services, "python-test"));
        REQUIRE(EXISTS(providerAccount.services["python-test"], "k1"));
        REQUIRE(EXISTS(providerAccount.services["python-test"], "k2"));
        REQUIRE(EXISTS(providerAccount.services["python-test"], "k3"));

        REQUIRE(providerAccount.services["python-test"]["k1"] == "provider_val1");
        REQUIRE(providerAccount.services["python-test"]["k2"] == "v2");
        REQUIRE(providerAccount.services["python-test"]["k3"] == "provider_val3");

        REQUIRE(AccountManager::Instance().deleteAccount(providerActName));
    }

    SECTION("Modify Account with Public Plugin") {

        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name("my_public_account")));
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name("modified_public_act")));

        AccountObject act;
        act.title = "my_public_account";
        act.services["public-test"];
        bool createRes = AccountManager::Instance().createAccount(act);
        (AccountManager::LastErrors());
        REQUIRE(createRes);

        AccountObject savedAct;
        REQUIRE(AccountManager::Instance().readAccount(act.title, &savedAct));
        REQUIRE(savedAct.services.find("public-test") != savedAct.services.end());
        savedAct.title = "modified_public_act";
        bool modifyRes = AccountManager::Instance().modifyAccount(act.title, savedAct);
        CAPTURE(AccountManager::LastErrors());
        REQUIRE(modifyRes);

    }
}
