//
// Created by Reza Alizadeh Majd on 2019-06-16.
//


#include <Secret/SecretManager.h>
#include <Accounts/AccountUtils.h>
#include <interface/Secret.capnp.h>
#include <capnp/ez-rpc.h>

Logger gLogger("secret-tester");

int main() {

    setvbuf(stdout, nullptr, _IONBF, 0);
    GLOG_INIT(LogTarget::CONSOLE, LogLevel::INF);

    string serverPath = "unix:/tmp/" + PXUTILS::SYSTEM::current_user() +  "/rpc/secret";

    SecretManager::Init(serverPath);


    uuid_t aid;
    uuid_generate(aid);

    SecretItemBase secret;
    secret.label = "test_secret";
    secret.attributes["service"] = "test_svc";
    secret.attributes["accountId"] = uuid_as_string(aid);
    secret.attributes["username"] = "foo";
    secret.attributes["schema"] = "password";
    secret.secrets["password"] = "bar";
    

    bool res = SecretManager::Instance().setSecret(secret);
    cout << "set val: " << res << endl;


    auto receivedVal = SecretManager::Instance().getAccountSecrets(aid);
    // cout << "get val: " << receivedVal << endl;
}
