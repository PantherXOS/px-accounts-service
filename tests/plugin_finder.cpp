//
// Created by Reza Alizadeh Majd on 2018-12-04.
//

#include <iostream>
using namespace std;

#include <Plugins/PluginManager.h>

Logger gLogger("plugin-finder");

int main()
{
    GLOG_INIT(LogTarget::CONSOLE, LogLevel::INF);

    for (auto &kv : PluginManager::Instance().registeredPlugins()) {
        auto &plugin = kv.second;
        cout << kv.first << endl;
    }
    return 0;
}
