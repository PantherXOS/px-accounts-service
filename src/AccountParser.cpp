//
// Created by Reza Alizadeh Majd on 11/7/18.
//

#include "AccountParser.h"
#include <yaml-cpp/yaml.h>


#define ACCOUNT_KEY          "account"
#define ACCOUNT_TITLE_KEY    "title"
#define ACCOUNT_PROVIDER_KEY "provider"
#define ACCOUNT_ACTIVE_KEY   "active"
#define ACCOUNT_SETTING_KEY  "settings"
#define ACCOUNT_SERVICE_KEY  "services"


/**
 * @param[in] acName name of account we want to read from disk
 * @param[in,out] ac pointer to account object we want to fill it's details
 *
 * @return account read status
 */
bool PXParser::read(const string &acName, AccountObject *ac) {
    string acPath = PXParser::fullPath(acName);
    if (!PXUTILS::FILE::exists(acPath)) {
        return false;
    }
    try {
        YAML::Node cfg = YAML::LoadFile(acPath);
        if (cfg[ACCOUNT_KEY]) {
            ac->title = cfg[ACCOUNT_KEY][ACCOUNT_TITLE_KEY].as<string>();
            ac->provider = cfg[ACCOUNT_KEY][ACCOUNT_PROVIDER_KEY].as<string>();
            ac->is_active = cfg[ACCOUNT_KEY][ACCOUNT_ACTIVE_KEY].as<bool>();

            for (const auto &it : cfg[ACCOUNT_KEY][ACCOUNT_SETTING_KEY]) {
                ac->settings[it.first.as<string>()] = it.second.as<string>();
            }

            for (const auto &it : cfg[ACCOUNT_KEY][ACCOUNT_SERVICE_KEY]) {
                for (const auto &service : it) {
                    auto serviceName = service.first.as<string>();
                    const YAML::Node& params = service.second;
                    ac->services[serviceName] = AccountService();
                    for (const auto& p : params) {
                        ac->services[serviceName][p.first.as<string>()] = p.second.as<string>();
                    }
                }
            }
        }
    }
    catch (const YAML::Exception &ex) {
        cout << ex.what() << endl;
        return false;
    }
    return true;
}

/**
 * @param[in] acName name of account that we want to write to disk
 * @param[out] ac pointer to AccountObject
 *
 * @return account write status
 */
bool PXParser::write(const string &acName, const AccountObject &ac) {

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
                    emitter << YAML::Value << ac.title;

                    emitter << YAML::Key << ACCOUNT_PROVIDER_KEY;
                    emitter << YAML::Value << ac.provider;

                    emitter << YAML::Key << ACCOUNT_ACTIVE_KEY;
                    emitter << YAML::Value << ac.is_active;

                    emitter << YAML::Key << ACCOUNT_SETTING_KEY;
                    emitter << YAML::Value << YAML::BeginMap;
                    {
                        for (const auto &p : ac.settings) {
                            emitter << YAML::Key << p.first;
                            emitter << YAML::Value << p.second;
                        }
                    }
                    emitter << YAML::EndMap;

                    emitter << YAML::Key << ACCOUNT_SERVICE_KEY;
                    emitter << YAML::Value << YAML::BeginSeq;
                    {
                        for (const auto &kv : ac.services) {
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

        PXUTILS::FILE::write(PXParser::fullPath(acName), emitter.c_str());
    }
    catch (YAML::Exception &ex) {
        return false;
    }
    return true;
}

/**
 * @param acName account name we want to remove from disk
 *
 * @return account removal status
 */
bool PXParser::remove(const string &acName) {
    string acPath = PXParser::fullPath(acName);
    if (PXUTILS::FILE::exists(acPath)) {
        return PXUTILS::FILE::remove(acPath);
    }
    return true;
}

/**
 * @param[out] act AccountObject that we want to print
 */
void PXParser::print_account(const AccountObject &act) {
    cout << endl;
    cout << "Account Details: " << endl;
    cout << "Title    : " << act.title << endl;
    cout << "Provider : " << act.provider << endl;
    cout << "Active   : " << act.is_active << endl;
    cout << "Settings : " << act.settings.size() << endl;
    for (const auto &p : act.settings) {
        cout << "\t" << p.first << " -> " << p.second << endl;
    }
    cout << "Services : " << act.services.size() << endl;
    for (const auto &svc : act.services) {
        cout << "\t" << svc.first << endl;
        for (const auto &p : svc.second) {
            cout << "\t\t" << p.first << " -> " << p.second << endl;
        }
    }
    cout << "--------------------------------------" << endl;
}
