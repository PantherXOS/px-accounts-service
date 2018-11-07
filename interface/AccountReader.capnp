@0xc66e8103dd1abf92;

using Account = import "Account.capnp".Account;

interface AccountReader {
    list    @0 (providerFilter: List(Text), serviceFilter: List(Text)) -> (accounts: List(Text));
    get     @1 (title: Text) -> (account: Account);

    setStatus @2 (title: Text, stat: Account.Status) -> (result: Bool);
    getStatus @3 (title: Text) -> (status: Account.Status);
}
