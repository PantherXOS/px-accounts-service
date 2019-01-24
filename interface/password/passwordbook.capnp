@0xa9f82a735c587bd2;

struct Line{
    userName @0 :Text;
    password @1 :Text;
}

struct Page{
    name  @0 :Text;
    lines @1 :List(Line);
}

struct Book{
    pages @0 :List(Page);
}
