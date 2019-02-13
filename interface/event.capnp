@0xa17314f7ec431084;

struct EventData {
   topic  @0 : Text;
   source @1 : Text;
   time   @2 : UInt64;
   event  @3 : Text;
   params @4 : List(Param);

   struct Param {
      key   @0 : Text;
      value @1 : Text;
   }
}
