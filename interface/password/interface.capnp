using PasswordBook = import "./passwordbook.capnp";
@0xf4b09c356bbce949;


interface PasswordInterface{
    
    createNewEntry @0 (page: Text, line : PasswordBook.Line)->(errorCode : Int16, errorText : Text);
    removeAccess @1  (page: Text, line : PasswordBook.Line)->(errorCode : Int16, errorText : Text);
    getPassword @2 (page : Text, userName: Text)->(password: Text, errorCode : Int16, errorText : Text);
    setPassword @3 (page: Text, userName: Text, oldPassword: Text, newPassword: Text)->(errorCode : Int16, errorText : Text);
    getPageContent @4 (page: Text)->(page: PasswordBook.Page, errorCode : Int16, errorText : Text);
    getAllPageContent @5 ()->(book: PasswordBook.Book, errorCode : Int16, errorText : Text);
    changeMasterPassword @6 (oldPassword: Text, newPassword:Text)->(errorCode : Int16, errorText : Text);
    registerPxAccountService @7(password: Text)->(errorCode : Int16, errorText : Text);
    removePxAccountService @8(password: Text)->(errorCode : Int16, errorText : Text);
    accCreateNewEntry @9 (pack:Text, page: Text, line : PasswordBook.Line)->(errorCode : Int16, errorText : Text);
    accRemoveAnEntry @10 (page: Text, userName : Text)->(errorCode : Int16, errorText : Text);
    accRemoveAccess @11 (page: Text, userName : Text, pack :Text)->(errorCode : Int16, errorText : Text);
    accGetPassword @12 (page : Text, userName: Text)->(password: Text, errorCode : Int16, errorText : Text);
    accSetPassword @13 (page: Text, userName: Text, oldPassword: Text, newPassword: Text)->(errorCode : Int16, errorText : Text);
    accIsRegisterd @14 ()->(isRegisterd : Bool, errorCode : Int16, errorText : Text);
   
}

