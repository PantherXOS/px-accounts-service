//
// Created by Reza Alizadeh Majd on 11/7/18.
//

#include "AccountParser.h"

#include <yaml-cpp/yaml.h>

#define ACCOUNT_KEY "account"
#define ACCOUNT_TITLE_KEY "title"
#define ACCOUNT_PROVIDER_KEY "provider"
#define ACCOUNT_ACTIVE_KEY "active"
#define ACCOUNT_SETTING_KEY "settings"
#define ACCOUNT_SERVICE_KEY "services"

AccountParser::AccountParser(const string &path, bool isReadonly) : m_path(path), m_readonly(isReadonly) {
    GLOG_INF("Account Parser Initiated for:", m_path);
}

bool AccountParser::read(const string &actName, AccountObject &account) {
    auto accountPath = this->accountPath(actName);
    if (!PXUTILS::FILE::exists(accountPath)) {
        addError("Account not found: " + actName);
        GLOG_WRN("Account not found: ", actName);
        GLOG_INF("path: ", accountPath);
        return false;
    }
    try {
        YAML::Node cfg = YAML::LoadFile(accountPath);
        if (cfg[ACCOUNT_KEY]) {
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

std::list<AccountObject> AccountParser::list() {
    std::list<AccountObject> actList;
    for (const auto &actFile : PXUTILS::FILE::dirfiles(m_path, ".yaml")) {
        auto actName = actFile.substr(0, actFile.find(".yaml"));
        AccountObject act;
        auto accepted = this->read(actName, act);
        if (accepted) {
            actList.push_back(act);
        }
    }
    return actList;
}

bool AccountParser::write(const string &actName, const AccountObject &account) {
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

        string actPath = m_path + actName + ".yaml";
        return PXUTILS::FILE::write(actPath, emitter.c_str());
    } catch (YAML::Exception &ex) {
        GLOG_ERR("failed to write account file:", ex.what());
        addError("failed to write account file");
        return false;
    }
    return true;
}

bool AccountParser::remove(const string &actName) {
    if (this->isReadonly()) {
        addError("attempt to delete readonly account");
        GLOG_ERR("remove method called on readonly parser:", m_path);
        return false;
    }
    string acPath = this->accountPath(actName);
    if (!PXUTILS::FILE::exists(acPath)) {
        addError("account not found");
        GLOG_WRN("account not found");
        return false;
    }
    return PXUTILS::FILE::remove(acPath);
}

bool AccountParser::hasAccount(const string &actName) {
    for (const auto &actFile : PXUTILS::FILE::dirfiles(m_path, ".yaml")) {
        auto curName = actFile.substr(0, actFile.find(".yaml"));
        if (curName == actName) {
            return true;
        }
    }
    GLOG_INF("account not found:", actName);
    return false;
}
