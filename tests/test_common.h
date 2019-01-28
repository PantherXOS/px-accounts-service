//
// Created by Reza Alizadeh Majd on 2018-12-23.
//

#ifndef PX_ACCOUNTS_SERVICE_TEST_COMMON_H
#define PX_ACCOUNTS_SERVICE_TEST_COMMON_H

#define SERVER_ADDRESS "127.0.0.1:1234"

#ifdef __linux__
#define PASSWORD_SIMULATOR_PATH "unix:/root/.userdata/rpc/password"
#else
#define PASSWORD_SIMULATOR_PATH "127.0.0.1:4444"
#endif

#endif //PX_ACCOUNTS_SERVICE_TEST_COMMON_H
