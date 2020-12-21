//
// Created by Reza Alizadeh Majd on 2018-12-23.
//

#ifndef PX_ACCOUNTS_SERVICE_TEST_COMMON_H
#define PX_ACCOUNTS_SERVICE_TEST_COMMON_H

#include <iostream>
#include <uuid/uuid.h>
using namespace std;

#define MAIN_SERVER_PATH "127.0.0.1:4001"
#define SECRET_SIMULATOR_PATH "127.0.0.1:4002"
#define EVENTS_SIMULATOR_PATH "127.0.0.1:4003"

namespace TESTCOMMON {
namespace ACCOUNTS {

bool cleanup(const uuid_t &id);
bool cleanup(const string &title);

}  // namespace ACCOUNTS

}  // namespace TESTCOMMON

#endif  // PX_ACCOUNTS_SERVICE_TEST_COMMON_H
