@0xbff711e54e34c0d2;

struct RPCSecretParam{
    key             @0 :Text;
    value           @1 :Text;
}

struct RPCSecretApplication{
    name            @0 :Text;
    params          @1 :List(RPCSecretParam);
}

struct RPCSecretWallet{
    name            @0 :Text;
    applications    @1 :List(RPCSecretApplication);
}

struct RPCSecretResult {
    success         @0 : Bool;
    error           @1 : Text;
}

interface RPCSecretService {
    getWallets       @0 ()                                                           -> (wallets     : List(Text));
    getApplications  @1 (wallet : Text)                                              -> (applications: List(Text));
    getParams        @2 (wallet : Text, application   : Text)                        -> (params      : List(Text));
    getParam         @3 (wallet : Text, application   : Text, paramKey : Text)       -> (paramValue  : Text);

    delWallet        @4 (wallet : Text)                                              -> (result      : RPCSecretResult);
    delApplication   @5 (wallet : Text, application   : Text)                        -> (result      : RPCSecretResult);
    delParam         @6 (wallet : Text, application   : Text, paramKey : Text)       -> (result      : RPCSecretResult);

    addParam         @7 (wallet : Text, application   : Text, param : RPCSecretParam)-> (result      : RPCSecretResult);
    editParam        @8 (wallet : Text, application   : Text, param : RPCSecretParam)-> (result      : RPCSecretResult);

    allowAccess      @9 (wallet : Text, application   : Text, thirdPartyApp : Text)  -> (result      : RPCSecretResult);
    disallowAccess   @10(wallet : Text, application   : Text, thirdPartyApp : Text)  -> (result      : RPCSecretResult);
    getAccessList    @11(wallet : Text, application   : Text)                        -> (thirdParties: List(Text));
}