//
// Created by Reza Alizadeh Majd on 2019-08-23.
//

#ifndef PX_ACCOUNTS_SERVICE_EVENTSIMULATOR_H
#define PX_ACCOUNTS_SERVICE_EVENTSIMULATOR_H

#include <nng/nng.h>

#include <iostream>
#include <mutex>
#include <thread>
#include <map>
using namespace std;

class EventSimulator {
public:
    explicit EventSimulator() = default;

    void start();
    void stop();

    void registerChannel(const string &topic);
    void publishData(const string &topic, const uint8_t *data, size_t len);

protected:
    std::string extractTopic(uint8_t *data, size_t len);

private:
    bool m_running = false;
    std::thread m_thread;
    nng_socket m_receiverSocket;
    map<string, nng_socket> m_publisherSockets;

};

#endif //PX_ACCOUNTS_SERVICE_EVENTSIMULATOR_H
