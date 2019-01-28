//
// Created by Reza Alizadeh Majd on 2019-01-28.
//

#include "PasswordSimulator.h"


kj::Promise<void> PasswordSimulator::accCreateNewEntry(PasswordInterface::Server::AccCreateNewEntryContext ctx) {
    // accCreateNewEntry @9 (pack:Text, page: Text, line : PasswordBook.Line)->(errorCode : Int16, errorText : Text);

    auto page = ctx.getParams().getPage().cStr();
    auto line = ctx.getParams().getLine();
    auto username = line.getUserName().cStr();
    auto password = line.getPassword().cStr();

    m_passDict[page][username] = password;
    ctx.getResults().setErrorCode(0);
    ctx.getResults().setErrorText("");

    return  kj::READY_NOW;
}

kj::Promise<void> PasswordSimulator::accIsRegisterd(PasswordInterface::Server::AccIsRegisterdContext ctx) {
    // accIsRegisterd @14 ()->(isRegisterd : Bool, errorCode : Int16, errorText : Text);

    ctx.getResults().setIsRegisterd(m_registered);
    ctx.getResults().setErrorCode(0);
    ctx.getResults().setErrorText("");
    return kj::READY_NOW;
}

kj::Promise<void>
PasswordSimulator::registerPxAccountService(PasswordInterface::Server::RegisterPxAccountServiceContext ctx) {
    // registerPxAccountService @7(password: Text)->(errorCode : Int16, errorText : Text);

    m_registered = true;
    ctx.getResults().setErrorCode(0);
    ctx.getResults().setErrorText("");
    return kj::READY_NOW;
}

kj::Promise<void> PasswordSimulator::accGetPassword(PasswordInterface::Server::AccGetPasswordContext ctx) {
    // accGetPassword @12 (page : Text, userName: Text)->(password: Text, errorCode : Int16, errorText : Text);

    auto page = ctx.getParams().getPage().cStr();
    auto username = ctx.getParams().getUserName().cStr();



    ctx.getResults().setErrorCode(0);
    ctx.getResults().setErrorText("");
    return kj::READY_NOW;
}


