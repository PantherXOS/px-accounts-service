#include <Accounts/AccountParser.h>

#include <catch2/catch.hpp>
#include <iostream>

#include "test_common.h"

TEST_CASE("Account Parser Tests", "[AccountParser]") {
    const string accountsFullPath = PXUTILS::FILE::abspath("./accounts/");
    system("mkdir -p ./accounts/");
    AccountParser accountWriter(accountsFullPath, false);
    AccountParser accountReader(accountsFullPath, true);

    // const string testAccountTitle = "parser test account";
    // const string testAccountName = PXUTILS::ACCOUNT::title2name(testAccountTitle);
    AccountObject testAccount;
    uuid_generate(testAccount.id);
    testAccount.title = "parser test account";
    testAccount.is_active = true;
    testAccount.services["test-python"].init(&testAccount, "test-python");
    testAccount.services["test-python"]["k1"] = "v1";
    testAccount.services["test-python"]["k2"] = "v2";

    TESTCOMMON::ACCOUNTS::cleanup(testAccount.title);

    SECTION("Create New Account") {
        REQUIRE(accountWriter.write(testAccount));

        AccountObject savedAccount;
        REQUIRE(accountReader.read(testAccount.id, savedAccount));
        CHECK(savedAccount.title == testAccount.title);
        CHECK(savedAccount.is_active == testAccount.is_active);
        CHECK(savedAccount.services.size() == testAccount.services.size());
        REQUIRE(savedAccount.services.find("test-python") != savedAccount.services.end());
    }

    SECTION("try to write on readonly parser") {
        REQUIRE_FALSE(accountReader.write(testAccount)); 
    }
}