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
    return Server::get(ctx);
}

kj::Promise<void> RPCHandler::setStatus(AccountReader::Server::SetStatusContext ctx) {
    return Server::setStatus(ctx);
}

kj::Promise<void> RPCHandler::getStatus(AccountReader::Server::GetStatusContext ctx) {
    return Server::getStatus(ctx);
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
