//
// Created by Reza Alizadeh Majd on 11/12/18.
//

//#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <capnp/ez-rpc.h>
#include <interface/AccountReader.capnp.h>
#include <interface/AccountWriter.capnp.h>
#include <AccountParser.h>
#include <iostream>

#include <RPCServer.h>
#include <RPCHandler.h>
#include "test_common.h"


TEST_CASE("Account Writer Tests", "[RPCServer]") {

    std::string title1 = "RPC Test Account";
    std::string title2 = "RPC Test Account Edited";

    AccountObject act;
    act.title = title1;
    act.is_active = false;

    act.settings["first key"] = "first value";
    act.settings["second key"] = "second value";

    act.services["test"]["k1"] = "v1";  // protected params need to be re-added during account modification
    act.services["test"]["k2"] = "v2";

    capnp::EzRpcClient rpcClient(SERVER_ADDRESS);
    auto& waitScope = rpcClient.getWaitScope();

    AccountWriter::Client client = rpcClient.getMain<AccountWriter>();

    SECTION("Cleanup Old Test files") {
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name(title1)));
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name(title2)));
    }

    SECTION("Create Account") {

        capnp::MallocMessageBuilder msg;
        Account::Builder account = msg.initRoot<Account>();
        RPCHandler::ACT2RPC(act, account);

        auto request = client.addRequest();
        request.setAccount(account);

        auto response = request.send().wait(waitScope);
        REQUIRE(response.getResult());

        auto getReq = client.getRequest();
        getReq.setTitle(PXUTILS::ACCOUNT::title2name(act.title));
        auto getRes = getReq.send().wait(waitScope);
        REQUIRE(getRes.hasAccount());

        AccountObject savedAct;
        RPCHandler::RPC2ACT(getRes.getAccount(), savedAct);

        REQUIRE(savedAct.title == act.title);
        REQUIRE(savedAct.provider == act.provider);
        REQUIRE(savedAct.is_active == act.is_active);
        REQUIRE(savedAct.settings.size() == act.settings.size());
        REQUIRE(savedAct.services.size() == act.services.size());
        REQUIRE(savedAct.services["test"]["k2"] == act.services["test"]["k2"]);
//        for (const auto &svc : act.services) {
//            REQUIRE(savedAct.services.find(svc.first) != savedAct.services.end());
//            for (const auto &kv: svc.second) {
//                cout << svc.first << " - " << kv.first << endl;
//                REQUIRE(savedAct.services[svc.first].find(kv.first) != savedAct.services[svc.first].end());
//                REQUIRE(savedAct.services[svc.first][kv.first] == kv.second);
//            }
//        }
    }

    SECTION("Edit Account") {

        auto getReq = client.getRequest();
        getReq.setTitle(PXUTILS::ACCOUNT::title2name(act.title));
        auto getRes = getReq.send().wait(waitScope);
        REQUIRE(getRes.hasAccount());

        AccountObject testAct;
        RPCHandler::RPC2ACT(getRes.getAccount(), testAct);
        testAct.title = title2;
        testAct.services["test"]["k1"] = "v1"; // protected params need to be re-added during account modification

        capnp::MallocMessageBuilder msg;
        Account::Builder account = msg.initRoot<Account>();
        RPCHandler::ACT2RPC(testAct, account);

        auto editReq = client.editRequest();
        editReq.setTitle(PXUTILS::ACCOUNT::title2name(title1));
        editReq.setAccount(account);

        auto response = editReq.send()
                .then(
                        [](capnp::Response<AccountWriter::EditResults> &&resp) {
                            REQUIRE(resp.getResult());
                        },
                        [](kj::Exception &&ex) {
                            std::cout << ex.getDescription().cStr() << std::endl;
                            REQUIRE(false);
                        });
        response.wait(waitScope);
    }

    SECTION("Delete Account") {
        auto rmReq = client.removeRequest();
        rmReq.setTitle(PXUTILS::ACCOUNT::title2name(title2));
        auto rmRes = rmReq.send().wait(waitScope);
        REQUIRE(rmRes.getResult());
    }
}

TEST_CASE("Account Reader Tests", "[RPCServer]") {

    capnp::EzRpcClient rpcClient(SERVER_ADDRESS);
    auto& waitScope = rpcClient.getWaitScope();

    AccountReader::Client client = rpcClient.getMain<AccountReader>();

    SECTION("List All Accounts") {
        auto listReq = client.listRequest();
        auto listRes = listReq.send().wait(waitScope);
        REQUIRE(listRes.hasAccounts());
        if (listRes.getAccounts().size() == 0)
            WARN("account list is empty");
    }

    SECTION("List test Accounts") {
        auto listReq = client.listRequest();
        auto serviceFilter = listReq.initServiceFilter(1);
        serviceFilter.set(0, "test");
        auto listRes = listReq.send().wait(waitScope);
        REQUIRE(listRes.hasAccounts());
        if (listRes.getAccounts().size() == 0)
            WARN("'test' accounts not found");
    }

    SECTION("Get Account Details") {
        auto listReq = client.listRequest();
        auto listRes = listReq.send().wait(waitScope);
        REQUIRE(listRes.hasAccounts());
        if (listRes.getAccounts().size() == 0)
            WARN("account list is empty");
        for (const auto &title : listRes.getAccounts()) {
            auto getReq = client.getRequest();
            getReq.setTitle(title);
            auto getRes = getReq.send().wait(waitScope);
            REQUIRE(getRes.hasAccount());
        }
    }

    SECTION("Account Status Test") {
        auto listReq = client.listRequest();
        auto listRes = listReq.send().wait(waitScope);
        REQUIRE(listRes.hasAccounts());
        if (listRes.getAccounts().size() == 0)
            WARN("account list is empty. test omitted.");
        else {
            auto title = listRes.getAccounts()[0];
            CAPTURE(title.cStr());

            auto getReq = client.getStatusRequest();
            getReq.setTitle(title);
            auto getRes = getReq.send().wait(waitScope);
            REQUIRE(getRes.getStatus() == Account::Status::NONE);

            auto setReq = client.setStatusRequest();
            setReq.setTitle(title);
            setReq.setStat(Account::Status::ONLINE);
            auto setRes = setReq.send().wait(waitScope);
            REQUIRE(setRes.getResult());

            getReq = client.getStatusRequest();
            getReq.setTitle(title);
            getRes = getReq.send().wait(waitScope);
            REQUIRE(getRes.getStatus() == Account::Status::ONLINE);

        }
    }
}
