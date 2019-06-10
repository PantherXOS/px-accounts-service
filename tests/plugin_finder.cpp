//
// Created by Reza Alizadeh Majd on 2018-12-04.
//

#include <iostream>
using namespace std;

#include <Plugins/PluginManager.h>

int main()
{
    for (auto &kv : PluginManager::Instance().plugins()) {
        auto &plugin = kv.second;
        cout << kv.first << endl;
    }
    return 0;
}
