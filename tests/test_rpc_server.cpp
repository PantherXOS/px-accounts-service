//
// Created by Reza Alizadeh Majd on 11/12/18.
//

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <RPCServer.h>
#include <capnp/ez-rpc.h>
#include <interface/AccountReader.capnp.h>
#include <interface/AccountWriter.capnp.h>
#include <AccountParser.h>
#include <iostream>

#define SERVER_ADDRESS "127.0.0.1:1234"

int main(int argc, char *argv[]) {

    RPCServer srv(SERVER_ADDRESS);
    srv.start();

    int result = Catch::Session().run(argc, argv);

    srv.stop();

    return result;
}

bool ac2rpc(const PXParser::AccountObject &ac, Account::Builder &rpc)
{
    rpc.setTitle(ac.title);
    rpc.setProvider(ac.provider);
    rpc.setActive(ac.is_active);

    int i, j;
    i = 0;
    auto rpcSettings = rpc.initSettings(static_cast<unsigned int>(ac.settings.size()));
    for (const auto &kv : ac.settings) {
        rpcSettings[i].setKey(kv.first);
        rpcSettings[i].setValue(kv.second);
        i++;
    }

    i = 0;
    auto rpcServices = rpc.initServices(static_cast<unsigned int>(ac.services.size()));
    for (const auto &svc : ac.services) {
        j = 0;
        rpcServices[i].setName(svc.first);
        auto svcParams = rpcServices[i].initParams(svc.second.size());
        for (const auto &p : svc.second) {
            svcParams[j].setKey(p.first);
            svcParams[j].setValue(p.second);
            j++;
        }
        i++;
    }
    return true;
}

bool rpc2ac(const Account::Reader &rpc, PXParser::AccountObject &act)
{
    act.title = rpc.getTitle().cStr();
    act.provider = rpc.getProvider().cStr();
    act.is_active = rpc.getActive();

    for (const auto &s : rpc.getSettings()) {
        act.settings[s.getKey().cStr()] = s.getValue().cStr();
    }

    for (const auto &svc : rpc.getServices()) {
        for (const auto &p : svc.getParams()) {
            act.services[svc.getName().cStr()][p.getKey().cStr()] = p.getValue().cStr();
        }
    }
    return true;
}


TEST_CASE("Account Writer Tests", "[RPCServer]") {

    std::string title1 = "RPC Test Account";
    std::string title2 = "RPC Test Account Edited";

    PXParser::AccountObject act;
    act.title = title1;
    act.provider = "sample provider";
    act.is_active = false;

    act.settings["first key"] = "first value";
    act.settings["second key"] = "second value";

    act.services["svc1"]["param11"] = "val11";
    act.services["svc1"]["param12"] = "val12";
    act.services["svc1"]["param13"] = "val13";

    act.services["svc2"]["param21"] = "val21";
    act.services["svc2"]["param22"] = "val22";
    act.services["svc2"]["param23"] = "val23";


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
        ac2rpc(act, account);

        auto request = client.addRequest();
        request.setAccount(account);

        auto response = request.send().wait(waitScope);
        REQUIRE(response.getResult());

        auto getReq = client.getRequest();
        getReq.setTitle(PXUTILS::ACCOUNT::title2name(act.title));
        auto getRes = getReq.send().wait(waitScope);
        REQUIRE(getRes.hasAccount());

        PXParser::AccountObject savedAct;
        rpc2ac(getRes.getAccount(), savedAct);

        REQUIRE(savedAct.title == act.title);
        REQUIRE(savedAct.provider == act.provider);
        REQUIRE(savedAct.is_active == act.is_active);
        REQUIRE(savedAct.settings.size() == act.settings.size());
        REQUIRE(savedAct.services.size() == act.services.size());
        for (const auto &svc : act.services) {
            REQUIRE(savedAct.services.find(svc.first) != savedAct.services.end());
            for (const auto &kv: svc.second) {
                REQUIRE(savedAct.services[svc.first].find(kv.first) != savedAct.services[svc.first].end());
                REQUIRE(savedAct.services[svc.first][kv.first] == kv.second);
            }
        }
    }

    SECTION("Edit Account") {

        auto getReq = client.getRequest();
        getReq.setTitle(PXUTILS::ACCOUNT::title2name(act.title));
        auto getRes = getReq.send().wait(waitScope);
        REQUIRE(getRes.hasAccount());

        PXParser::AccountObject testAct;
        rpc2ac(getRes.getAccount(), testAct);
        testAct.title = title2;

        capnp::MallocMessageBuilder msg;
        Account::Builder account = msg.initRoot<Account>();
        ac2rpc(testAct, account);

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

    SECTION("List imap Accounts") {
        auto listReq = client.listRequest();
        auto serviceFilter = listReq.initServiceFilter(1);
        serviceFilter.set(0, "imap");
        auto listRes = listReq.send().wait(waitScope);
        REQUIRE(listRes.hasAccounts());
        if (listRes.getAccounts().size() == 0)
            WARN("'imap' accounts not found");
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
