@0x80b2e4d12f8358df;

using Account = import "Account.capnp".Account;

interface AccountWriter {
   add @0 (account: Account) -> (result: Bool);
   edit @1 (account: Account) -> (result: Bool);
   delete @2 (title: Text) -> (result: Bool);
}
