//
// Created by Reza Alizadeh Majd on 2019-01-17.
//

//
// Created by Reza Alizadeh Majd on 2018-12-03.
//

#include <catch2/catch.hpp>

#include <ProviderHandler.h>
#include <AccountUtils.h>
#include <iostream>


TEST_CASE("Provider Related Tests", "[ProviderHandler]") {

    auto &mgr = ProviderHandler::Instance();
    string providerName = "test_provider";

    SECTION("Check Loaded Providers") {
        REQUIRE(mgr.exists(providerName));

        ProviderStruct &provider = mgr[providerName];
        REQUIRE(EXISTS(provider.plugins, "test"));
        REQUIRE(EXISTS(provider.plugins["test"], "k1"));
        REQUIRE(EXISTS(provider.plugins["test"], "k3"));
    }
}
