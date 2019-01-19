//
// Created by Reza Alizadeh Majd on 2018-12-03.
//

#include <catch2/catch.hpp>

#include <PluginManager.h>
#include <iostream>


TEST_CASE("Plugin Management Tasks", "[PluginManager]") {

    auto &mgr = PluginManager::Instance();

    SECTION("Check Loaded Plugins") {

        REQUIRE(mgr.plugins().size() > 0);
        REQUIRE(mgr.exists("test"));
    }

    SECTION("Check verify Method") {

        REQUIRE(mgr.exists("test"));

        StrStrMap params;
        params["k1"] = "v1";

        auto vResult = mgr["test"].verify(params);
        REQUIRE_FALSE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors[0] == string("Param 'k2' is required"));

        params["k2"] = "v2";
        vResult = mgr["test"].verify(params);
        REQUIRE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors.empty());
    }

    SECTION("Check authenticate method.") {
        REQUIRE(mgr.exists("test"));

        StrStrMap params;
        params["k1"] = "v1";
        params["k2"] = "v2";
        auto vResult = mgr["test"].verify(params);
        REQUIRE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors.empty());

        auto authResult = mgr["test"].authenticate(vResult.params);
        REQUIRE(authResult.authenticated);
        CAPTURE(authResult.errors);
        REQUIRE(authResult.errors.empty());
    }
}