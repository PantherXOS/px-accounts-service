@0xdc8c8c7471090424;

struct Account {
    title    @0 : Text;
    provider @1 : Text;
    active   @2 : Bool;
    settings @3 : List(Param);
    services @4 : List(Service);

    struct Service {
        name   @0 : Text;
        params @1 : List(Param);
    }

    struct Param {
        key   @0 : Text;
        value @1 : Text;
    }

    enum Status {
        none @0;
        online @1;
        offline @2;
        error @3;
    }
}
