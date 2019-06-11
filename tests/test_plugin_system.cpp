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

void register_test_plugin(const string &name, const string &version, const string &type, const string &path) {
    mkdir("./plugins", S_IRWXU | S_IRWXG | S_IRWXO);
    string pluginFileName = "./plugins/" + name + ".yaml";
    if (PXUTILS::FILE::exists(pluginFileName)) {
        PXUTILS::FILE::remove(pluginFileName);
    }
    ofstream pluginStream;
    pluginStream.open(pluginFileName);
    pluginStream << "plugin:\n"
                 << "   name: " << name << "\n"
                 << "   version: " << version << "\n"
                 << "   type: " << type << "\n"
                 << "   path: " << path << "\n";
}

TEST_CASE("Plugin Management Tasks", "[PluginManager]") {

    register_test_plugin("px-accounts-service-plugin-cpp-test",
                         "0.0.1",
                         "cpp",
                         "../cpp-test-plugin/libpx-accounts-service-plugin-cpp-test.dylib");
    register_test_plugin("px-accounts-service-plugin-python-test",
                         "0.0.1",
                         "python",
                         ".");

    auto &mgr = PluginManager::Instance();

    SECTION("Check Loaded Plugins") {

        REQUIRE(mgr.plugins().size() > 0);
        REQUIRE(mgr.exists("python-test"));
        REQUIRE(mgr.exists("cpp-test"));
    }

    SECTION("Check Python Plugin - verify Method") {

        REQUIRE(mgr.exists("python-test"));

        StrStrMap params;
        params["k1"] = "v1";

        auto vResult = mgr["python-test"].verify(params);
        REQUIRE_FALSE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors[0] == string("Param 'k2' is required"));

        params["k2"] = "v2";
        vResult = mgr["python-test"].verify(params);
        REQUIRE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors.empty());
    }

    SECTION("Check Python Plugin - authenticate method.") {
        REQUIRE(mgr.exists("python-test"));

        StrStrMap params;
        params["k1"] = "v1";
        params["k2"] = "v2";
        auto vResult = mgr["python-test"].verify(params);
        REQUIRE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors.empty());

        auto authResult = mgr["python-test"].authenticate(vResult.params);
        REQUIRE(authResult.authenticated);
        CAPTURE(authResult.errors);
        REQUIRE(authResult.errors.empty());
    }

    SECTION("Check CPP Plugin") {
        REQUIRE(mgr.exists("cpp-test"));

        StrStrMap params;
        params["k1"] = "v1";
        params["k2"] = "v2";
        auto vResult = mgr["cpp-test"].verify(params);
        REQUIRE(vResult.verified);
        REQUIRE(vResult.params.size() == 4);
    }
}