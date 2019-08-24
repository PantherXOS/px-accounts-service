@0x80b2e4d12f8358df;

using Account = import "Account.capnp".Account;
using AccountReader = import "AccountReader.capnp".AccountReader;

interface AccountWriter extends(AccountReader) {

   add @0 (account: Account) -> (result: Bool, warnings: List(Text));
   edit @1 (title: Text, account: Account) -> (result: Bool, warnings: List(Text));
   remove @2 (title: Text) -> (result: Bool);
}
