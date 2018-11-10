//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "RPCHandler.h"
#include <kj/async.h>
#include <string>
#include <kj/debug.h>

#include "AccountManager.h"

using namespace std;

kj::Promise<void> RPCHandler::list(AccountReader::Server::ListContext ctx) {

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

    KJ_REQUIRE(ctx.getParams().hasTitle(), "'title' parameter not set");

    auto title = ctx.getParams().getTitle().cStr();
    PXParser::AccountObject actObj;
    KJ_ASSERT(AccountManager::Instance().readAccount(title, &actObj), "Account not found");

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

    KJ_REQUIRE(ctx.getParams().hasTitle(), "'title' parameter not set");

    auto title = ctx.getParams().getTitle().cStr();
    auto stat = (PXParser::AccountStatus)ctx.getParams().getStat();

    KJ_ASSERT(!AccountManager::Instance().setStatus(title, stat), "set account status failed");
    ctx.getResults().setResult(true);

    return kj::READY_NOW;
}

kj::Promise<void> RPCHandler::getStatus(AccountReader::Server::GetStatusContext ctx) {

    KJ_REQUIRE(ctx.getParams().hasTitle(), "'title' parameter not set");

    auto title = ctx.getParams().getTitle().cStr();
    PXParser::AccountStatus  stat = AccountManager::Instance().getStatus(title);
    ctx.getResults().setStatus((Account::Status)stat);

    return kj::READY_NOW;
}

kj::Promise<void> RPCHandler::add(AccountWriter::Server::AddContext ctx) {
    return Server::add(ctx);
}

kj::Promise<void> RPCHandler::edit(AccountWriter::Server::EditContext ctx) {
    return Server::edit(ctx);
}

kj::Promise<void> RPCHandler::remove(AccountWriter::Server::RemoveContext ctx) {
    return Server::remove(ctx);
}
