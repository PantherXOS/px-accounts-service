//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "RPCHandler.h"
#include <kj/async.h>
#include <string>
#include <kj/debug.h>

#include "Accounts/AccountManager.h"

using namespace std;

kj::Promise<void> RPCHandler::list(AccountReader::Server::ListContext ctx) {
    GLOG_INF("==========================================================");

    vector<string> pFilter;
    vector<string> sFilter;

    if (ctx.getParams().hasProviderFilter()) {
        for (auto p : ctx.getParams().getProviderFilter()) {
            pFilter.emplace_back(p.cStr());
        }
    }
    if (ctx.getParams().hasServiceFilter()) {
        for (auto s : ctx.getParams().getServiceFilter()) {
            sFilter.emplace_back(s.cStr());
        }
    }

    vector<string> accountList = AccountManager::Instance().listAccounts(pFilter, sFilter);

    auto result = ctx.getResults().initAccounts(accountList.size());
    for (int i = 0; i < accountList.size(); ++i) {
        result.set(i, accountList[i]);
    }

    return kj::READY_NOW;
}

kj::Promise<void> RPCHandler::get(AccountReader::Server::GetContext ctx) {
    GLOG_INF("==========================================================");

    KJ_REQUIRE(ctx.getParams().hasTitle(), "'title' parameter not set");

    auto title = ctx.getParams().getTitle().cStr();
    auto actName = PXUTILS::ACCOUNT::title2name(title);
    AccountObject actObj;
    bool res = AccountManager::Instance().readAccount(actName, &actObj);
    for (const auto &err : AccountManager::LastErrors()) {
        KJ_DBG(err);
    }
    KJ_ASSERT(res, "Account not found");

    auto account = ctx.getResults().initAccount();
    account.setTitle(actObj.title);
    account.setProvider(actObj.provider);
    account.setActive(actObj.is_active);

    auto accountSettings = account.initSettings(actObj.settings.size());
    int i, j;
    i = 0;
    for (const auto &kv: actObj.settings) {
        accountSettings[i].setKey(kv.first);
        accountSettings[i].setValue(kv.second);
        i++;
    }

    auto accountServices = account.initServices(actObj.services.size());
    i = 0;
    for (const auto &svc : actObj.services) {
        accountServices[i].setName(svc.first);
        auto params = accountServices[i].initParams(svc.second.size());
        j = 0;
        for (const auto &kv : svc.second) {
            params[j].setKey(kv.first);
            params[j].setValue(kv.second);
            j++;
        }
        i++;
    }
    return kj::READY_NOW;
}

kj::Promise<void> RPCHandler::setStatus(AccountReader::Server::SetStatusContext ctx) {
    GLOG_INF("==========================================================");

    KJ_REQUIRE(ctx.getParams().hasTitle(), "'title' parameter not set");

    auto title = ctx.getParams().getTitle().cStr();
    auto accountName = PXUTILS::ACCOUNT::title2name(title);
    auto stat = (AccountStatus) ctx.getParams().getStat();
    bool res = AccountManager::Instance().setStatus(accountName, stat);
    for (const auto &err : AccountManager::LastErrors()) {
        KJ_DBG(err);
    }
    KJ_ASSERT(res, "set account status failed");
    ctx.getResults().setResult(true);

    return kj::READY_NOW;
}

kj::Promise<void> RPCHandler::getStatus(AccountReader::Server::GetStatusContext ctx) {
    GLOG_INF("==========================================================");

    KJ_REQUIRE(ctx.getParams().hasTitle(), "'title' parameter not set");

    auto title = ctx.getParams().getTitle().cStr();
    auto accountName = PXUTILS::ACCOUNT::title2name(title);
    AccountStatus stat = AccountManager::Instance().getStatus(accountName);
    ctx.getResults().setStatus((Account::Status) stat);

    return kj::READY_NOW;
}

kj::Promise<void> RPCHandler::add(AccountWriter::Server::AddContext ctx) {
    GLOG_INF("==========================================================");

    KJ_REQUIRE(ctx.getParams().hasAccount(), "'account' parameter is not set");

    auto rpcAccount = ctx.getParams().getAccount();
    AccountObject account;
    KJ_ASSERT(RPCHandler::RPC2ACT(rpcAccount, account), "Error on parse received account");

    bool res = AccountManager::Instance().createAccount(account);
    if (!res) {
        for (const auto &err : AccountManager::LastErrors()) {
            KJ_DBG(err);
        }
        KJ_ASSERT(res, "Create new account failed.");
    } else {
        auto warnings = ctx.getResults().initWarnings(AccountManager::LastErrors().size());
        int i = 0;
        for (const auto &wrn : AccountManager::LastErrors()) {
            warnings.set(i++, wrn);
        }
    }
    ctx.getResults().setResult(res);
    return kj::READY_NOW;
}

kj::Promise<void> RPCHandler::edit(AccountWriter::Server::EditContext ctx) {
    GLOG_INF("==========================================================");

    KJ_REQUIRE(ctx.getParams().hasTitle(), "'title' parameter is not set");
    KJ_REQUIRE(ctx.getParams().hasAccount(), "'account' parameter is not set");

    auto title = ctx.getParams().getTitle().cStr();
    auto actName = PXUTILS::ACCOUNT::title2name(title);
    auto rpcAccount = ctx.getParams().getAccount();
    AccountObject account;
    KJ_ASSERT(RPCHandler::RPC2ACT(rpcAccount, account), "Error on parse received account");
    bool res = AccountManager::Instance().modifyAccount(actName, account);
    if (!res) {
        for (const auto &err : AccountManager::LastErrors()) {
            KJ_DBG(err);
        }
        KJ_ASSERT(res, "Modify Existing Account failed.");
    } else {
        auto warnings = ctx.getResults().initWarnings(AccountManager::LastErrors().size());
        int i = 0;
        for (const auto &wrn : AccountManager::LastErrors()) {
            warnings.set(i++, wrn);
        }
    }
    ctx.getResults().setResult(true);

    return kj::READY_NOW;
}

kj::Promise<void> RPCHandler::remove(AccountWriter::Server::RemoveContext ctx) {
    GLOG_INF("==========================================================");

    KJ_REQUIRE(ctx.getParams().hasTitle(), "'title' parameter is not set");

    auto title = ctx.getParams().getTitle().cStr();
    auto actName = PXUTILS::ACCOUNT::title2name(title);
    bool res = AccountManager::Instance().deleteAccount(actName);
    for (const auto &err : AccountManager::LastErrors()) {
        KJ_DBG(err);
    }
    KJ_ASSERT(res, "Remove Account Failed");
    ctx.getResults().setResult(true);

    return kj::READY_NOW;
}

bool RPCHandler::RPC2ACT(const Account::Reader &rpc, AccountObject &act) {
    act.title = rpc.getTitle().cStr();
    act.provider = rpc.getProvider().cStr();
    act.is_active = rpc.getActive();

    for (const auto &s : rpc.getSettings()) {
        act.settings[s.getKey().cStr()] = s.getValue().cStr();
    }

    for (const auto &svc : rpc.getServices()) {
        string svcName = svc.getName().cStr();
        act.services[svcName].init(&act, svcName);
        if (!act.services[svcName].inited()) {
            return false;
        }
        for (const auto &p : svc.getParams()) {
            string key = p.getKey().cStr();
            string val = p.getValue().cStr();
            act.services[svcName][key] = val;
        }
    }
    return true;
}

bool RPCHandler::ACT2RPC(const AccountObject &act, Account::Builder &rpc) {
    rpc.setTitle(act.title);
    rpc.setProvider(act.provider);
    rpc.setActive(act.is_active);

    int i, j;
    i = 0;
    auto rpcSettings = rpc.initSettings(static_cast<unsigned int>(act.settings.size()));
    for (const auto &kv : act.settings) {
        rpcSettings[i].setKey(kv.first);
        rpcSettings[i].setValue(kv.second);
        i++;
    }

    i = 0;
    auto rpcServices = rpc.initServices(static_cast<unsigned int>(act.services.size()));
    for (const auto &svc : act.services) {
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
