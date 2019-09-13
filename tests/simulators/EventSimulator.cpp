//
// Created by Reza Alizadeh Majd on 2019-08-23.
//

#include "EventSimulator.h"

#include <chrono>

#include <kj/io.h>
#include <capnp/serialize-packed.h>
#include <interface/event.capnp.h>

#include <nng/protocol/pipeline0/pull.h>
#include <nng/protocol/pubsub0/pub.h>

#include <Accounts/AccountUtils.h>

#define EVENT_CHANNEL_BASE_PATH   "~/.userdata/event/channels/"
#define EVENT_RPC_PATH            "~/.userdata/rpc/events"


void EventSimulator::start() {

    int ret = 0;
    if ((ret = nng_pull0_open(&m_receiverSocket)) != 0) {
        cerr << "unable to open socket!" << endl;
        exit(EXIT_FAILURE);
    }

    string listenerPath = "ipc://" + PXUTILS::FILE::abspath(EVENT_RPC_PATH);
    if ((ret = nng_listen(m_receiverSocket, listenerPath.c_str(), nullptr, NNG_FLAG_NONBLOCK)) != 0) {
        cerr << "failed to listen on: '" << listenerPath << "'" << endl;
        exit(EXIT_FAILURE);
    }

    m_running = true;
    m_thread = std::thread([&]() {
        while (m_running) {
            uint8_t *data = nullptr;
            size_t len = 0;
            int ret;
            if ((ret = nng_recv(m_receiverSocket, &data, &len, NNG_FLAG_ALLOC | NNG_FLAG_NONBLOCK)) != 0) {
                if (ret != NNG_EAGAIN) {
                    cerr << "error on receive from pipe: " << ret << endl;
                    m_running = false;
                }
            } else if (len > 0) {
                auto evtTopic = extractTopic(data, len);
                if (!evtTopic.empty()) {
                    publishData(evtTopic, data, len);
                }
            }
            nng_free(data, len);
            std::this_thread::sleep_for(chrono::milliseconds(1));
        }
        nng_close(m_receiverSocket);
        for (auto &kv : m_publisherSockets) {
            nng_close(kv.second);
        }
    });
}

void EventSimulator::stop() {
    m_running = false;
    m_thread.join();
}

void EventSimulator::registerChannel(const string &topic) {
    if (m_publisherSockets.find(topic) == m_publisherSockets.end()) {
        system("mkdir -p " EVENT_CHANNEL_BASE_PATH);
        string channelPath = "ipc://" + PXUTILS::FILE::abspath(EVENT_CHANNEL_BASE_PATH) + topic;
        int ret;
        m_publisherSockets[topic];
        if ((ret = nng_pub0_open(&m_publisherSockets[topic])) != 0) {
            cerr << "failed to open socket" << endl;
            m_publisherSockets.erase(topic);
            exit(EXIT_FAILURE);
        }
        if ((ret = nng_listen(m_publisherSockets[topic], channelPath.c_str(), nullptr, 0)) != 0) {
            cerr << "failed to open channel path: " << channelPath << endl;
            m_publisherSockets.erase(topic);
            exit(EXIT_FAILURE);
        }
    }
}

void EventSimulator::publishData(const string &topic, const uint8_t *data, size_t len) {
    if (m_publisherSockets.find(topic) != m_publisherSockets.end()) {
        int ret = nng_send(m_publisherSockets[topic], (char *) data, len, 0);
        if (ret != 0) {
            cerr << "Error on sending event: " << ret << endl;
        }
    }
}

std::string EventSimulator::extractTopic(uint8_t *data, size_t len) {
    kj::ArrayPtr<uint8_t> dataArray(data, len);
    kj::ArrayInputStream dataStream(dataArray);
    capnp::InputStreamMessageReader dataReader(dataStream);

    EventData::Reader evt = dataReader.getRoot<EventData>();
    return evt.getTopic().cStr();
}
