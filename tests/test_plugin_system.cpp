//
// Created by Reza Alizadeh Majd on 2018-12-03.
//

#include <catch2/catch.hpp>

#include <Plugins/PluginManager.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <AccountUtils.h>


TEST_CASE("Plugin Management Tasks", "[PluginManager]") {

    auto &mgr = PluginManager::Instance();

    SECTION("Check Loaded Plugins") {

        REQUIRE(mgr.plugins().size() > 0);
        REQUIRE(mgr["python-test"] != nullptr);
        REQUIRE(mgr["cpp-test"] != nullptr);
    }

    SECTION("Check Python Plugin - verify Method") {

        REQUIRE(mgr["python-test"] != nullptr);

        StrStrMap params;
        params["k1"] = "v1";

        auto vResult = mgr["python-test"]->verify(params);
        REQUIRE_FALSE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors[0] == string("Param 'k2' is required"));

        params["k2"] = "v2";
        vResult = mgr["python-test"]->verify(params);
        REQUIRE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors.empty());
    }

    SECTION("Check Python Plugin - authenticate method.") {

        REQUIRE(mgr["python-test"] != nullptr);

        StrStrMap params;
        params["k1"] = "v1";
        params["k2"] = "v2";
        auto vResult = mgr["python-test"]->verify(params);
        REQUIRE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors.empty());

        auto authResult = mgr["python-test"]->authenticate(vResult.params);
        REQUIRE(authResult.authenticated);
        CAPTURE(authResult.errors);
        if (authResult.errors.size() > 0) {
            REQUIRE(authResult.errors.size() == 1);
            REQUIRE(authResult.errors[0] == "sample warning");
        }
    }

    SECTION("Check CPP Plugin") {

        REQUIRE(mgr["cpp-test"] != nullptr);

        StrStrMap params;
        params["k1"] = "v1";
        params["k2"] = "v2";
        auto vResult = mgr["cpp-test"]->verify(params);
        REQUIRE(vResult.verified);
        REQUIRE(vResult.params.size() == 4);
    }
}