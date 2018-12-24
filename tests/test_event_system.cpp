//
// Created by Reza Alizadeh Majd on 2018-12-23.
//

#include <cstring>
#include <thread>
#include <chrono>
#include <catch2/catch.hpp>
#include <capnp/ez-rpc.h>
#include <capnp/serialize-packed.h>
#include <nng/nng.h>
#include <nng/protocol/pubsub0/sub.h>

#include <AccountDefinitions.h>
#include <AccountParser.h>
#include <interface/AccountWriter.capnp.h>
#include <RPCServer.h>
#include <RPCHandler.h>
#include <EventManager.h>

#include "test_common.h"

#define  IPC_PATH   "~/.userdata/events/accounts"

TEST_CASE("Event System Tests", "[EventSystem]") {

    AccountObject act;
    act.title = "event_test_account";
    act.provider = "sample provider";
    act.is_active = true;
    act.services["test"]["k1"] = "v1";
    act.services["test"]["k2"] = "v2";


    SECTION("Cleanup old test files") {
        REQUIRE(PXParser::remove(PXUTILS::ACCOUNT::title2name(act.title)));
    }

    SECTION("Create Test Accounet") {
        capnp::EzRpcClient rpcClient(SERVER_ADDRESS);
        kj::WaitScope &waitScope = rpcClient.getWaitScope();
        AccountWriter::Client client = rpcClient.getMain<AccountWriter>();

        capnp::MallocMessageBuilder msg;
        Account::Builder rpcAct = msg.initRoot<Account>();
        RPCHandler::ACT2RPC(act, rpcAct);

        auto request = client.addRequest();
        request.setAccount(rpcAct);
        auto response = request.send().wait(waitScope);
        REQUIRE(response.getResult());
    }

    SECTION("Test Subscriber") {
        nng_socket subSock;
        EventManager::Instance();


        REQUIRE(nng_sub0_open(&subSock) == 0);
        REQUIRE(nng_setopt(subSock, NNG_OPT_SUB_SUBSCRIBE, "", 0) == 0);

        string ipcSock = string("ipc://") + PXUTILS::FILE::abspath(IPC_PATH);
        CAPTURE(ipcSock.c_str());
        REQUIRE(nng_dial(subSock, ipcSock.c_str(), NULL, 0) == 0);
        INFO("DIAL finished");

        std::thread statThread = std::thread([&]() {
            INFO("Client Thread");
            capnp::EzRpcClient rpcClient(SERVER_ADDRESS);
            kj::WaitScope &waitScope = rpcClient.getWaitScope();
            AccountWriter::Client client = rpcClient.getMain<AccountWriter>();

            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto setReq = client.setStatusRequest();
            setReq.setTitle(PXUTILS::ACCOUNT::title2name(act.title));
            setReq.setStat(Account::Status::ONLINE);
            setReq.send().wait(waitScope);
            INFO("Send DONE");
        });

        unsigned char *buff;
        size_t sz;
        REQUIRE(nng_recv(subSock, &buff, &sz, NNG_FLAG_ALLOC) == 0);
        CAPTURE(sz);

        kj::ArrayPtr<uint8_t> data(buff, sz);
        kj::ArrayInputStream strm(data);
        capnp::InputStreamMessageReader reader(strm);
        Account::AccountEvent::Reader evtData = reader.getRoot<Account::AccountEvent>();
        REQUIRE(evtData.getSource().cStr() == act.title);
        REQUIRE(evtData.getType() == Account::EventType::STATUS_CHANGE);
        bool oldFound = false, newFound = false;
        for (const auto &param : evtData.getParams()) {
            if (param.getKey().cStr() == string("old")) {
                REQUIRE(param.getValue().cStr() == AccounrStatusString[AC_NONE]);
                oldFound = true;
            }
            if (param.getKey().cStr() == string("new")) {
                REQUIRE(param.getValue().cStr() == AccounrStatusString[AC_ONLINE]);
                newFound = true;
            }
        }
        REQUIRE((oldFound && newFound));
        nng_free(buff, sz);

        statThread.join();
    }
}
