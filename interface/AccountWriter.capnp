@0xa71c855d0ffaebfe;

using Account = import "Account.capnp".Account;
using AccountReader = import "AccountReader.capnp".AccountReader;

interface AccountWriter extends(AccountReader) {

   add @0 (account: Account) -> (result: Bool, warnings: List(Text));
   edit @1 (id: Text, account: Account) -> (result: Bool, warnings: List(Text));
   remove @2 (id: Text) -> (result: Bool);
}
