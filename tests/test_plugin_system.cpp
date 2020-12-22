//
// Created by Reza Alizadeh Majd on 2018-12-03.
//

#include <catch2/catch.hpp>

#include <Plugins/PluginManager.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <Accounts/AccountUtils.h>


TEST_CASE("Plugin Management Tasks", "[PluginManager]") {

    auto &mgr = PluginManager::Instance();

    SECTION("Check Loaded Plugins") {

        REQUIRE(mgr.registeredPlugins().size() > 0);
        REQUIRE(mgr["python-test"] != nullptr);
        REQUIRE(mgr["python-json"] != nullptr);
        REQUIRE(mgr["cpp-test"] != nullptr);
        REQUIRE(mgr["cpp-custom"] != nullptr);
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

    SECTION("Check Python Plugin - custom read/write/delete methods") {
        PluginContainerBase *plugin = mgr["python-json"];
        REQUIRE(plugin != nullptr);

        StrStrMap params;
        params["k1"] = "v1";
        params["k2"] = "v2";

        auto vResult = plugin->verify(params);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.verified);

        auto aResult = plugin->authenticate(vResult.params);
        CAPTURE(aResult.errors);
        REQUIRE(aResult.authenticated);
        string writeID = plugin->write(vResult, aResult);
        REQUIRE_FALSE(writeID.empty());


        StrStrMap rResult = plugin->read(writeID);
        REQUIRE(rResult.size() == 4);
        CHECK(rResult["k1"] == "v1");
        CHECK(rResult["k2"] == "v2");
        CHECK(rResult["o1"] == "ov1");
        CHECK(rResult["o2"] == "ov2");


        bool dResult = plugin->remove(writeID);
        REQUIRE(dResult);
    }

    SECTION("Check C++ Plugin - custom read/write/delete methods") {
        PluginContainerBase *plugin = mgr["cpp-custom"];
        REQUIRE(plugin != nullptr);

        StrStrMap params;
        params["k1"] = "v1";
        params["k2"] = "v2";

        auto vResult = plugin->verify(params);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.verified);

        auto aResult = plugin->authenticate(vResult.params);
        CAPTURE(aResult.errors);
        REQUIRE(aResult.authenticated);
        string writeID = plugin->write(vResult, aResult);
        REQUIRE_FALSE(writeID.empty());


        StrStrMap rResult = plugin->read(writeID);
        REQUIRE(rResult.size() == 2);
        CHECK(rResult["k1"] == "v1");
        CHECK(rResult["k2"] == "v2");


        bool dResult = plugin->remove(writeID);
        REQUIRE(dResult);
    }
}