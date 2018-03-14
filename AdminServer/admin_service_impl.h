/* admin_service_impl.h

 Generated by phxrpc_pb2service from admin.proto

*/

#pragma once

#include "admin.pb.h"
#include "phxrpc_admin_service.h"


class AdminServerConfig;


typedef struct tagServiceArgs {
    AdminServerConfig *config;
    //You can add other arguments here and initiate in main().
} ServiceArgs_t;


class AdminServiceImpl : public AdminService {
  public:
    AdminServiceImpl(ServiceArgs_t &app_args);
    virtual ~AdminServiceImpl();

    virtual int PhxMqttConnect(const phxrpc::MqttConnectPb &req, phxrpc::MqttConnackPb *resp) override;
    virtual int PhxMqttPublish(const phxrpc::MqttPublishPb &req, phxrpc::MqttPubackPb *resp) override;
    virtual int PhxMqttDisconnect(const phxrpc::MqttDisconnectPb &req) override;

    virtual int PhxEcho(const google::protobuf::StringValue &req, google::protobuf::StringValue *resp) override;
    virtual int RegisterNode(const magna::RegisterNodeRequest &req, magna::RegisterNodeResponse *resp) override;
    virtual int NodeHeatbeat(const magna::NodeHeartbeatRequest &req, magna::NodeHeartbeatResponse *resp) override;
    virtual int RegisterService(const magna::RegisterServiceRequest &req, magna::RegisterServiceResponse *resp) override;
    virtual int ServiceHeatbeat(const magna::ServiceHeartbeatRequest &req, magna::ServiceHeartbeatResponse *resp) override;
    virtual int Handle(const magna::AppRequest &req, magna::AppResponse *resp) override;
    virtual int GetServiceTable(const magna::ServiceTableRequest &req, magna::ServiceTableResponse *resp) override;

  private:
    ServiceArgs_t &args_;
};

