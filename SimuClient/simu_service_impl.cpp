/* simu_service_impl.cpp

 Generated by phxrpc_pb2service from simu.proto

*/

#include "simu_service_impl.h"

#include "simu_server_config.h"
#include "simu.pb.h"
#include "phxrpc/file.h"
#include <map>
#include "ReqAnalytics.h"
#include "SimuFunc.h"

SimuServiceImpl::SimuServiceImpl(ServiceArgs_t &app_args)
    : args_(app_args) {
}

SimuServiceImpl::~SimuServiceImpl() {
}

int SimuServiceImpl::PhxMqttConnect(const phxrpc::MqttConnectPb &req, phxrpc::MqttConnackPb *resp) {
    return 0;
}

int SimuServiceImpl::PhxMqttPublish(const phxrpc::MqttPublishPb &req, phxrpc::MqttPubackPb *resp) {
    // TODO: process req.content()

    return -1;
}

int SimuServiceImpl::PhxMqttDisconnect(const phxrpc::MqttDisconnectPb &req) {
    return 0;
}

int SimuServiceImpl::PhxEcho(const google::protobuf::StringValue &req, google::protobuf::StringValue *resp) {
    resp->set_value(req.value());

    return 0;
}
extern std::map<uint32_t, ReqLog> g_rstData;
int SimuServiceImpl::GetRet(const magna::RetRequest &req, magna::RetResponse *resp) {
	uint32_t id = req.id();
	g_rstDataMutex.lock();
	ReqLog & log = g_rstData[id];
	strcpy(log.serviceName, req.servicename().c_str());
	log.clientWeight = req.clientweight();
	log.compLamda = req.complamda();
	log.queueLength = req.queuelength();
	log.queueTime = req.queuetime();
	log.processTime = req.processtime();
	log.localEnd = phxrpc::Timer::GetSteadyClockMS();
	g_rstDataMutex.unlock();
	static int32_t retCount = 0;
	printf("return count: %d, id: %d\n", ++retCount, id);

    return 0;
}

int SimuServiceImpl::ReportLoad(const magna::ReportLoadRequest &req, magna::ReportLoadResponse *resp) {
	LoadLog log;
	log.sendId = g_sendCount;
	log.sendLamda = g_sendLamda;
	log.cpuLoad = req.cpuload();
	log.diskLoad = req.diskload();
	//strcpy(log.ip, req.ip().c_str());
	g_loadLogDataMutex.lock();
	g_loadLogList.push_back(log);
	g_loadLogDataMutex.unlock();
	// printf("%s: cpuLoad: %.2f, diskLoad: %.2f\n", req.ip().c_str(), req.cpuload(), req.diskload());
	resp->set_ack(true);
    return 0;
}