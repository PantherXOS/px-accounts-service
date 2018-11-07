//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "RPCHandler.h"
#include <kj/async.h>
#include <string>
#include <kj/debug.h>

using namespace std;

kj::Promise<void> RPCHandler::list(AccountReader::Server::ListContext ctx) {
    return Server::list(ctx);
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
