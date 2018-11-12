//
// Created by Reza Alizadeh Majd on 11/12/18.
//

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include <AccountManager.h>
#include <iostream>


TEST_CASE("Account Management Tasks", "[AccountManager]") {


    std::string title1 = "My Test Account";
    std::string title2 = "My Edited Title";

    PXParser::AccountObject newAccount, account;
    newAccount.title = title1;
    newAccount.provider = "sample provider";
    newAccount.is_active = false;

    newAccount.settings["first key"] = "first value";
    newAccount.settings["second key"] = "second value";

    newAccount.services["svc1"]["param1"] = "val1";
    newAccount.services["svc1"]["param2"] = "val2";
    newAccount.services["svc1"]["param3"] = "val3";

    newAccount.services["svc2"]["param1"] = "val1";
    newAccount.services["svc2"]["param2"] = "val2";
    newAccount.services["svc2"]["param3"] = "val3";


    string accountName = PXUTILS::ACCOUNT::title2name(newAccount.title);

    SECTION("Cleanup Old Test files") {
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name(title1)));
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name(title2)));
    }

    SECTION("Create New Account") {
        REQUIRE(AccountManager::Instance().createAccount(newAccount));
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
        REQUIRE(newAccount.services.size() == 2);
        REQUIRE(newAccount.services.find("svc1") != newAccount.services.end());
        REQUIRE(newAccount.services["svc1"].size() == 3);
        REQUIRE(newAccount.services.find("svc2") != newAccount.services.end());
        REQUIRE(newAccount.services["svc2"].size() == 3);
    }

    SECTION("Change Account active status") {
        REQUIRE(AccountManager::Instance().readAccount(accountName, &account));

        account.is_active = true;
        REQUIRE(AccountManager::Instance().modifyAccount(accountName, account));

        REQUIRE(AccountManager::Instance().readAccount(accountName, &account));
        REQUIRE(account.is_active == true);
    }

    SECTION("Change Account Title") {
        REQUIRE(AccountManager::Instance().readAccount(accountName, &account));

        account.title = title2;
        REQUIRE(AccountManager::Instance().modifyAccount(accountName, account));

        string newAccountName = PXUTILS::ACCOUNT::title2name(account.title);
        REQUIRE_FALSE(AccountManager::Instance().readAccount(accountName, &account));
        REQUIRE(AccountManager::Instance().readAccount(newAccountName, &account));
    }

    SECTION("Delete Account") {
        accountName = PXUTILS::ACCOUNT::title2name(title2);
        REQUIRE(AccountManager::Instance().deleteAccount(accountName));
    }
}
