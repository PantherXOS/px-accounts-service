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
    bool read(const uuid_t &id, AccountObject &account);
    bool read(const string &strId, AccountObject &account);
    std::list<AccountObject> list();
    bool write(const AccountObject &account);
    bool remove(const uuid_t &id);

    bool hasAccount(const uuid_t &id);

    inline bool isReadonly() { return m_readonly; }
    inline string path() { return m_path; }

   protected:
    inline string accountPath(const uuid_t &id) {
        auto strId = uuid_as_string(id);
        return m_path + strId + ".yaml";
    }

   private:
    string m_path;
    bool m_readonly;
};

#endif  // PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
