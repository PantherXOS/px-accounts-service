//
// Created by Reza Alizadeh Majd on 11/7/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H


#include <list>

#include "AccountDefinitions.h"
#include "AccountUtils.h"

class AccountParser : public ErrorReporter {
   public:
    explicit AccountParser(const string &path, bool isReadonly);

   public:
    bool read(const string &actName, AccountObject &account);
    std::list<AccountObject> list();
    bool write(const string &actName, const AccountObject &account);
    bool remove(const string &actName);

    bool hasAccount(const string &actName);
    inline bool isReadonly() { return m_readonly; }
    inline string path() { return m_path; }

   protected:
    inline string accountPath(const string &actName) { return m_path + actName + ".yaml"; }

   private:
    string m_path;
    bool m_readonly;
};

#endif  // PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
