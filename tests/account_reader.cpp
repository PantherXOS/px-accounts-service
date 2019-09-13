//
// Created by Reza Alizadeh Majd on 11/20/18.
//

#include <iostream>
#include <unistd.h>
#include <RPCServer.h>
#include <capnp/ez-rpc.h>
#include <Accounts/AccountParser.h>
#include <interface/AccountReader.capnp.h>

#define SERVER_ADDRESS "~/.userdata/rpc/accounts"

using namespace std;

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

int main() {

    RPCServer srv;
    srv.start();

    usleep(10000);

    string addr = string("unix:") + PXUTILS::FILE::abspath(SERVER_ADDRESS);
    capnp::EzRpcClient rpcClient(addr);
    auto &waitScope = rpcClient.getWaitScope();
    AccountReader::Client client = rpcClient.getMain<AccountReader>();

    auto listReq = client.listRequest();
    auto listRes = listReq.send().wait(waitScope);

    for (const auto &title : listRes.getAccounts()) {
        auto getReq = client.getRequest();
        getReq.setTitle(title);
        auto getRes = getReq.send().wait(waitScope);

        PXParser::AccountObject act;
        rpc2ac(getRes.getAccount(), act);
        PXParser::print_account(act);

    }
    srv.stop();
    return 0;
}
