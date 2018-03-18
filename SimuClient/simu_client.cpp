/* simu_client.cpp

 Generated by phxrpc_pb2client from simu.proto

*/

#include "simu_client.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>

#include "phxrpc_simu_stub.h"

#include "phxrpc/rpc.h"


static phxrpc::ClientConfig global_simuclient_config_;
static phxrpc::ClientMonitorPtr global_simuclient_monitor_;


bool SimuClient::Init(const char *config_file) {
    return global_simuclient_config_.Read(config_file);
}

const char *SimuClient::GetPackageName() {
    const char *ret = global_simuclient_config_.GetPackageName();
    if (strlen(ret) == 0) {
        ret = "magna";
    }
    return ret;
}

SimuClient::SimuClient() {
    static std::mutex monitor_mutex;
    if (!global_simuclient_monitor_.get()) {
        monitor_mutex.lock();
        if (!global_simuclient_monitor_.get()) {
            global_simuclient_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor(GetPackageName());
        }
        global_simuclient_config_.SetClientMonitor(global_simuclient_monitor_);
        monitor_mutex.unlock();
    }
}

SimuClient::~SimuClient() {
}

int SimuClient::PhxMqttConnect(const phxrpc::MqttConnectPb &req, phxrpc::MqttConnackPb *resp) {
    const phxrpc::Endpoint_t *ep = global_simuclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_simuclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_simuclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_simuclient_config_.GetSocketTimeoutMS());

            SimuStub stub(socket, *(global_simuclient_monitor_.get()));
            return stub.PhxMqttConnect(req, resp);
        }
    }

    return -1;
}

int SimuClient::PhxMqttPublish(const phxrpc::MqttPublishPb &req, phxrpc::MqttPubackPb *resp) {
    const phxrpc::Endpoint_t *ep = global_simuclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_simuclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_simuclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_simuclient_config_.GetSocketTimeoutMS());

            SimuStub stub(socket, *(global_simuclient_monitor_.get()));
            return stub.PhxMqttPublish(req, resp);
        }
    }

    return -1;
}

int SimuClient::PhxMqttDisconnect(const phxrpc::MqttDisconnectPb &req) {
    const phxrpc::Endpoint_t *ep = global_simuclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_simuclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_simuclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_simuclient_config_.GetSocketTimeoutMS());

            SimuStub stub(socket, *(global_simuclient_monitor_.get()));
            return stub.PhxMqttDisconnect(req);
        }
    }

    return -1;
}

int SimuClient::PhxEcho(const google::protobuf::StringValue &req, google::protobuf::StringValue *resp) {
    const phxrpc::Endpoint_t *ep = global_simuclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_simuclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_simuclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_simuclient_config_.GetSocketTimeoutMS());

            SimuStub stub(socket, *(global_simuclient_monitor_.get()));
            return stub.PhxEcho(req, resp);
        }
    }

    return -1;
}

int SimuClient::PhxBatchEcho(const google::protobuf::StringValue &req, google::protobuf::StringValue *resp) {
    int ret = -1;
    size_t echo_server_count = 2;
    uthread_begin;
    for (size_t i{0}; echo_server_count > i; ++i) {
        uthread_t [=, &uthread_s, &ret](void *) {
            const phxrpc::Endpoint_t *ep = global_simuclient_config_.GetByIndex(i);
            if (ep != nullptr) {
                phxrpc::UThreadTcpStream socket;
                if(phxrpc::PhxrpcTcpUtils::Open(&uthread_s, &socket, ep->ip, ep->port,
                            global_simuclient_config_.GetConnectTimeoutMS(), *(global_simuclient_monitor_.get()))) {
                    socket.SetTimeout(global_simuclient_config_.GetSocketTimeoutMS());
                    SimuStub stub(socket, *(global_simuclient_monitor_.get()));
                    int this_ret = stub.PhxEcho(req, resp);
                    if (this_ret == 0) {
                        ret = this_ret;
                        uthread_s.Close();
                    }
                }
            }
        };
    }
    uthread_end;
    return ret;
}

int SimuClient::GetRet(const magna::RetRequest &req, magna::RetResponse *resp) {
    const phxrpc::Endpoint_t *ep = global_simuclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_simuclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_simuclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_simuclient_config_.GetSocketTimeoutMS());

            SimuStub stub(socket, *(global_simuclient_monitor_.get()));
            return stub.GetRet(req, resp);
        }
    }

    return -1;
}

