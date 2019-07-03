//
// Created by Reza Alizadeh Majd on 2019-06-16.
//


#include <Secret/SecretManager.h>
#include <AccountUtils.h>
#include <interface/Secret.capnp.h>
#include <capnp/ez-rpc.h>


int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);
    gLogger.setLevel(Logger::LVL_INF);

    string serverPath = "unix:/root/.userdata/rpc/secret";

    SecretManager::Init(serverPath);

    std::string act = "test_act",
            svc = "testsvc",
            key = "testkey",
            val = "testval";

    bool res = SecretManager::Instance().Set(act, svc, key, val);
    cout << "set val: " << res << endl;

    string receivedVal = SecretManager::Instance().Get(act, svc, key);
    cout << "get val: " << receivedVal << endl;
}