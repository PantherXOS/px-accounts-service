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

bool PXParser::read(const string &acName, PXParser::AccountObject *ac) {
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
                    for (const auto &param : service.second.as<YAML::Node>()) {
                        ac->services[serviceName][param.first.as<string>()] = param.second.as<string>();
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

bool PXParser::write(const string &acName, const PXParser::AccountObject &ac) {

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
                                emitter << YAML::Key << kv.first;
                                emitter << YAML::Value << YAML::BeginMap;
                                {
                                    for (const auto &p : kv.second) {
                                        emitter << YAML::Key << p.first;
                                        emitter << YAML::Value << p.second;
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

bool PXParser::remove(const string &acName) {
    string acPath = PXParser::fullPath(acName);
    if (PXUTILS::FILE::exists(acPath)) {
        return PXUTILS::FILE::remove(acPath);
    }
    return true;
}