/* admin_client.cpp

 Generated by phxrpc_pb2client from admin.proto

*/

#include "admin_client.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>

#include "phxrpc_admin_stub.h"

#include "phxrpc/rpc.h"


phxrpc::ClientConfig global_adminclient_config_;
phxrpc::ClientMonitorPtr global_adminclient_monitor_;


bool AdminClient::Init(const char *config_file) {
    return global_adminclient_config_.Read(config_file);
}

const char *AdminClient::GetPackageName() {
    const char *ret = global_adminclient_config_.GetPackageName();
    if (strlen(ret) == 0) {
        ret = "magna";
    }
    return ret;
}

AdminClient::AdminClient() {
    static std::mutex monitor_mutex;
    if (!global_adminclient_monitor_.get()) {
        monitor_mutex.lock();
        if (!global_adminclient_monitor_.get()) {
            global_adminclient_monitor_ = phxrpc::MonitorFactory::GetFactory()
                ->CreateClientMonitor(GetPackageName());
        }
        global_adminclient_config_.SetClientMonitor(global_adminclient_monitor_);
        monitor_mutex.unlock();
    }
}

AdminClient::~AdminClient() {
}

int AdminClient::PhxMqttConnect(const phxrpc::MqttConnectPb &req, phxrpc::MqttConnackPb *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.PhxMqttConnect(req, resp);
        }
    }

    return -1;
}

int AdminClient::PhxMqttPublish(const phxrpc::MqttPublishPb &req, phxrpc::MqttPubackPb *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.PhxMqttPublish(req, resp);
        }
    }

    return -1;
}

int AdminClient::PhxMqttDisconnect(const phxrpc::MqttDisconnectPb &req) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.PhxMqttDisconnect(req);
        }
    }

    return -1;
}

int AdminClient::PhxEcho(const google::protobuf::StringValue &req, google::protobuf::StringValue *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.PhxEcho(req, resp);
        }
    }

    return -1;
}

int AdminClient::PhxBatchEcho(const google::protobuf::StringValue &req, google::protobuf::StringValue *resp) {
    int ret = -1;
    size_t echo_server_count = 2;
    uthread_begin;
    for (size_t i{0}; echo_server_count > i; ++i) {
        uthread_t [=, &uthread_s, &ret](void *) {
            const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetByIndex(i);
            if (ep != nullptr) {
                phxrpc::UThreadTcpStream socket;
                if(phxrpc::PhxrpcTcpUtils::Open(&uthread_s, &socket, ep->ip, ep->port,
                            global_adminclient_config_.GetConnectTimeoutMS(), *(global_adminclient_monitor_.get()))) {
                    socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());
                    AdminStub stub(socket, *(global_adminclient_monitor_.get()));
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

int AdminClient::RegisterNode(const magna::RegisterNodeRequest &req, magna::RegisterNodeResponse *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.RegisterNode(req, resp);
        }
    }

    return -1;
}

int AdminClient::NodeHeatbeat(const magna::NodeHeartbeatRequest &req, magna::NodeHeartbeatResponse *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.NodeHeatbeat(req, resp);
        }
    }

    return -1;
}

int AdminClient::RegisterService(const magna::RegisterServiceRequest &req, magna::RegisterServiceResponse *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.RegisterService(req, resp);
        }
    }

    return -1;
}

int AdminClient::ServiceHeatbeat(const magna::ServiceHeartbeatRequest &req, magna::ServiceHeartbeatResponse *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.ServiceHeatbeat(req, resp);
        }
    }

    return -1;
}

int AdminClient::Handle(const magna::AppRequest &req, magna::AppResponse *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.Handle(req, resp);
        }
    }

    return -1;
}

int AdminClient::GetServiceTable(const magna::ServiceTableRequest &req, magna::ServiceTableResponse *resp) {
    const phxrpc::Endpoint_t *ep = global_adminclient_config_.GetRandom();

    if (ep != nullptr) {
        phxrpc::BlockTcpStream socket;
        bool open_ret = phxrpc::PhxrpcTcpUtils::Open(&socket, ep->ip, ep->port,
                    global_adminclient_config_.GetConnectTimeoutMS(), nullptr, 0,
                    *(global_adminclient_monitor_.get()));
        if (open_ret) {
            socket.SetTimeout(global_adminclient_config_.GetSocketTimeoutMS());

            AdminStub stub(socket, *(global_adminclient_monitor_.get()));
            return stub.GetServiceTable(req, resp);
        }
    }

    return -1;
}

