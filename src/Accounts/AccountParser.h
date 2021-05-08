//
// Created by Reza Alizadeh Majd on 11/7/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H

#include <list>

#include "AccountDefinitions.h"
#include "AccountUtils.h"


struct ParserPath {
    string path;
    bool isReadOnly;
};

class AccountParser : public ErrorReporter {
   public:
    explicit AccountParser(const ParserPath & path);

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
        string result = m_path;
        if (!result.empty() && result[result.length() - 1] != '/') {
            result += "/";
        }
        result += strId + ".yaml";
        return result;
    }

   private:
    string m_path;
    bool m_readonly;
};

#endif  // PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
