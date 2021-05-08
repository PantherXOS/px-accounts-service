//
// Created by Reza Alizadeh Majd on 11/7/18.
//

#include "AccountParser.h"

#include <yaml-cpp/yaml.h>

#define ACCOUNT_KEY "account"
#define ACCOUNT_ID_KEY "id"
#define ACCOUNT_TITLE_KEY "title"
#define ACCOUNT_PROVIDER_KEY "provider"
#define ACCOUNT_ACTIVE_KEY "active"
#define ACCOUNT_SETTING_KEY "settings"
#define ACCOUNT_SERVICE_KEY "services"

AccountParser::AccountParser(const ParserPath &path) : m_path(path.path), m_readonly(path.isReadOnly) {
    GLOG_INF("Account Parser Initiated for:", m_path);
    if (!PXUTILS::FILE::exists(m_path) && !m_readonly) {
        PXUTILS::FILE::mkpath(m_path);
    }
}

bool AccountParser::read(const uuid_t &id, AccountObject &account) {
    auto accountPath = this->accountPath(id);
    if (!PXUTILS::FILE::exists(accountPath)) {
        addError("Account not found: " + uuid_as_string(id));
        GLOG_WRN("Account not found: ", uuid_as_string(id));
        GLOG_INF("path: ", accountPath);
        return false;
    }
    try {
        YAML::Node cfg = YAML::LoadFile(accountPath);
        if (cfg[ACCOUNT_KEY]) {
            account.setId(cfg[ACCOUNT_KEY][ACCOUNT_ID_KEY].as<string>());
            account.title = cfg[ACCOUNT_KEY][ACCOUNT_TITLE_KEY].as<string>();
            account.provider = cfg[ACCOUNT_KEY][ACCOUNT_PROVIDER_KEY].as<string>();
            account.is_active = cfg[ACCOUNT_KEY][ACCOUNT_ACTIVE_KEY].as<bool>();

            for (const auto &it : cfg[ACCOUNT_KEY][ACCOUNT_SETTING_KEY]) {
                account.settings[it.first.as<string>()] = it.second.as<string>();
            }

            for (const auto &it : cfg[ACCOUNT_KEY][ACCOUNT_SERVICE_KEY]) {
                for (const auto &service : it) {
                    auto serviceName = service.first.as<string>();
                    const YAML::Node &params = service.second;
                    account.services[serviceName].init(&account, serviceName);
                    for (const auto &p : params) {
                        account.services[serviceName][p.first.as<string>()] = p.second.as<string>();
                    }
                }
            }
        }
    } catch (const YAML::Exception &ex) {
        GLOG_ERR("failed to parse account details:", ex.what());
        addError("failed to parse account details");
        return false;
    }
    return true;
}

bool AccountParser::read(const string &strId, AccountObject &account) {
    uuid_t accountId;
    if (uuid_from_string(strId, accountId)) {
        return this->read(accountId, account);
    }
    return false;
}

std::list<AccountObject> AccountParser::list() {
    std::list<AccountObject> actList;
    for (const auto &actFile : PXUTILS::FILE::dirfiles(m_path, ".yaml")) {
        auto accountId = actFile.substr(0, actFile.find(".yaml"));
        AccountObject act;
        auto accepted = this->read(accountId, act);
        if (accepted) {
            actList.push_back(act);
        }
    }
    return actList;
}

bool AccountParser::write(const AccountObject &account) {
    if (uuid_is_null(account.id)) {
        addError("account.id is not set");
        GLOG_ERR("account.id is not set");
        return false;
    }
    if (this->isReadonly()) {
        addError("selected parser is readonly");
        GLOG_ERR("call write for readonly parser:", m_path);
        return false;
    }

    YAML::Emitter emitter;
    try {
        emitter << YAML::BeginDoc;
        {
            emitter << YAML::BeginMap;
            {
                emitter << YAML::Key << ACCOUNT_KEY;
                emitter << YAML::Value << YAML::BeginMap;
                {
                    emitter << YAML::Key << ACCOUNT_ID_KEY;
                    emitter << YAML::Value << account.idAsString();

                    emitter << YAML::Key << ACCOUNT_TITLE_KEY;
                    emitter << YAML::Value << account.title;

                    emitter << YAML::Key << ACCOUNT_PROVIDER_KEY;
                    emitter << YAML::Value << account.provider;

                    emitter << YAML::Key << ACCOUNT_ACTIVE_KEY;
                    emitter << YAML::Value << account.is_active;

                    emitter << YAML::Key << ACCOUNT_SETTING_KEY;
                    emitter << YAML::Value << YAML::BeginMap;
                    {
                        for (const auto &p : account.settings) {
                            emitter << YAML::Key << p.first;
                            emitter << YAML::Value << p.second;
                        }
                    }
                    emitter << YAML::EndMap;

                    emitter << YAML::Key << ACCOUNT_SERVICE_KEY;
                    emitter << YAML::Value << YAML::BeginSeq;
                    {
                        for (const auto &kv : account.services) {
                            emitter << YAML::BeginMap;
                            {
                                const auto &svcName = kv.first;
                                const auto &svc = kv.second;

                                emitter << YAML::Key << svcName;
                                emitter << YAML::Value << YAML::BeginMap;
                                {
                                    for (const auto &p : svc) {
                                        const auto &key = p.first;
                                        const auto &val = p.second;
                                        if (!svc.isProtected(key)) {
                                            emitter << YAML::Key << key;
                                            emitter << YAML::Value << val;
                                        }
                                    }
                                }
                                emitter << YAML::EndMap;
                            }
                            emitter << YAML::EndMap;
                        }
                    }
                    emitter << YAML::EndSeq;
                }
                emitter << YAML::EndMap;
            }
            emitter << YAML::EndMap;
        }
        emitter << YAML::EndDoc;

        string actPath = this->accountPath(account.id);
        return PXUTILS::FILE::write(actPath, emitter.c_str());
    } catch (YAML::Exception &ex) {
        GLOG_ERR("failed to write account file:", ex.what());
        addError("failed to write account file");
        return false;
    }
    return true;
}

bool AccountParser::remove(const uuid_t &id) {
    if (this->isReadonly()) {
        addError("attempt to delete readonly account");
        GLOG_ERR("remove method called on readonly parser:", m_path);
        return false;
    }
    string acPath = this->accountPath(id);
    if (!PXUTILS::FILE::exists(acPath)) {
        addError("account not found");
        GLOG_WRN("account not found");
        return false;
    }
    return PXUTILS::FILE::remove(acPath);
}

bool AccountParser::hasAccount(const uuid_t &id) {
    for (const auto &actFile : PXUTILS::FILE::dirfiles(m_path, ".yaml")) {
        auto strCurId = actFile.substr(0, actFile.find(".yaml"));
        uuid_t curId;
        if (uuid_from_string(strCurId, curId) && uuid_compare(id, curId) == 0) {
            return true;
        }
    }
    GLOG_INF("account not found:", uuid_as_string(id));
    return false;
}
