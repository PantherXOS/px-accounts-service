//
// Created by Reza Alizadeh Majd on 2018-12-03.
//

#include <catch2/catch.hpp>

#include <PluginManager.h>
#include <iostream>


TEST_CASE("Plugin Management Tasks", "[PluginManager]") {

    auto &mgr = PluginManager::Instance();

    REQUIRE(mgr.plugins().size() > 0);

    for (auto &kv : mgr.plugins()) {
        auto & plugin = kv.second;

        REQUIRE(kv.first == plugin.getTitle());

        StrStrMap params;
        params["k1"] = "v1";

        auto vResult = plugin.verify(params);
        REQUIRE_FALSE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors[0] == string("Param 'k2' is required"));

        params["k2"] = "v2";
        vResult = plugin.verify(params);
        REQUIRE(vResult.verified);
        CAPTURE(vResult.errors);
        REQUIRE(vResult.errors.size() == 0);
    }
}